#ifndef SERVER_CREDIS_H
#define SERVER_CREDIS_H

#include <hiredis/hiredis.h>
#include <event2/event.h>
#include <hiredis/adapters/libevent.h>
#include <hiredis/async.h>
#include <stdbool.h>
#include <signal.h>
#include <stdlib.h>

typedef void (credis_callback_func)(char*,char*);

redisContext* credis_connect(const char* host, int port);
redisAsyncContext* credis_connect_async(const char* host, int port);
void credis_set(redisContext* context, char* key, char* value);
void credis_setex(redisContext* context, char* key, char* value, int expire_seconds);
char* credis_get(redisContext* context, char* key);
void credis_del(redisContext* context, char* key);
void credis_persist(redisContext* context, char* key);
bool credis_exists(redisContext* context, char* key);
void credis_flush(redisContext* context);
void credis_expire(redisContext* context, char* key, int seconds);
void credis_subscribe(redisAsyncContext* context, char* channel, credis_callback_func* callback);
void credis_publish(redisContext* context, char* channel, char* data);

#endif
