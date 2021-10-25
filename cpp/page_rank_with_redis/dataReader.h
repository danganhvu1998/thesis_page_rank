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
        if(i%100000==0){
            printf("Build command %d / %lld\n", i, nodesCount);
        }
    }
    printf("Send command\n");
    (redisReply*)redisCommand(local, command);
    printf("Command has been executed");
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
        printf("Start get data from %s\n", currWorker.ip);
        redisReply* reply = (redisReply*)redisCommand(currWorker.redis, "GET result_of_rould_%lld", roundId);
        printf("Done get data from %s. Start load the data to ram\n", currWorker.ip);
        valueStr = strtok(reply->str, ";");
        while( valueStr!=NULL ){
            nodeCachedValue[ currNode ] = atof(valueStr);
            if(currNode%100000==0){
                printf("Loading data from %lld to %lld, finished to %d", currWorker.startNode, currWorker.endNode, currNode);
            }
            ++currNode;
            valueStr = strtok(NULL, ";");
        }
        freeReplyObject(reply);
    }
}