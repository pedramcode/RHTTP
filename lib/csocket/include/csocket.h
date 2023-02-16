#ifndef SERVER_SOCKET_H
#define SERVER_SOCKET_H

#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <stdbool.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <unistd.h>
#include <unistd.h>
#include "chttp.h"
#include "cnetwork.h"
#include "ctime.h"
#include "chttpmsg.h"
#include "cconfig.h"

int csocket_create();
void csocket_listen(int sockfd, rhttp_config_t* rconfig);

#endif
