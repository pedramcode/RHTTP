#include "csocket.h"
#include "credis.h"

static sqlite3 *db;
static rhttp_config_t *config;

int csocket_create() {
    int sockfd = socket(AF_INET, SOCK_STREAM, 0);
    if (sockfd < 0) {
        fprintf(stderr, "Cannot create csocket file\n");
        exit(EXIT_FAILURE);
    }
    int opt = 1;
    if (setsockopt(sockfd, SOL_SOCKET, SO_REUSEADDR | SO_REUSEPORT, &opt, sizeof(opt)) < 0) {
        fprintf(stderr, "Cannot set csocket options\n");
        exit(EXIT_FAILURE);
    }
    return sockfd;
}

typedef struct socket_func_struct {
    int sockfd;
} socket_func_t;

typedef struct req_handler_func_struct {
    int sockfd;
    redisContext *redis_ctx;
} req_handler_func_t;

void *request_handler(void *params) {
    req_handler_func_t *param_obj = (req_handler_func_t *) params;
    int new_socket = param_obj->sockfd;

    char buffer[1024] = {0};
    long val = read(new_socket, buffer, 1024);
    http_prot_t *data = chttp_parse(buffer, REQUEST);
    if (data == NULL) {
        return 0;
    }
    data->sock_id = new_socket;

    Net_Request_t req = {0, data->sock_id, ctime_get_now_str(), 0, data->url, chttp_method_to_str(data->method)};
    cnetwork_add_req(db, &req);

    credis_publish(param_obj->redis_ctx, "REQUEST_PIPE", chttp_to_str(data, REQUEST));
    chttp_free(data);
    return 0;
}

void response_handler(char *channel, char *data) {
    if (strcmp(channel, "RESPONSE_PIPE") != 0) return;
    if (!chttp_is_http(data)) return;
    http_prot_t *http = chttp_parse(data, RESPONSE);
    http->date = ctime_get_now_str();
    http->status_text = chttp_status_to_msg(http->status);
    char *res_str = chttp_to_str(http, RESPONSE);
    send(http->sock_id, res_str, strlen(res_str), 0);
    close(http->sock_id);

    if (config->debug) {
        Net_Request_t *req = cnetwork_get_request_by_sockfd(db, http->sock_id);
        if (req) {
            fprintf(stdout, "\033[0;35m");
            fprintf(stdout, "%s\t", req->path);
            fprintf(stdout, "\033[0;37m");
            fprintf(stdout, "%s\t", req->method);
            fprintf(stdout, "\033[0;36m");
            fprintf(stdout, "%d %s\t", http->status, http->status_text);
            fprintf(stdout, "\033[0;34m");
            fprintf(stdout, "%s\t", http->date);
            if (http->header_len != 0) {
                char *res_server_name = 0;
                for (int i = 0; i < http->header_len; i++) {
                    if (strcmp(http->headers[i]->key, "X-RES-SERVER") == 0) {
                        res_server_name = (char *) calloc(strlen(http->headers[i]->value), sizeof(char));
                        strcpy(res_server_name, http->headers[i]->value);
                        break;
                    }
                }
                if (res_server_name) {
                    fprintf(stdout, "\033[0;33m");
                    fprintf(stdout, "%s\t", res_server_name);
                }
            }
            fprintf(stdout, "\n\033[0m");
        }
    }

    // Remove req from database
    cnetwork_delete_req_by_sockfd(db, http->sock_id);
    chttp_free(http);
}

void *redis_response_subscribe() {
    redisAsyncContext *redis_ctx_async = credis_connect_async("127.0.0.1", 6379);
    credis_subscribe(redis_ctx_async, "RESPONSE_PIPE", response_handler);
    return NULL;
}

_Noreturn void *heartbeat_broadcast(void *redis_content) {
    redisContext *ctx = (redisContext *) redis_content;
    unsigned int beat_inter = 1;
    while (1) {
        credis_publish(ctx, "HEARTBEAT", "BEAT");

        // Remove dead services
        Service_t **services = 0;
        unsigned int service_len = cnetwork_get_services(db, &services);
        for (int x = 0; x < service_len; x++) {
            time_t t = ctime_get_from_str(services[x]->last_update);
            time_t now = ctime_get_now();
            if (t + (beat_inter * config->hb_dead_server_beats) < now) {
                if (config->debug) {
                    fprintf(stdout, "\033[0;31m");
                    fprintf(stdout, "Server left: ");
                    fprintf(stdout, "\033[0;36m");
                    fprintf(stdout, "%s (%s)\n", services[x]->name, services[x]->desc);
                    fprintf(stdout, "\033[0m");
                }
                cnetwork_delete_service_by_id(db, services[x]->id);
            }
        }

        // Take care of rejected requests
        Net_Request_t **requests = 0;
        unsigned int req_num = cnetwork_get_requests(db, &requests);
        for (int i = 0; i < req_num; i++) {
            // Take care of lost requests
            if (requests[i]->rejected >= service_len) {

                char *res = chttpmsg_response("{\"err\":\"Lost request\"}", 418, "application/json",
                                              requests[i]->sockfd);
                credis_publish(ctx, "RESPONSE_PIPE", res);
            }
        }

        // Free allocated memory
        for (int i = 0; i < service_len; i++) {
            free(services[i]);
        }
        free(services);
        for (int i = 0; i < req_num; i++) {
            free(requests[i]);
        }
        free(requests);
        sleep(config->hb_interval);
    }
}

_Noreturn void *timeout_handler(){
    Net_Request_t **requests = 0;
    redisContext *redis_ctx = credis_connect("127.0.0.1", 6379);
    while(1) {
        unsigned int req_num = cnetwork_get_requests(db, &requests);
        for (int i = 0; i < req_num; i++) {
            const long TIME_OUE_SECONDS = config->http_timeout;
            time_t created_at = ctime_get_from_str(requests[i]->created_at);
            if (created_at + TIME_OUE_SECONDS <= ctime_get_now()) {
                char *res = chttpmsg_response("{\"err\":\"Request timeout\"}", 408, "application/json",
                                              requests[i]->sockfd);
                credis_publish(redis_ctx, "RESPONSE_PIPE", res);
            }
        }
        sleep(config->http_timeout);
    }
}

void acknowledge_handler(char *channel, char *data) {
    if (strcmp(channel, "ACKNOWLEDGE_PIPE") != 0) return;
    char *sp = strchr(data, 14);
    if (sp == NULL) return;
    char *service_name = (char *) calloc(sp - data + 1, sizeof(char));
    memcpy(service_name, data, sp - data);
    char *service_desc = (char *) calloc(strlen(data) - strlen(service_name) + 1, sizeof(char));
    memcpy(service_desc, sp + 1, strlen(data) - strlen(service_name));

    Service_t service = {0, service_name, service_desc, ctime_get_now_str()};
    Service_t **services;
    unsigned int service_count = cnetwork_get_services_by_name(db, service_name, &services);
    if (service_count == 0) {
        cnetwork_add_service(db, &service);
//        printf("Service created: %s %s\n", service.name, service.desc);
        if (config->debug) {
            fprintf(stdout, "\033[0;33m");
            fprintf(stdout, "Server connected: ");
            fprintf(stdout, "\033[0;36m");
            fprintf(stdout, "%s (%s)\n", service.name, service.desc);
            fprintf(stdout, "\033[0m");
        }
    } else {
        service.id = services[0]->id;
        cnetwork_update_service(db, &service);
//        printf("Service updated: %s %s\n", service.name, service.desc);
    }

    for (int i = 0; i < service_count; i++) {
        free(services[i]);
    }
    free(services);
    free(service_name);
    free(service_desc);
}

void *redis_acknowledge_subscribe() {
    redisAsyncContext *redis_ctx_async = credis_connect_async("127.0.0.1", 6379);
    credis_subscribe(redis_ctx_async, "ACKNOWLEDGE_PIPE", acknowledge_handler);
    return NULL;
}

void reject_handler(char *channel, char *data) {
    if (strcmp(channel, "REJECT_PIPE") != 0) return;
    unsigned int sockfd = atoi(data);
    cnetwork_inc_req_reject_by_sockfd(db, sockfd);
}

void *redis_reject_subscribe() {
    redisAsyncContext *redis_ctx_async = credis_connect_async("127.0.0.1", 6379);
    credis_subscribe(redis_ctx_async, "REJECT_PIPE", reject_handler);
    return NULL;
}

_Noreturn void *server_listener(void *params) {
    socket_func_t *params_r = (socket_func_t *) params;
    struct sockaddr_in address;
    int addrlen = sizeof(address);

    db = cnetwork_init();

    redisContext *redis_ctx = credis_connect("127.0.0.1", 6379);
    pthread_t subscribe_thread;
    pthread_create(&subscribe_thread, NULL, redis_response_subscribe, NULL);

    pthread_t heartbeat_thread;
    pthread_create(&heartbeat_thread, NULL, heartbeat_broadcast, (void *) redis_ctx);

    pthread_t ack_thread;
    pthread_create(&ack_thread, NULL, redis_acknowledge_subscribe, NULL);

    pthread_t reject_thread;
    pthread_create(&reject_thread, NULL, redis_reject_subscribe, NULL);

    while (1) {
        int new_socket = accept(params_r->sockfd, (struct sockaddr *) &address, (socklen_t *) &addrlen);
        req_handler_func_t req_handler_params = {new_socket, redis_ctx};
        pthread_t handler_thread;
        pthread_create(&handler_thread, NULL, request_handler, (void *) &req_handler_params);
    }
}

void csocket_listen(int sockfd, rhttp_config_t* rconfig) {
    struct sockaddr_in address;
    int addrlen = sizeof(address);
    address.sin_family = AF_INET;
    address.sin_addr.s_addr = INADDR_ANY;
    address.sin_port = htons(rconfig->http_port);
    if (bind(sockfd, (struct sockaddr *) &address, sizeof(address)) < 0) {
        fprintf(stderr, "Cannot bind port to socket\n");
        exit(EXIT_FAILURE);
    }
    if (listen(sockfd, 30) < 0) {
        fprintf(stderr, "Cannot listen to port\n");
        exit(EXIT_FAILURE);
    }

    config = rconfig;

    fprintf(stdout, "\033[0;32m");
    fprintf(stdout, "Server is listening to port ");
    fprintf(stdout, "\033[0;36m");
    fprintf(stdout, "%d ", rconfig->http_port);
    if(config->debug){
        fprintf(stdout, "\033[0;34m");
        fprintf(stdout, "(DEBUG mode)");
    }
    fprintf(stdout, "\n\033[0m");

    pthread_t timeout_thread;
    pthread_create(&timeout_thread, NULL, timeout_handler, NULL);

    socket_func_t params;
    params.sockfd = sockfd;
    pthread_t listener_thread;
    pthread_create(&listener_thread, NULL, server_listener, (void *) &params);
    pthread_join(listener_thread, NULL);
}