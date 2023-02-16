#include "chttpmsg.h"

char* chttpmsg_response(char* body, unsigned int status, char* content_type, int sockfd, char* servername){
    http_prot_t* res = (http_prot_t*) calloc(1, sizeof(http_prot_t));
    res->sock_id = sockfd;
    res->status = status;
    res->status_text = chttp_status_to_msg(status);
    res->http_version = "HTTP/1.1";
    res->server = servername;
    res->content_type = content_type;
    res->body = body;
    res->content_length = strlen(body);
    char* res_str = chttp_to_str(res, RESPONSE);
    chttp_free(res);

    return res_str;
}