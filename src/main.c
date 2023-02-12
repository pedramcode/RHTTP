#include <stdio.h>
#include <stdlib.h>
#include <cargs.h>
#include "csocket.h"

#include "cnetwork.h"


int main(int argc, char *argv[]) {
//    CargsMap_t map[] = {
//            {"port", 'p', true, "Server port number"},
//            {"workers", 'w', false, "Server workers number"},
//    };
//    unsigned int map_size = 2;
//    cargs_parse(map, map_size, argc, argv);
//    cargs_validate(map, map_size, true);
//
//    const int port = atoi(cargs_get(map, map_size, "port"));
//
//    int server = csocket_create();
//    csocket_listen(server, port);

    sqlite3 *db = cnetwork_init();
    cnetwork_add_service(db, &(Service_t) {
            0,
            "user-service",
            "User service (python)",
            "2022-01-01 12:12:12",
    });
    cnetwork_add_service(db, &(Service_t) {
            0,
            "blog-service",
            "Blog service (nodejs)",
            "2022-02-01 6:1:4",
    });
    cnetwork_add_service(db, &(Service_t) {
            0,
            "auth-service",
            "Auth service (GoLang)",
            "2022-04-01 6:1:4",
    });
    cnetwork_add_service(db, &(Service_t) {
            0,
            "shopping-service",
            "Shopping service (python)",
            "2021-02-01 6:1:4",
    });

    Service_t **services = NULL;
    unsigned int count = cnetwork_get_services(db, &services);
    for(int x = 0 ; x < count ; x++){
        Service_t *service = services[x];
        printf("#%d\t[%s]\t[%s]\t[%s]\n", service->id, service->name, service->desc, service->last_update);
    }
    return EXIT_SUCCESS;
}