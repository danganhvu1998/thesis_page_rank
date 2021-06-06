#include <stdio.h>
#include <hiredis/hiredis.h>

void printRedisReply(redisReply *reply, char* startStr = ""){
    printf("%s================================================\n", startStr);
    printf("%sTYPE: %d\n", startStr, reply->type);
    printf("%sINTEGER: %lld\n", startStr, reply->integer);
    printf("%sSTRING: %s\n", startStr, reply->str);
    printf("%sELEMENTS: %ld\n", startStr, reply->elements);
    for(int i=0; i<reply->elements; i++){
        printRedisReply(reply->element[i], "----");
    }
    printf("%s================================================\n", startStr);
}

int main(void) {
    redisContext *context;
    redisReply *reply;

    context = redisConnect("127.0.0.1", 6379);
    if (!context) {
        fprintf(stderr, "Error:  Can't connect to Redis\n");
        return -1;
    }

    reply = (redisReply *)redisCommand(context, "MSET %s %d %s %d", "foo", 1236323, "bar", 12233);
    if (!reply || context->err) {
        fprintf(stderr, "Error:  Can't send MSET command to Redis\n");
        return -1;
    }
    printf("set rec: %s\n", reply->str);
    printRedisReply(reply);

    freeReplyObject(reply);

    reply = (redisReply *)redisCommand(context, "MGET foo bar haha");
    // if (!reply || context->err || reply->type != REDIS_REPLY_STRING) {
    //     fprintf(stderr, "Error:  Can't send MGET command to Redis\n");
    //     return -1;
    // }

    printf("foo = %s\n", reply->str);
    printRedisReply(reply);
    freeReplyObject(reply);

    return 0;
}