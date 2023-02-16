#ifndef RHTTP_CCONFIG_H
#define RHTTP_CCONFIG_H

#include <stdlib.h>
#include <stdio.h>
#include <stdbool.h>
#include <string.h>
#include <libconfig.h>

typedef struct cconfig_struct {
    char *name;
    bool debug;

    char *http_host;
    int http_port;

    char *redis_host;
    int redis_port;
} rhttp_config_t;

void cconfig_init(char *path, rhttp_config_t **config);

#endif
