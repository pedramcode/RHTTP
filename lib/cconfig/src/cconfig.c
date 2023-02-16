#include "cconfig.h"



void cconfig_init(char *path, rhttp_config_t **config) {
    config_t conf;
    config_init(&conf);
    int read_stat = config_read_file(&conf, path);
    if (!read_stat) {
        fprintf(stderr, "%s:%d - %s\n", config_error_file(&conf), config_error_line(&conf), config_error_text(&conf));
        config_destroy(&conf);
        exit(EXIT_FAILURE);
    }

    *config = (rhttp_config_t*) calloc(1, sizeof(rhttp_config_t));

    const char* server_name = "RHTTP Server";
    config_lookup_string(&conf, "server.name", &server_name);
    (*config)->name = (char*) calloc(strlen(server_name)+1, sizeof(char));
    memcpy((*config)->name, server_name, strlen(server_name));

    const char* http_host = "127.0.0.1";
    config_lookup_string(&conf, "server.http.host", &http_host);
    (*config)->http_host = (char*) calloc(strlen(http_host)+1, sizeof(char));
    memcpy((*config)->http_host, http_host, strlen(http_host));

    if(!config_lookup_int(&conf, "server.http.port", &(*config)->http_port)) (*config)->http_port = 1998;

    const char* redis_host = "127.0.0.1";
    config_lookup_string(&conf, "server.redis.host", &redis_host);
    (*config)->redis_host = (char*) calloc(strlen(redis_host)+1, sizeof(char));
    memcpy((*config)->redis_host, redis_host, strlen(redis_host));

    if(!config_lookup_int(&conf, "server.redis.port", &(*config)->redis_port)) (*config)->redis_port = 6379;

    if(!config_lookup_bool(&conf, "server.debug", &(*config)->debug)) (*config)->debug = false;

    config_destroy(&conf);
}

