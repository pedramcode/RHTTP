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

typedef struct cnetwork_req_struct {
    unsigned int id;
    unsigned int sockfd;
    char *created_at;
    unsigned int rejected;
} Net_Request_t;

sqlite3 *cnetwork_init();

void cnetwork_add_service(sqlite3 *db, Service_t *service);

unsigned int cnetwork_get_services(sqlite3 *db, Service_t ***result);

void cnetwork_get_service_by_id(sqlite3 *db, unsigned int id, Service_t **result);

unsigned int cnetwork_get_services_by_name(sqlite3 *db, char *name, Service_t ***result);

void cnetwork_print_service_info(Service_t *service);

void cnetwork_delete_service_by_id(sqlite3 *db, unsigned int id);

void cnetwork_update_service(sqlite3 *db, Service_t *service);

void cnetwork_add_req(sqlite3 *db, Net_Request_t *req);

void cnetwork_inc_req_reject_by_sockfd(sqlite3 *db, unsigned int sockfd);

void cnetwork_delete_req_by_sockfd(sqlite3 *db, unsigned int sockfd);

unsigned int cnetwork_get_requests(sqlite3 *db, Net_Request_t ***result);

#endif
