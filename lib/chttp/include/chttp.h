#ifndef SERVER_CHTTP_H
#define SERVER_CHTTP_H

#include <string.h>
#include <stdio.h>
#include <stdlib.h>

typedef enum http_method_struct {
    GET,
    HEAD,
    POST,
    PUT,
    DELETE,
    CONNECT,
    OPTIONS,
    TRACE,
    PATCH,
} http_method_t;

typedef enum http_prot_type_struct {
    RESPONSE,
    REQUEST
} http_prot_type_t;

typedef enum http_connection_type_struct {
    KEEP_ALIVE,
    CLOSE,
} http_connection_t;

typedef struct http_prot_struct {
    int sock_id;
    http_method_t method;
    char *url;
    unsigned int status;
    char *http_version;
    char* status_text;

    char *host;
    char *user_agent;
    char *accept;
    char *accept_language;
    char *accept_encoding;

    char *date;
    char *content_type;
    unsigned int content_length;
    char *last_modified;
    char *server;
    char *etag;
    char *accept_ranges;

    char *body;
    http_connection_t connection;
} http_prot_t;

http_method_t chttp_str_to_method(char *type_str);
char* chttp_method_to_str(http_method_t method);

http_prot_t chttp_parse(char *data, http_prot_type_t type);
char* chttp_response(http_prot_t prot);

#endif
