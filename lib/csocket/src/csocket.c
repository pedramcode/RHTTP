#include "csocket.h"
#include "credis.h"


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
    redisContext* redis_ctx;
} req_handler_func_t;

void* request_handler(void* params){
    req_handler_func_t* param_obj = (req_handler_func_t*) params;
    int new_socket = param_obj->sockfd;

    char buffer[1024] = { 0 };
    long val = read(new_socket, buffer, 1024);
    http_prot_t* data = chttp_parse(buffer, REQUEST);
    data->sock_id = new_socket;
    credis_publish(param_obj->redis_ctx, "REQUEST_PIPE", chttp_to_str(data, REQUEST));
    free(data);

    http_prot_t res = {0};
    res.http_version = "HTTP/1.1";
    res.status = 200;
    res.status_text = "OK";
    res.connection = CLOSE;
    res.server = "My C server";
    res.content_type = "application/json";
    res.body = "{\"backend\":\"Wow it's really C!\", \"benefits\":[\"Speed\",\"Fast\",\"Quick\"]}";
    char* res_str = chttp_to_str(&res, RESPONSE);

    send(new_socket, res_str, strlen(res_str), 0);
    close(new_socket);
    return 0;
}

_Noreturn void* server_listener(void* params){
    socket_func_t *params_r = (socket_func_t*) params;
    struct sockaddr_in address;
    int addrlen = sizeof(address);

    redisContext* redis_ctx = credis_connect("127.0.0.1", 6379);

    while(1) {
        int new_socket = accept(params_r->sockfd, (struct sockaddr *) &address, (socklen_t *) &addrlen);
        req_handler_func_t req_handler_params = {new_socket, redis_ctx};
        pthread_t handler_thread;
        pthread_create(&handler_thread, NULL, request_handler, (void*) &req_handler_params);
    }
}

void csocket_listen(int sockfd, const unsigned int port) {
    struct sockaddr_in address;
    int addrlen = sizeof(address);
    address.sin_family = AF_INET;
    address.sin_addr.s_addr = INADDR_ANY;
    address.sin_port = htons(port);
    if (bind(sockfd, (struct sockaddr *) &address, sizeof(address)) < 0) {
        fprintf(stderr, "Cannot bind port to socket\n");
        exit(EXIT_FAILURE);
    }
    if (listen(sockfd, 30) < 0) {
        fprintf(stderr, "Cannot listen to port\n");
        exit(EXIT_FAILURE);
    }
    fprintf(stdout, "Server is listening to port %d\n", port);

    socket_func_t params;
    params.sockfd = sockfd;
    pthread_t listener_thread;
    pthread_create(&listener_thread, NULL, server_listener, (void*) &params);
    pthread_join(listener_thread, NULL);
}