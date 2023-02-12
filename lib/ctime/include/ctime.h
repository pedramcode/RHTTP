#ifndef SERVER_CTIME_H
#define SERVER_CTIME_H

#include <time.h>
#include <stdio.h>
#include <stdlib.h>

// It's UTC timezone by default

char *ctime_get_now_str();

time_t ctime_get_now();

time_t ctime_get_from_str(char *date);

char *ctime_str_from_time(time_t date);

#endif
