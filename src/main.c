#include <stdio.h>
#include <stdlib.h>
#include <cargs.h>
#include "csocket.h"


int main(int argc, char *argv[]) {
    CargsMap_t map[] = {
            {"port", 'p', true, "Server port number"},
    };
    unsigned int map_size = 2;
    cargs_parse(map, map_size, argc, argv);
    cargs_validate(map, map_size, true);

    const int port = atoi(cargs_get(map, map_size, "port"));

    int server = csocket_create();
    csocket_listen(server, port);
    return EXIT_SUCCESS;
}