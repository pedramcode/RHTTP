#include "chttp.h"

http_prot_t *chttp_parse(char *data, http_prot_type_t type) {
    if(!chttp_is_http(data)){
        return 0;
    }
    http_prot_t *prot = (http_prot_t *) calloc(1, sizeof(http_prot_t));

    char *split_point = NULL;
    split_point = strstr(data, "\r\n\r\n");
    char *header_part = NULL;
    char *body_part = NULL;

    prot->headers = (http_header_t **) calloc(0, sizeof(http_header_t *));
    prot->header_len = 0;

    if (split_point != NULL) {
        header_part = (char *) calloc(split_point - data + 1, sizeof(char));
        memcpy(header_part, &data[0], split_point - data);
        char *start_body_pnt = split_point + 4;
        body_part = (char *) calloc(strlen(data) - 4 - strlen(header_part), sizeof(char));
        memcpy(body_part, start_body_pnt, strlen(data) - 4 - strlen(header_part));
    } else {
        header_part = (char *) calloc(strlen(data), sizeof(char));
        strcpy(header_part, data);
    }

    char *tok = NULL;
    char *next_line = NULL;
    char line_del[] = "\r\n";
    tok = strtok_r(header_part, line_del, &next_line);
    int line_num = 0;

    while (tok != NULL) {
        // first header line
        if (line_num == 0) {
            char *next_dt = NULL;
            char *dt = strtok_r(tok, " ", &next_dt);
            int header_token = 0;
            if (type == RESPONSE) {
                prot->status_text = (char *) calloc(0, sizeof(char));
            }
            while (dt != NULL) {
                if (type == REQUEST) {
                    if (header_token == 0) {
                        char *method_str = (char *) calloc(strlen(dt), sizeof(char));
                        strcpy(method_str, dt);
                        prot->method = chttp_str_to_method(method_str);
                        free(method_str);
                    } else if (header_token == 1) {
                        prot->url = (char *) calloc(strlen(dt), sizeof(char));
                        strcpy(prot->url, dt);
                    } else if (header_token == 2) {
                        prot->http_version = (char *) calloc(strlen(dt), sizeof(char));
                        strcpy(prot->http_version, dt);
                    }
                } else if (type == RESPONSE) {
                    if (header_token == 0) {
                        prot->http_version = (char *) calloc(strlen(dt), sizeof(char));
                        strcpy(prot->http_version, dt);
                    } else if (header_token == 1) {
                        prot->status = atoi(dt);
                    } else {
                        prot->status_text = (char *) realloc(prot->status_text, strlen(dt) + 1);
                        strcat(prot->status_text, dt);
                        strcat(prot->status_text, " ");
                    }
                }
                dt = strtok_r(NULL, " ", &next_dt);
                header_token++;
            }
        } else {
            char *next_dt = NULL;
            char delim[] = " ";
            char *dt = strtok_r(tok, delim, &next_dt);
            char *key;
            char *value = (char *) calloc(0, sizeof(char));
            int i = 0;
            while (dt != NULL) {
                if (i == 0) {
                    key = (char *) calloc(strlen(dt) - 1, sizeof(char));
                    strncpy(key, dt, strlen(dt) - 1);
                } else {
                    value = (char *) realloc(value, strlen(value) + strlen(dt) + 2);
                    strcat(value, dt);
                    if (strlen(dt) == 1) {
                        value = (char *) realloc(value, strlen(value) + 1);
                        strcat(value, " ");
                    }
                    strcat(value, delim);
                }
                dt = strtok_r(NULL, delim, &next_dt);
                i++;
            }
            unsigned int last = strlen(value);
            value = (char *) realloc(value, strlen(value) - 2);
            value[last - 1] = 0;

            if (strcmp(key, "Host") == 0) {
                prot->host = (char *) calloc(strlen(value), sizeof(char));
                strcpy(prot->host, value);
            } else if (strcmp(key, "User-Agent") == 0) {
                prot->user_agent = (char *) calloc(strlen(value), sizeof(char));
                strcpy(prot->user_agent, value);
            } else if (strcmp(key, "Accept") == 0) {
                prot->accept = (char *) calloc(strlen(value), sizeof(char));
                strcpy(prot->accept, value);
            } else if (strcmp(key, "Accept-Language") == 0) {
                prot->accept_language = (char *) calloc(strlen(value), sizeof(char));
                strcpy(prot->accept_language, value);
            } else if (strcmp(key, "Accept-Encoding") == 0) {
                prot->accept_encoding = (char *) calloc(strlen(value), sizeof(char));
                strcpy(prot->accept_encoding, value);
            } else if (strcmp(key, "Date") == 0) {
                prot->date = (char *) calloc(strlen(value), sizeof(char));
                strcpy(prot->date, value);
            } else if (strcmp(key, "Content-Type") == 0) {
                prot->content_type = (char *) calloc(strlen(value), sizeof(char));
                strcpy(prot->content_type, value);
            } else if (strcmp(key, "Content-Length") == 0) {
                prot->content_length = atoi(value);
            } else if (strcmp(key, "X-Socket-ID") == 0) {
                prot->sock_id = atoi(value);
            } else if (strcmp(key, "Last-Modified") == 0) {
                prot->last_modified = (char *) calloc(strlen(value), sizeof(char));
                strcpy(prot->last_modified, value);
            } else if (strcmp(key, "Server") == 0) {
                prot->server = (char *) calloc(strlen(value), sizeof(char));
                strcpy(prot->server, value);
            } else if (strcmp(key, "ETag") == 0) {
                prot->etag = (char *) calloc(strlen(value), sizeof(char));
                strcpy(prot->etag, value);
            } else if (strcmp(key, "Accept-Ranges") == 0) {
                prot->accept_ranges = (char *) calloc(strlen(value), sizeof(char));
                strcpy(prot->accept_ranges, value);
            } else if (strcmp(key, "Connection") == 0) {
                if (strcmp(value, "keep-alive") == 0) {
                    prot->connection = KEEP_ALIVE;
                } else if (strcmp(value, "close") == 0) {
                    prot->connection = CLOSE;
                }
            } else {
                prot->headers = (http_header_t **) realloc(prot->headers, sizeof(http_header_t *) * prot->header_len +
                                                                          sizeof(http_header_t *));
                prot->headers[prot->header_len] = (http_header_t *) calloc(1, sizeof(http_header_t));
                prot->headers[prot->header_len]->key = key;
                prot->headers[prot->header_len]->value = value;
                prot->header_len++;
            }
        }

        line_num++;
        tok = strtok_r(NULL, line_del, &next_line);
    }


    if (body_part != NULL) {
        prot->body = (char *) calloc(strlen(body_part), sizeof(char));
        memcpy(prot->body, body_part, prot->content_length);
    }

    return prot;
}

http_method_t chttp_str_to_method(char *type_str) {
    if (strcmp(type_str, "GET") == 0) return GET;
    if (strcmp(type_str, "HEAD") == 0) return HEAD;
    if (strcmp(type_str, "POST") == 0) return POST;
    if (strcmp(type_str, "PUT") == 0) return PUT;
    if (strcmp(type_str, "DELETE") == 0) return DELETE;
    if (strcmp(type_str, "CONNECT") == 0) return CONNECT;
    if (strcmp(type_str, "OPTIONS") == 0) return OPTIONS;
    if (strcmp(type_str, "TRACE") == 0) return TRACE;
    if (strcmp(type_str, "PATCH") == 0) return PATCH;
}

char *chttp_method_to_str(http_method_t method) {
    if (method == GET) return "GET";
    if (method == HEAD) return "HEAD";
    if (method == POST) return "POST";
    if (method == PUT) return "PUT";
    if (method == DELETE) return "DELETE";
    if (method == CONNECT) return "CONNECT";
    if (method == OPTIONS) return "OPTIONS";
    if (method == TRACE) return "TRACE";
    if (method == PATCH) return "PATCH";
}

char *chttp_to_str(http_prot_t *prot, http_prot_type_t type) {
    unsigned int first_line_length = 0;

    char *result = 0;
    if (type == RESPONSE) {
        char status_code_str[4];
        sprintf(status_code_str, "%d", prot->status);
        // 2 => spaces
        // 2 => \r\n
        first_line_length =
                strlen(prot->http_version) + strlen(prot->status_text) + strlen(status_code_str) + 2 + 2;
        result = (char *) calloc(first_line_length, sizeof(char));
        sprintf(result, "%s %s %s\r\n", prot->http_version, status_code_str, prot->status_text);
    } else if (type == REQUEST) {
        // 2 => spaces
        // 2 => \r\n
        char *method_str = chttp_method_to_str(prot->method);
        first_line_length =
                strlen(method_str) + strlen(prot->url) + strlen(prot->http_version) + 2 + 2;
        result = (char *) calloc(first_line_length, sizeof(char));
        sprintf(result, "%s %s %s\r\n", method_str, prot->url, prot->http_version);
    }

    if (prot->host != NULL) {
        char *key_text = "Host: ";
        char *pair_text = (char *) calloc(strlen(key_text) + strlen(prot->host) + 2, sizeof(char));
        sprintf(pair_text, "%s%s\r\n", key_text, prot->host);
        result = (char *) realloc(result, strlen(result) + strlen(pair_text) + 1);
        strcat(result, pair_text);
    }
    if (prot->user_agent != NULL) {
        char *key_text = "User-Agent: ";
        char *pair_text = (char *) calloc(strlen(key_text) + strlen(prot->user_agent) + 2, sizeof(char));
        sprintf(pair_text, "%s%s\r\n", key_text, prot->user_agent);
        result = (char *) realloc(result, strlen(result) + strlen(pair_text) + 1);
        strcat(result, pair_text);
    }
    if (prot->accept != NULL) {
        char *key_text = "Accept: ";
        char *pair_text = (char *) calloc(strlen(key_text) + strlen(prot->accept) + 2, sizeof(char));
        sprintf(pair_text, "%s%s\r\n", key_text, prot->accept);
        result = (char *) realloc(result, strlen(result) + strlen(pair_text) + 1);
        strcat(result, pair_text);
    }
    if (prot->accept_language != NULL) {
        char *key_text = "Accept-Language: ";
        char *pair_text = (char *) calloc(strlen(key_text) + strlen(prot->accept_language) + 2, sizeof(char));
        sprintf(pair_text, "%s%s\r\n", key_text, prot->accept_language);
        result = (char *) realloc(result, strlen(result) + strlen(pair_text) + 1);
        strcat(result, pair_text);
        free(pair_text);
    }
    if (prot->accept_encoding != NULL) {
        char *key_text = "Accept-Encoding: ";
        char *pair_text = (char *) calloc(strlen(key_text) + strlen(prot->accept_encoding) + 2, sizeof(char));
        sprintf(pair_text, "%s%s\r\n", key_text, prot->accept_encoding);
        result = (char *) realloc(result, strlen(result) + strlen(pair_text) + 1);
        strcat(result, pair_text);
        free(pair_text);
    }
    if (prot->date != NULL) {
        char *key_text = "Date: ";
        char *pair_text = (char *) calloc(strlen(key_text) + strlen(prot->date) + 2, sizeof(char));
        sprintf(pair_text, "%s%s\r\n", key_text, prot->date);
        result = (char *) realloc(result, strlen(result) + strlen(pair_text) + 1);
        strcat(result, pair_text);
        free(pair_text);
    }
    if (prot->content_type != NULL) {
        char *key_text = "Content-Type: ";
        char *pair_text = (char *) calloc(strlen(key_text) + strlen(prot->content_type) + 2, sizeof(char));
        sprintf(pair_text, "%s%s\r\n", key_text, prot->content_type);
        result = (char *) realloc(result, strlen(result) + strlen(pair_text) + 1);
        strcat(result, pair_text);
        free(pair_text);
    }
    if (prot->last_modified != NULL) {
        char *key_text = "Last-Modified: ";
        char *pair_text = (char *) calloc(strlen(key_text) + strlen(prot->last_modified) + 2, sizeof(char));
        sprintf(pair_text, "%s%s\r\n", key_text, prot->last_modified);
        result = (char *) realloc(result, strlen(result) + strlen(pair_text) + 1);
        strcat(result, pair_text);
        free(pair_text);
    }
    if (prot->server != NULL) {
        char *key_text = "Server: ";
        char *pair_text = (char *) calloc(strlen(key_text) + strlen(prot->server) + 2, sizeof(char));
        sprintf(pair_text, "%s%s\r\n", key_text, prot->server);
        result = (char *) realloc(result, strlen(result) + strlen(pair_text) + 1);
        strcat(result, pair_text);
        free(pair_text);
    }
    if (prot->etag != NULL) {
        char *key_text = "ETag: ";
        char *pair_text = (char *) calloc(strlen(key_text) + strlen(prot->etag) + 2, sizeof(char));
        sprintf(pair_text, "%s%s\r\n", key_text, prot->etag);
        result = (char *) realloc(result, strlen(result) + strlen(pair_text) + 1);
        strcat(result, pair_text);
        free(pair_text);
    }
    if (prot->accept_ranges != NULL) {
        char *key_text = "Accept-Ranges: ";
        char *pair_text = (char *) calloc(strlen(key_text) + strlen(prot->accept_ranges) + 2, sizeof(char));
        sprintf(pair_text, "%s%s\r\n", key_text, prot->accept_ranges);
        result = (char *) realloc(result, strlen(result) + strlen(pair_text) + 1);
        strcat(result, pair_text);
        free(pair_text);
    }

    // Headers
    {
        if (prot->header_len != 0) {
            for (int i = 0; i < prot->header_len; i++) {
                http_header_t *header = prot->headers[i];
                char *key = header->key;
                char *val = header->value;
                char *pair_text = (char *) calloc(strlen(key) + strlen(val) + 2 + 2, sizeof(char));
                sprintf(pair_text, "%s: %s\r\n", key, val);
                result = (char *) realloc(result, strlen(result) + strlen(pair_text) + 1);
                strcat(result, pair_text);
                free(pair_text);
            }
        }
    }

    {
        char *key_text = "Connection: ";
        char *val = NULL;
        if (prot->connection == KEEP_ALIVE) {
            char *txt = "keep-alive";
            val = (char *) calloc(strlen(txt), sizeof(char));
            strcpy(val, txt);
        } else if (prot->connection == CLOSE) {
            char *txt = "close";
            val = (char *) calloc(strlen(txt), sizeof(char));
            strcpy(val, txt);
        }
        char *pair_text = (char *) calloc(strlen(key_text) + strlen(val) + 2 + 1, sizeof(char));
        sprintf(pair_text, "%s%s\r\n", key_text, val);
        result = (char *) realloc(result, strlen(result) + strlen(pair_text) + 1);
        strcat(result, pair_text);
        free(pair_text);
    }
    {
        char *key_text = "Content-Length: ";
        char num[16];
        sprintf(num, "%ld", strlen(prot->body));
        char *pair_text = (char *) calloc(strlen(key_text) + strlen(num) + 2 + 1, sizeof(char));
        sprintf(pair_text, "%s%s\r\n", key_text, num);
        result = (char *) realloc(result, strlen(result) + strlen(pair_text) + 1);
        strcat(result, pair_text);
        free(pair_text);
    }
    {
        char *key_text = "X-Socket-ID: ";
        char num[16];
        sprintf(num, "%d", prot->sock_id);
        char *pair_text = (char *) calloc(strlen(key_text) + strlen(num) + 2 + 1, sizeof(char));
        sprintf(pair_text, "%s%s\r\n", key_text, num);
        result = (char *) realloc(result, strlen(result) + strlen(pair_text) + 1);
        strcat(result, pair_text);
        free(pair_text);
    }

    result = (char *) realloc(result, strlen(result) + 2 + strlen(prot->body) + 1);
    strcat(result, "\r\n");
    strcat(result, prot->body);

    return result;
}

void chttp_free(http_prot_t *prot) {
    for (int i = 0; i < prot->header_len; i++) {
        free(prot->headers[i]);
    }
    free(prot);
}

bool chttp_is_http(char *message) {
    // Check if the message starts with "HTTP/1."
    if (strncmp(message, "HTTP/1.", 7) == 0) {
        return true;
    }

    // Check if the message starts with "GET ", "POST ", "PUT ", "DELETE ", "HEAD ", or "OPTIONS ".
    if (strncmp(message, "GET ", 4) == 0 ||
        strncmp(message, "POST ", 5) == 0 ||
        strncmp(message, "PUT ", 4) == 0 ||
        strncmp(message, "DELETE ", 7) == 0 ||
        strncmp(message, "HEAD ", 5) == 0 ||
        strncmp(message, "OPTIONS ", 8) == 0) {
        return true;
    }

    // Otherwise, the message is not an HTTP message.
    return false;
}

char* chttp_status_to_msg(int status_code) {
    switch (status_code) {
        case 100:
            return "Continue";
        case 101:
            return "Switching Protocols";
        case 102:
            return "Processing";
        case 200:
            return "OK";
        case 201:
            return "Created";
        case 202:
            return "Accepted";
        case 203:
            return "Non-Authoritative Information";
        case 204:
            return "No Content";
        case 205:
            return "Reset Content";
        case 206:
            return "Partial Content";
        case 207:
            return "Multi-Status";
        case 208:
            return "Already Reported";
        case 226:
            return "IM Used";
        case 300:
            return "Multiple Choices";
        case 301:
            return "Moved Permanently";
        case 302:
            return "Found";
        case 303:
            return "See Other";
        case 304:
            return "Not Modified";
        case 305:
            return "Use Proxy";
        case 307:
            return "Temporary Redirect";
        case 308:
            return "Permanent Redirect";
        case 400:
            return "Bad Request";
        case 401:
            return "Unauthorized";
        case 402:
            return "Payment Required";
        case 403:
            return "Forbidden";
        case 404:
            return "Not Found";
        case 405:
            return "Method Not Allowed";
        case 406:
            return "Not Acceptable";
        case 407:
            return "Proxy Authentication Required";
        case 408:
            return "Request Timeout";
        case 409:
            return "Conflict";
        case 410:
            return "Gone";
        case 411:
            return "Length Required";
        case 412:
            return "Precondition Failed";
        case 413:
            return "Payload Too Large";
        case 414:
            return "URI Too Long";
        case 415:
            return "Unsupported Media Type";
        case 416:
            return "Range Not Satisfiable";
        case 417:
            return "Expectation Failed";
        case 418:
            return "I'm a teapot";
        case 421:
            return "Misdirected Request";
        case 422:
            return "Unprocessable Entity";
        case 423:
            return "Locked";
        case 424:
            return "Failed Dependency";
        case 426:
            return "Upgrade Required";
        case 428:
            return "Precondition Required";
        case 429:
            return "Too Many Requests";
        case 431:
            return "Request Header Fields Too Large";
        case 451:
            return "Unavailable For Legal Reasons";
        case 500:
            return "Internal Server Error";
        case 501:
            return "Not Implemented";
        case 502:
            return "Bad Gateway";
        case 503:
            return "Service Unavailable";
        case 504:
            return "Gateway Timeout";
        case 505:
            return "HTTP Version Not Supported";
        case 506:
            return "Variant Also Negotiates";
        case 507:
            return "Insufficient Storage";
        case 508:
            return "Loop Detected";
        case 510:
            return "Not Extended";
        case 511:
            return "Network Authentication Required";
        default:
            return "Unknown Status";
    }
}