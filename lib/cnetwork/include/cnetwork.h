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

#endif
