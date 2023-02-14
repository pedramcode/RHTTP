#include "cnetwork.h"


sqlite3 *cnetwork_init() {
    sqlite3 *db = NULL;
    char *err = 0;

    int state = sqlite3_open(":memory:", &db);
    if (state) {
        fprintf(stderr, "Can't open database: %s\n", sqlite3_errmsg(db));
        exit(EXIT_FAILURE);
    }

    char *sql_service_table = "CREATE TABLE SERVICE ("
                              "ID INTEGER PRIMARY KEY AUTOINCREMENT NOT NULL, "
                              "NAME TEXT NOT NULL, "
                              "DESC TEXT, "
                              "LAST_UPDATE TEXT)";
    int rc = sqlite3_exec(db, sql_service_table, NULL, 0, &err);
    if (rc != SQLITE_OK) {
        fprintf(stderr, "Cannot create service table: %s\n", err);
        sqlite3_free(err);
        exit(EXIT_FAILURE);
    }

    char *sql_req_table = "CREATE TABLE REQUEST ("
                          "ID INTEGER PRIMARY KEY AUTOINCREMENT NOT NULL, "
                          "SOCKFD INTEGER NOT NULL, "
                          "REJECT INTEGER DEFAULT 0, "
                          "PATH TEXT, "
                          "METHOD TEXT, "
                          "CREATED_AT TEXT)";
    rc = sqlite3_exec(db, sql_req_table, NULL, 0, &err);
    if (rc != SQLITE_OK) {
        fprintf(stderr, "Cannot create request table: %s\n", err);
        sqlite3_free(err);
        exit(EXIT_FAILURE);
    }

    return db;
}

void cnetwork_add_service(sqlite3 *db, Service_t *service) {
    char *err = 0;
    char *sql_service_add = (char *) calloc(1024, sizeof(char));
    sprintf(sql_service_add, "INSERT INTO SERVICE (NAME, DESC, LAST_UPDATE) "
                             "VALUES ('%s', '%s', '%s')", service->name, service->desc, service->last_update);
    int rc = sqlite3_exec(db, sql_service_add, NULL, 0, &err);
    if (rc != SQLITE_OK) {
        fprintf(stderr, "Cannot add service: %s\n", err);
        sqlite3_free(err);
    }
    free(sql_service_add);
}

unsigned int cnetwork_get_services(sqlite3 *db, Service_t ***result) {
    int row_count = 0;
    sqlite3_stmt *stmt;
    char *err = 0;

    char *query = "SELECT ID, NAME, DESC, LAST_UPDATE FROM SERVICE;";
    *result = (Service_t **) calloc(0, sizeof(Service_t));

    int state = sqlite3_prepare_v2(db, query, -1, &stmt, 0);
    if (state != SQLITE_OK) {
        fprintf(stderr, "Cannot fetch services: %s\n", err);
        sqlite3_free(err);
        return 0;
    }

    state = sqlite3_step(stmt);
    if (state == SQLITE_DONE || state != SQLITE_ROW) {
        sqlite3_finalize(stmt);
        return 0;
    }

    int col_count = sqlite3_column_count(stmt);

    *result = (Service_t **) realloc(*result, sizeof(Service_t *) * (row_count + 1));
    (*result)[row_count] = (Service_t *) malloc(sizeof(Service_t));
    for (int i = 0; i < col_count; i++) {
        char *val = (char *) sqlite3_column_text(stmt, i);
        switch (i) {
            case 0:
                (*result)[row_count]->id = atoi(val);
                break;
            case 1:
                (*result)[row_count]->name = (char *) malloc(sizeof(char) * strlen(val));
                strcpy((*result)[row_count]->name, val);
                break;
            case 2:
                (*result)[row_count]->desc = (char *) malloc(sizeof(char) * strlen(val));
                strcpy((*result)[row_count]->desc, val);
                break;
            case 3:
                (*result)[row_count]->last_update = (char *) malloc(sizeof(char) * strlen(val));
                strcpy((*result)[row_count]->last_update, val);
                break;
            default:
                fprintf(stderr, "Critical service fetch error\n");
                exit(EXIT_FAILURE);
        }
    }
    row_count++;

    while (sqlite3_step(stmt) == SQLITE_ROW) {
        *result = (Service_t **) realloc(*result, sizeof(Service_t *) * (row_count + 1));
        (*result)[row_count] = (Service_t *) calloc(1, sizeof(Service_t));
        for (int i = 0; i < col_count; i++) {
            char *val = (char *) sqlite3_column_text(stmt, i);
            switch (i) {
                case 0:
                    (*result)[row_count]->id = atoi(val);
                    break;
                case 1:
                    (*result)[row_count]->name = (char *) malloc(sizeof(char) * strlen(val));
                    strcpy((*result)[row_count]->name, val);
                    break;
                case 2:
                    (*result)[row_count]->desc = (char *) malloc(sizeof(char) * strlen(val));
                    strcpy((*result)[row_count]->desc, val);
                    break;
                case 3:
                    (*result)[row_count]->last_update = (char *) malloc(sizeof(char) * strlen(val));
                    strcpy((*result)[row_count]->last_update, val);
                    break;
                default:
                    fprintf(stderr, "Critical service fetch error\n");
                    exit(EXIT_FAILURE);
            }
        }
        row_count++;
    }

    sqlite3_finalize(stmt);
    return row_count;
}

void cnetwork_get_service_by_id(sqlite3 *db, unsigned int id, Service_t **result) {
    sqlite3_stmt *stmt;
    char *err = 0;

    char query[1024];
    sprintf(query, "SELECT ID, NAME, DESC, LAST_UPDATE FROM SERVICE WHERE ID=%d;", id);

    int state = sqlite3_prepare_v2(db, query, -1, &stmt, 0);
    if (state != SQLITE_OK) {
        fprintf(stderr, "Cannot fetch service: %s\n", err);
        sqlite3_free(err);
        return;
    }

    state = sqlite3_step(stmt);
    if (state == SQLITE_DONE || state != SQLITE_ROW) {
        sqlite3_finalize(stmt);
        return;
    }

    *result = (Service_t *) malloc(sizeof(Service_t));
    int col_count = sqlite3_column_count(stmt);

    for (int i = 0; i < col_count; i++) {
        char *val = (char *) sqlite3_column_text(stmt, i);
        switch (i) {
            case 0:
                (*result)->id = atoi(val);
                break;
            case 1:
                (*result)->name = (char *) malloc(sizeof(char) * strlen(val));
                strcpy((*result)->name, val);
                break;
            case 2:
                (*result)->desc = (char *) malloc(sizeof(char) * strlen(val));
                strcpy((*result)->desc, val);
                break;
            case 3:
                (*result)->last_update = (char *) malloc(sizeof(char) * strlen(val));
                strcpy((*result)->last_update, val);
                break;
            default:
                fprintf(stderr, "Critical service fetch error\n");
                exit(EXIT_FAILURE);
        }
    }
}


unsigned int cnetwork_get_services_by_name(sqlite3 *db, char *name, Service_t ***result) {
    int row_count = 0;
    sqlite3_stmt *stmt;
    char *err = 0;

    char query[1024];
    sprintf(query, "SELECT ID, NAME, DESC, LAST_UPDATE FROM SERVICE WHERE NAME like '%s';", name);
    *result = (Service_t **) calloc(0, sizeof(Service_t));

    int state = sqlite3_prepare_v2(db, query, -1, &stmt, 0);
    if (state != SQLITE_OK) {
        fprintf(stderr, "Cannot fetch services: %s\n", err);
        sqlite3_free(err);
        return 0;
    }

    state = sqlite3_step(stmt);
    if (state == SQLITE_DONE || state != SQLITE_ROW) {
        sqlite3_finalize(stmt);
        return 0;
    }

    int col_count = sqlite3_column_count(stmt);

    *result = (Service_t **) realloc(*result, sizeof(Service_t *) * (row_count + 1));
    (*result)[row_count] = (Service_t *) malloc(sizeof(Service_t));
    for (int i = 0; i < col_count; i++) {
        char *val = (char *) sqlite3_column_text(stmt, i);
        switch (i) {
            case 0:
                (*result)[row_count]->id = atoi(val);
                break;
            case 1:
                (*result)[row_count]->name = (char *) malloc(sizeof(char) * strlen(val));
                strcpy((*result)[row_count]->name, val);
                break;
            case 2:
                (*result)[row_count]->desc = (char *) malloc(sizeof(char) * strlen(val));
                strcpy((*result)[row_count]->desc, val);
                break;
            case 3:
                (*result)[row_count]->last_update = (char *) malloc(sizeof(char) * strlen(val));
                strcpy((*result)[row_count]->last_update, val);
                break;
            default:
                fprintf(stderr, "Critical service fetch error\n");
                exit(EXIT_FAILURE);
        }
    }
    row_count++;

    while (sqlite3_step(stmt) == SQLITE_ROW) {
        *result = (Service_t **) realloc(*result, sizeof(Service_t *) * (row_count + 1));
        (*result)[row_count] = (Service_t *) calloc(1, sizeof(Service_t));
        for (int i = 0; i < col_count; i++) {
            char *val = (char *) sqlite3_column_text(stmt, i);
            switch (i) {
                case 0:
                    (*result)[row_count]->id = atoi(val);
                    break;
                case 1:
                    (*result)[row_count]->name = (char *) malloc(sizeof(char) * strlen(val));
                    strcpy((*result)[row_count]->name, val);
                    break;
                case 2:
                    (*result)[row_count]->desc = (char *) malloc(sizeof(char) * strlen(val));
                    strcpy((*result)[row_count]->desc, val);
                    break;
                case 3:
                    (*result)[row_count]->last_update = (char *) malloc(sizeof(char) * strlen(val));
                    strcpy((*result)[row_count]->last_update, val);
                    break;
                default:
                    fprintf(stderr, "Critical service fetch error\n");
                    exit(EXIT_FAILURE);
            }
        }
        row_count++;
    }

    sqlite3_finalize(stmt);
    return row_count;
}

void cnetwork_print_service_info(Service_t *service) {
    printf("#%d\t[%s]\t[%s]\t[%s]\n", service->id, service->name, service->desc, service->last_update);
}

void cnetwork_delete_service_by_id(sqlite3 *db, unsigned int id) {
    char *err = 0;
    char *sql_service_add = (char *) calloc(1024, sizeof(char));
    sprintf(sql_service_add, "DELETE FROM SERVICE WHERE id=%d", id);
    int rc = sqlite3_exec(db, sql_service_add, NULL, 0, &err);
    if (rc != SQLITE_OK) {
        fprintf(stderr, "Cannot delete service: %s\n", err);
        sqlite3_free(err);
    }
    free(sql_service_add);
}

void cnetwork_update_service(sqlite3 *db, Service_t *service) {
    char *err = 0;
    char *sql_service_update = (char *) calloc(1024, sizeof(char));
    sprintf(sql_service_update, "UPDATE SERVICE SET NAME='%s', DESC='%s', LAST_UPDATE='%s' WHERE id=%d", service->name,
            service->desc, service->last_update, service->id);
    int rc = sqlite3_exec(db, sql_service_update, NULL, 0, &err);
    if (rc != SQLITE_OK) {
        fprintf(stderr, "Cannot update service: %s\n", err);
        sqlite3_free(err);
    }
    free(sql_service_update);
}

void cnetwork_add_req(sqlite3 *db, Net_Request_t *req) {
    char *err = 0;
    char *sql_req_add = (char *) calloc(1024, sizeof(char));
    sprintf(sql_req_add, "INSERT INTO REQUEST (SOCKFD, CREATED_AT, PATH, METHOD) VALUES (%d, '%s', '%s', '%s')", req->sockfd,
            req->created_at, req->path, req->method);
    int rc = sqlite3_exec(db, sql_req_add, NULL, 0, &err);
    if (rc != SQLITE_OK) {
        fprintf(stderr, "Cannot add request: %s\n", err);
        sqlite3_free(err);
    }
    free(sql_req_add);
}

void cnetwork_delete_req_by_sockfd(sqlite3 *db, unsigned int sockfd) {
    char *err = 0;
    char *sql_req_del = (char *) calloc(1024, sizeof(char));
    sprintf(sql_req_del, "DELETE FROM REQUEST WHERE SOCKFD=%d", sockfd);
    int rc = sqlite3_exec(db, sql_req_del, NULL, 0, &err);
    if (rc != SQLITE_OK) {
        fprintf(stderr, "Cannot delete request: %s\n", err);
        sqlite3_free(err);
    }
    free(sql_req_del);
}

void cnetwork_inc_req_reject_by_sockfd(sqlite3 *db, unsigned int sockfd) {
    char *err = 0;
    char *sql_req_update = (char *) calloc(1024, sizeof(char));
    sprintf(sql_req_update, "UPDATE REQUEST SET REJECT=REJECT+1 WHERE SOCKFD=%d", sockfd);
    int rc = sqlite3_exec(db, sql_req_update, NULL, 0, &err);
    if (rc != SQLITE_OK) {
        fprintf(stderr, "Cannot update request rejects: %s\n", err);
        sqlite3_free(err);
    }
    free(sql_req_update);
}

unsigned int cnetwork_get_requests(sqlite3 *db, Net_Request_t ***result) {
    int row_count = 0;
    sqlite3_stmt *stmt;
    char *err = 0;

    char *query = "SELECT ID, SOCKFD, REJECT, CREATED_AT, PATH, METHOD FROM REQUEST;";
    *result = (Net_Request_t **) calloc(0, sizeof(Net_Request_t));

    int state = sqlite3_prepare_v2(db, query, -1, &stmt, 0);
    if (state != SQLITE_OK) {
        fprintf(stderr, "Cannot fetch requests: %s\n", err);
        sqlite3_free(err);
        return 0;
    }

    state = sqlite3_step(stmt);
    if (state == SQLITE_DONE || state != SQLITE_ROW) {
        sqlite3_finalize(stmt);
        return 0;
    }

    int col_count = sqlite3_column_count(stmt);

    *result = (Net_Request_t **) realloc(*result, sizeof(Net_Request_t *) * (row_count + 1));
    (*result)[row_count] = (Net_Request_t *) malloc(sizeof(Net_Request_t));
    for (int i = 0; i < col_count; i++) {
        char *val = (char *) sqlite3_column_text(stmt, i);
        switch (i) {
            case 0:
                (*result)[row_count]->id = atoi(val);
                break;
            case 1:
                (*result)[row_count]->sockfd = atoi(val);
                break;
            case 2:
                (*result)[row_count]->rejected = atoi(val);
                break;
            case 3:
                (*result)[row_count]->created_at = (char *) malloc(sizeof(char) * strlen(val));
                strcpy((*result)[row_count]->created_at, val);
                break;
            case 4:
                (*result)[row_count]->path = (char *) malloc(sizeof(char) * strlen(val));
                strcpy((*result)[row_count]->path, val);
                break;
            case 5:
                (*result)[row_count]->method = (char *) malloc(sizeof(char) * strlen(val));
                strcpy((*result)[row_count]->method, val);
                break;
            default:
                fprintf(stderr, "Critical request fetch error\n");
                exit(EXIT_FAILURE);
        }
    }
    row_count++;

    while (sqlite3_step(stmt) == SQLITE_ROW) {
        *result = (Net_Request_t **) realloc(*result, sizeof(Net_Request_t *) * (row_count + 1));
        (*result)[row_count] = (Net_Request_t *) malloc(sizeof(Net_Request_t));
        for (int i = 0; i < col_count; i++) {
            char *val = (char *) sqlite3_column_text(stmt, i);
            switch (i) {
                case 0:
                    (*result)[row_count]->id = atoi(val);
                    break;
                case 1:
                    (*result)[row_count]->sockfd = atoi(val);
                    break;
                case 2:
                    (*result)[row_count]->rejected = atoi(val);
                    break;
                case 3:
                    (*result)[row_count]->created_at = (char *) malloc(sizeof(char) * strlen(val));
                    strcpy((*result)[row_count]->created_at, val);
                    break;
                case 4:
                    (*result)[row_count]->path = (char *) malloc(sizeof(char) * strlen(val));
                    strcpy((*result)[row_count]->path, val);
                    break;
                case 5:
                    (*result)[row_count]->method = (char *) malloc(sizeof(char) * strlen(val));
                    strcpy((*result)[row_count]->method, val);
                    break;
                default:
                    fprintf(stderr, "Critical request fetch error\n");
                    exit(EXIT_FAILURE);
            }
        }
        row_count++;
    }

    sqlite3_finalize(stmt);
    return row_count;
}

Net_Request_t *cnetwork_get_request_by_sockfd(sqlite3 *db, unsigned int sockfd) {
    sqlite3_stmt *stmt;
    char *err = 0;

    char query[1024];
    sprintf(query, "SELECT ID, SOCKFD, REJECT, CREATED_AT, PATH, METHOD FROM REQUEST WHERE SOCKFD=%d;", sockfd);

    int state = sqlite3_prepare_v2(db, query, -1, &stmt, 0);
    if (state != SQLITE_OK) {
        fprintf(stderr, "Cannot fetch request: %s\n", err);
        sqlite3_free(err);
        return 0;
    }

    state = sqlite3_step(stmt);
    if (state == SQLITE_DONE || state != SQLITE_ROW) {
        sqlite3_finalize(stmt);
        return 0;
    }

    Net_Request_t *req = (Net_Request_t *) malloc(sizeof(Net_Request_t));
    int col_count = sqlite3_column_count(stmt);
    for (int i = 0; i < col_count; i++) {
        char *val = (char *) sqlite3_column_text(stmt, i);
        switch (i) {
            case 0:
                req->id = atoi(val);
                break;
            case 1:
                req->sockfd = atoi(val);
                break;
            case 2:
                req->rejected = atoi(val);
                break;
            case 3:
                req->created_at = (char *) malloc(sizeof(char) * strlen(val));
                strcpy(req->created_at, val);
                break;
            case 4:
                req->path = (char *) malloc(sizeof(char) * strlen(val));
                strcpy(req->path, val);
                break;
            case 5:
                req->method = (char *) malloc(sizeof(char) * strlen(val));
                strcpy(req->method, val);
                break;
            default:
                fprintf(stderr, "Critical request fetch error\n");
                exit(EXIT_FAILURE);
        }
    }
    return req;
}