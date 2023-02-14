#include <stdio.h>
#include <stdlib.h>
#include <cargs.h>
#include "csocket.h"


int main(int argc, char *argv[]) {
    CargsMap_t map[] = {
            {"port", 'p', true, "Server port number"},
            {"debug", 'd', false, "Debug mode (true|false)"},
    };
    unsigned int map_size = 2;
    cargs_parse(map, map_size, argc, argv);
    cargs_validate(map, map_size, true);

    const int port = atoi(cargs_get(map, map_size, "port"));

    bool debug = false;
    char* debug_str = cargs_get(map, map_size, "debug");
    if(debug_str){
        if(strcmp(debug_str, "true")==0){
            debug = true;
        }
    }
    int server = csocket_create(debug);
    csocket_listen(server, port);
    return EXIT_SUCCESS;
}