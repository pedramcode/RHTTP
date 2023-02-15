#ifndef SERVER_CHTTP_H
#define SERVER_CHTTP_H

#include <string.h>
#include <stdio.h>
#include <stdbool.h>
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

typedef struct http_header_struct {
    char *key;
    char *value;
} http_header_t;

typedef struct http_prot_struct {
    int sock_id;
    http_method_t method;
    char *url;
    unsigned int status;
    char *http_version;
    char *status_text;

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

    http_header_t **headers;
    unsigned int header_len;
} http_prot_t;

/**
 * @brief This function converts HTTP string method to <http_method_t> enum
 * @param type_str String value of HTTP method (Example GET, POST, PUT, ...)
 * @return http_method_t enum
 */
http_method_t chttp_str_to_method(char *type_str);

/**
 * @brief Converts http_method_t structure to HTTP method string
 * @param method A value of http_method_t enum
 * @return HTTP method string value
 */
char *chttp_method_to_str(http_method_t method);

/**
 * @brief This function parses a raw HTTP message and converts it into http_prot_t structure
 * @param data Raw HTTP message string
 * @param type Is an enum value of http_prot_type_t, Request/Response
 * @return http_prot_t structure
 */
http_prot_t *chttp_parse(char *data, http_prot_type_t type);

/**
 * @brief This function serializes value of http_prot_t to HTTP raw message string
 * @param prot_p http_prot_t structure data
 * @param type Is an enum value of http_prot_type_t, Request/Response
 * @return Raw HTTP message string
 */
char *chttp_to_str(http_prot_t *prot_p, http_prot_type_t type);

void chttp_free(http_prot_t *prot);

bool chttp_is_http(char *message);

char *chttp_status_to_msg(int status_code);
#endif
