#include <bits/stdc++.h>
#include <hiredis/hiredis.h>
#include <omp.h>
#include "redis.h"

void setNodesValToAllRedis(long long* nodesId, double* values, long long nodesCount, long long roundId) {
    char* command = (char*)malloc(10 * bulkSide * sizeof(char));
    char* tempVal = (char*)malloc(20);
    int curr=0, currStrPos = 0;
    for(int i=0; i<nodesCount; i++){
        if(i%bulkSide==0){
            if(i>0) {
                command[currStrPos] = '\0';
                // printf("Send command\n%s\n", command);
                (redisReply*)redisCommand(local, command);
            }
            printf("Build command %dth | %d / %lld\n", curr, i, nodesCount);
            sprintf(command, "SET result_of_round_%lld_%d ", roundId, curr);
            ++curr;
            currStrPos = strlen(command);
        }
        sprintf(tempVal, "%lf;", values[i]);
        int srcLen = strlen(tempVal);
        for (int i = 0; i < srcLen; i++) {
            command[currStrPos] = tempVal[i];
            ++currStrPos;
        }
    }
    command[currStrPos] = '\0';
    // printf("Send command\n%s\n", command);
    (redisReply*)redisCommand(local, command);
    free(command);
    free(tempVal);
}

void getAllNodesValue(long long roundId) {
// # pragma omp parallel for default(shared)
    for (int i = 0; i < workersCount; i++) {
        int currWorkerId = getWorkerById(i);
        worker currWorker = workersList[currWorkerId];
        int currNode = currWorker.startNode;
        char* valueStr;
        int commandCnt = (currWorker.endNode - currWorker.startNode) / bulkSide + 1;

        for(int j=0; j<commandCnt; j++){
            printf("Start get data pack %dth from %s\n", j, currWorker.ip);
            redisReply* reply = (redisReply*)redisCommand(currWorker.redis, "GET result_of_round_%lld_%d", roundId, j);
            if (reply->str == NULL) {
                printf("Cannot yet get data pack %dth from %s, Sleep 0.25s\n", j, currWorker.ip);
                freeReplyObject(reply);
                --j;
                usleep(250000);
                continue;
            } else {
                printf("Done get data pack %dth from %s. Start load the data to ram\n", j, currWorker.ip);
            }
            // TODO: Check if data has not been complated
            valueStr = strtok(reply->str, ";");
            while( valueStr!=NULL ){
                nodeCachedValue[ currNode ] = atof(valueStr);
                if(currNode%100000==0){
                    printf("Loading data from %lld to %lld, finished to %d\n", currWorker.startNode, currWorker.endNode, currNode);
                }
                ++currNode;
                valueStr = strtok(NULL, ";");
            }
            freeReplyObject(reply);
        }
        printf("Successfully loaded data from %lld to %d\n", currWorker.startNode, currNode-1);
    }
}