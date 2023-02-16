#ifndef RHTTP_CHTTPMSG_H
#define RHTTP_CHTTPMSG_H

#include "chttp.h"

char* chttpmsg_response(char* body, unsigned int status, char* content_type, int sockfd, char* servername);

#endif
