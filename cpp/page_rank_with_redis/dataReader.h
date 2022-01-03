#include <bits/stdc++.h>
#include <hiredis/hiredis.h>
#include <omp.h>
#include "redis.h"

void setNodesValToAllRedis(double* values, long long nodesCount, long long roundId) {
    int commandCnt = nodesCount / bulkSide;
    if (nodesCount%bulkSide!=0) {
        commandCnt++;
    }
    printf("VALUES: ");
    for(int i=0; i<10; i++) {
        printf("%lf ", values[i]);
    }
    printf("\n");
# pragma omp parallel for default(shared)
    for(int commandPos = 0; commandPos < commandCnt; commandPos++) {
        int threadId = omp_get_thread_num();
        int startNode = commandPos * bulkSide;
        int endNode = min(1LL * (commandPos + 1) * bulkSide , nodesCount) - 1; 
        int currStrPos = 0;
        char* command = (char*)malloc(10 * bulkSide * sizeof(char));
        char* tempVal = (char*)malloc(20);
        
        printf("Thread %d: Build command %dth from nodes %d to %d\n", threadId, commandPos, startNode, endNode);
        sprintf(command, "SET result_of_round_%lld_%d ", roundId, commandPos);
        currStrPos = strlen(command);
        int srcLen = 0;
        for(int i = startNode; i <= endNode; i++) {
            sprintf(tempVal, "%f", values[i]);
            srcLen = strlen(tempVal);
            for (int j = 0; j < srcLen; j++) {
                command[currStrPos] = tempVal[j];
                ++currStrPos;
            }
            command[currStrPos] = ';';
            ++currStrPos;
        }
        command[currStrPos] = '\0';
        
        (redisReply*)redisCommand(localConnections[threadId], command);
        printf("\n\n\n%s\n\n\n", command);
        printf("Sent command %dth from nodes %d to %d successfully\n", commandPos, startNode, endNode);
        free(command);
        free(tempVal);
    }
}

void getAllNodesValue(long long roundId) {
# pragma omp parallel for default(shared)
    for (int i = 0; i < workersCount; i++) {
        int threadId = omp_get_thread_num();
        int currWorkerId = getWorkerById(i);
        worker currWorker = workersList[currWorkerId];
        printWorker(currWorker);
        int currNode = currWorker.startNode;
        char* valueStr;
        int commandCnt = (currWorker.endNode - currWorker.startNode) / bulkSide;
        if ((currWorker.endNode - currWorker.startNode)%bulkSide!=0) {
            commandCnt++;
        }
        printf("===> currNode: %d; commandCnt: %d\n", currNode, commandCnt);

        for(int j=0; j<commandCnt; j++){
            printf("Thread %d: Start get data pack %dth from %s\n", threadId, j, currWorker.ip);
            auto sendStart = std::chrono::high_resolution_clock::now();
            redisReply* reply = (redisReply*)redisCommand(currWorker.redis, "GET result_of_round_%lld_%d", roundId, j);
            auto sendEnd = std::chrono::high_resolution_clock::now();
            double sendTime = std::chrono::duration<double, std::milli>(sendEnd - sendStart).count();
            if (reply->str == NULL) {
                printf("Cannot yet get data pack %dth of round %lld from %s, Sleep 0.25s\n", j, roundId,currWorker.ip);
                freeReplyObject(reply);
                --j;
                usleep(250000);
                continue;
            } else {
                printf("Done get data pack %dth from %s in %lfms. Start load the data to ram\n", j, currWorker.ip,sendTime);
            }
            // if(currNode >= 60){
            //     printf("\n\n%s\n\n", reply->str);
            // }
            // TODO: Check if data has not been complated
            valueStr = strtok(reply->str, ";");
            while( valueStr!=NULL ){
                nodeCachedValue[ currNode ] = atof(valueStr);
                // if(currNode>=60){
                //     printf("======> %d: %f\n", currNode, nodeCachedValue[ currNode ]);
                // }
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