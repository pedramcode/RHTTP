#ifndef SERVER_CNETWORK_H
#define SERVER_CNETWORK_H

#include "stdio.h"
#include "stdlib.h"
#include "sqlite3.h"
#include "string.h"

typedef struct cnetwork_service_struct {
    unsigned int id;
    char *name;
    char *desc;
    char *last_update;
} Service_t;

sqlite3 *cnetwork_init();

void cnetwork_add_service(sqlite3 *db, Service_t *service);

unsigned int cnetwork_get_services(sqlite3 *db, Service_t ***result);

void cnetwork_get_service_by_id(sqlite3 *db, unsigned int id, Service_t **result);

unsigned int cnetwork_get_services_by_name(sqlite3 *db, char *name, Service_t ***result);

void cnetwork_print_service_info(Service_t *service);

#endif
