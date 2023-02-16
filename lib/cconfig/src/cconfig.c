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
    (*config)->name = (char*) malloc(strlen(server_name) + 1);
    strcpy((*config)->name, server_name);

    const char* http_host = "127.0.0.1";
    config_lookup_string(&conf, "server.http.host", &http_host);
    (*config)->http_host = (char*) malloc(strlen(http_host) + 1);
    strcpy((*config)->http_host, http_host);

    if(!config_lookup_int(&conf, "server.http.port", &(*config)->http_port)) (*config)->http_port = 1998;

    const char* redis_host = "127.0.0.1";
    config_lookup_string(&conf, "server.redis.host", &redis_host);
    (*config)->redis_host = (char*) malloc(strlen(redis_host) + 1);
    strcpy((*config)->redis_host, redis_host);

    if(!config_lookup_int(&conf, "server.redis.port", &(*config)->redis_port)) (*config)->redis_port = 6379;

    if(!config_lookup_bool(&conf, "server.debug", &(*config)->debug)) (*config)->debug = false;

    if(!config_lookup_int(&conf, "server.http.timeout", &(*config)->http_timeout)) (*config)->http_timeout = 120;

    if(!config_lookup_int(&conf, "server.heartbeat.interval", &(*config)->hb_interval)) (*config)->hb_interval = 10;

    if(!config_lookup_int(&conf, "server.heartbeat.dead_server_beats", &(*config)->hb_dead_server_beats)) (*config)->hb_dead_server_beats = 3;

    config_destroy(&conf);
}

