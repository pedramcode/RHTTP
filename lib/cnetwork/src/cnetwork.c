#include "cnetwork.h"


sqlite3* cnetwork_init(){
    sqlite3 *db = NULL;
    char *err = 0;

    int state = sqlite3_open(":memory:", &db);
    if(state) {
        fprintf(stderr, "Can't open database: %s\n", sqlite3_errmsg(db));
        exit(EXIT_FAILURE);
    }

    char* sql_service_table = "CREATE TABLE SERVICE ("
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

    return db;
}

void cnetwork_add_service(sqlite3 *db, Service_t *service){
    char *err = 0;
    char *sql_service_add = (char*) calloc(sizeof(char), 1024);
    sprintf(sql_service_add, "INSERT INTO SERVICE (NAME, DESC, LAST_UPDATE) "
                             "VALUES ('%s', '%s', '%s')", service->name, service->desc, service->last_update);
    int rc = sqlite3_exec(db, sql_service_add, NULL, 0, &err);
    if (rc != SQLITE_OK) {
        fprintf(stderr, "Cannot add service: %s\n", err);
        sqlite3_free(err);
        return;
    }
    free(sql_service_add);
}

unsigned int cnetwork_get_services(sqlite3 *db, Service_t ***result){
    int row_count = 0;
    sqlite3_stmt *stmt;
    char *err = 0;

    char* query = "SELECT ID, NAME, DESC, LAST_UPDATE FROM SERVICE;";
    *result = (Service_t**) calloc(sizeof(Service_t), 0);

    int state = sqlite3_prepare_v2(db, query, -1, &stmt, 0);
    if (state != SQLITE_OK) {
        fprintf(stderr, "Cannot fetch services: %s\n", err);
        sqlite3_free(err);
        return 0;
    }

    state = sqlite3_step(stmt);
    if(state==SQLITE_DONE || state!=SQLITE_ROW){
        sqlite3_finalize(stmt);
        return 0;
    }

    int col_count = sqlite3_column_count(stmt);

    *result = (Service_t**) realloc(*result, sizeof(Service_t*) * (row_count + 1));
    (*result)[row_count] = (Service_t*) malloc(sizeof(Service_t));
    for (int i = 0; i < col_count; i++) {
        char* val = (char*)sqlite3_column_text(stmt, i);
        switch (i) {
            case 0:
                (*result)[row_count]->id = atoi(val);
                break;
            case 1:
                (*result)[row_count]->name = (char*) malloc(sizeof(char) * strlen(val));
                strcpy((*result)[row_count]->name, val);
                break;
            case 2:
                (*result)[row_count]->desc = (char*) malloc(sizeof(char) * strlen(val));
                strcpy((*result)[row_count]->desc, val);
                break;
            case 3:
                (*result)[row_count]->last_update = (char*) malloc(sizeof(char) * strlen(val));
                strcpy((*result)[row_count]->last_update, val);
                break;
            default:
                fprintf(stderr, "Critical service fetch error\n");
                exit(EXIT_FAILURE);
                break;
        }
    }
    row_count++;

    while (sqlite3_step(stmt) == SQLITE_ROW) {
        *result = (Service_t**) realloc(*result, sizeof(Service_t*) * (row_count + 1));
        (*result)[row_count] = (Service_t*) calloc(sizeof(Service_t), 1);
        for (int i = 0; i < col_count; i++) {
            char* val = (char*)sqlite3_column_text(stmt, i);
            switch (i) {
                case 0:
                    (*result)[row_count]->id = atoi(val);
                    break;
                case 1:
                    (*result)[row_count]->name = (char*) malloc(sizeof(char) * strlen(val));
                    strcpy((*result)[row_count]->name, val);
                    break;
                case 2:
                    (*result)[row_count]->desc = (char*) malloc(sizeof(char) * strlen(val));
                    strcpy((*result)[row_count]->desc, val);
                    break;
                case 3:
                    (*result)[row_count]->last_update = (char*) malloc(sizeof(char) * strlen(val));
                    strcpy((*result)[row_count]->last_update, val);
                    break;
                default:
                    fprintf(stderr, "Critical service fetch error\n");
                    exit(EXIT_FAILURE);
                    break;
            }
        }
        row_count++;
    }

    sqlite3_finalize(stmt);
    return row_count;
}
