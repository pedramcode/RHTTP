#include <stdio.h>
#include <stdlib.h>
#include <cargs.h>
#include "csocket.h"
#include "cconfig.h"


int main(int argc, char *argv[]) {
    CargsMap_t map[] = {
            {"config", 'c', true, "RHTTP configuration file path"},
    };
    unsigned int map_size = 1;
    cargs_parse(map, map_size, argc, argv);
    cargs_validate(map, map_size, true);

    rhttp_config_t* rhttp_config;
    cconfig_init(cargs_get(map, map_size, "config"), &rhttp_config);

    int server = csocket_create();
    csocket_listen(server, rhttp_config);
    return EXIT_SUCCESS;
}