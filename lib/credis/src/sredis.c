#include "credis.h"

redisContext* credis_connect(const char* host, int port){
    redisContext* context = redisConnect(host, port);
    if(context == NULL || context->err){
        if (context) {
            fprintf(stderr, "Error: %s\n", context->errstr);
            exit(EXIT_FAILURE);
        } else {
            fprintf(stderr, "Can't allocate redis context\n");
            exit(EXIT_FAILURE);
        }
    }
//    fprintf(stdout, "Sockethub is connected to redis server\n");
    return context;
}

redisAsyncContext* credis_connect_async(const char* host, int port){
    redisAsyncContext* context = redisAsyncConnect(host, port);
    if(context == NULL || context->err){
        if (context) {
            fprintf(stderr, "Error: %s\n", context->errstr);
            exit(EXIT_FAILURE);
        } else {
            fprintf(stderr, "Can't allocate redis context\n");
            exit(EXIT_FAILURE);
        }
    }
//    fprintf(stdout, "Sockethub is connected (async) to redis server\n");
    return context;
}

void credis_set(redisContext* context, char* key, char* value){
    redisReply* reply = redisCommand(context, "SET %s %s", key, value);
    if(reply == NULL){
        if(context->err){
            fprintf(stderr, "Error: %s\n", context->errstr);
        }else {
            fprintf(stderr, "Can't perform SET command\n");
        }
    }
}

void credis_setex(redisContext* context, char* key, char* value, int expire_seconds){
    redisReply* reply = redisCommand(context, "SETEX %s %d %s", key, expire_seconds, value);
    if(reply == NULL){
        if(context->err){
            fprintf(stderr, "Error: %s\n", context->errstr);
        }else {
            fprintf(stderr, "Can't perform SETX command\n");
        }
    }
}

char* credis_get(redisContext* context, char* key){
    if(!credis_exists(context, key)){
        return "(null)";
    }
    redisReply* reply = redisCommand(context, "GET %s", key);
    if(reply == NULL){
        if(context->err){
            fprintf(stderr, "Error: %s\n", context->errstr);
        }else {
            fprintf(stderr, "Can't perform GET command\n");
        }
    }
    return reply->str;
}

void credis_del(redisContext* context, char* key){
    if(!credis_exists(context, key)){
        return;
    }
    redisReply* reply = redisCommand(context, "DEL %s", key);
    if(reply == NULL){
        if(context->err){
            fprintf(stderr, "Error: %s\n", context->errstr);
        }else {
            fprintf(stderr, "Can't perform DEL command\n");
        }
    }
}

bool credis_exists(redisContext* context, char* key){
    redisReply* reply = redisCommand(context, "EXISTS %s", key);
    if(reply == NULL){
        if(context->err){
            fprintf(stderr, "Error: %s\n", context->errstr);
            return false;
        }else {
            fprintf(stderr, "Can't perform EXISTS command\n");
            return false;
        }
    }
    return reply->integer == 1;
}

void credis_flush(redisContext* context){
    redisReply* reply = redisCommand(context, "FLUSHALL");
    if(reply == NULL){
        if(context->err){
            fprintf(stderr, "Error: %s\n", context->errstr);
        }else {
            fprintf(stderr, "Can't perform FLUSHALL command\n");
        }
    }
}

void credis_expire(redisContext* context, char* key, int seconds){
    if(!credis_exists(context, key)){
        return;
    }
    redisReply* reply = redisCommand(context, "EXPIRE %s %d", key, seconds);
    if(reply == NULL){
        if(context->err){
            fprintf(stderr, "Error: %s\n", context->errstr);
        }else {
            fprintf(stderr, "Can't perform EXPIRE command\n");
        }
    }
}

void credis_persist(redisContext* context, char* key){
    if(!credis_exists(context, key)){
        return;
    }
    redisReply* reply = redisCommand(context, "PERSIST %s", key);
    if(reply == NULL){
        if(context->err){
            fprintf(stderr, "Error: %s\n", context->errstr);
        }else {
            fprintf(stderr, "Can't perform PERSIST command\n");
        }
    }
}

void on_message(redisAsyncContext *c, void *async_reply, void *callback){
    redisReply* reply = async_reply;
    if(async_reply == NULL) return;
    if (reply->type == REDIS_REPLY_ARRAY) {
        if(reply->element[2]->type != 1) return;
        credis_callback_func* func = callback;
        func(reply->element[1]->str, reply->element[2]->str);
    }
}

void credis_subscribe(redisAsyncContext* context, char* channel, credis_callback_func* callback){
    signal(SIGPIPE, SIG_IGN);
    struct event_base *base = event_base_new();
    redisLibeventAttach(context, base);
    redisAsyncCommand(context, on_message, callback, "SUBSCRIBE %s", channel);
    event_base_dispatch(base);
}

void credis_publish(redisContext* context, char* channel, char* data){
    redisReply* reply = redisCommand(context, "PUBLISH %s %s", channel, data);
    if(reply == NULL){
        if(context->err){
            fprintf(stderr, "Error: %s\n", context->errstr);
        }else {
            fprintf(stderr, "Can't perform PUBLISH command\n");
        }
    }
}