#include <bits/stdc++.h>
#include <hiredis/hiredis.h>
#include <omp.h>
#include "redis.h"

void setNodesValToAllRedis(long long* nodesId, double* values, long long nodesCount, long long roundId) {
    char* command = (char*)malloc(20 * nodesCount * sizeof(char));
    char* tempVal = (char*)malloc(20);
    sprintf(command, "SET result_of_rould_%lld ", roundId, LOCAL_IP_ADDRESS);
    for(int i=0; i<nodesCount; i++){
        sprintf(tempVal, "%lf;", values[i]);
        strCat(command, tempVal);
    }
    (redisReply*)redisCommand(local, command);
    free(command);
    free(tempVal);
}

void getAllNodesValue(long long roundId) {
    // TODO: omp
    for (int i = 0; i < workersCount; i++) {
        int currWorkerId = getWorkerById(i);
        worker currWorker = workersList[currWorkerId];
        int currNode = currWorker.startNode;
        char* valueStr;
        redisReply* reply = (redisReply*)redisCommand(currWorker.redis, "GET result_of_rould_%lld", roundId);
        valueStr = strtok(reply->str, ";");
        while( valueStr!=NULL ){
            nodeCachedValue[ currNode ] = atof(valueStr);
            ++currNode;
            valueStr = strtok(NULL, ";");
        }
        freeReplyObject(reply);
    }
}