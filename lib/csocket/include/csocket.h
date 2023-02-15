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

int csocket_create(bool debug);
void csocket_listen(int sockfd, unsigned int port);

#endif
