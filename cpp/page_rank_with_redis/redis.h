#include <stdio.h>
#include <bits/stdc++.h>
#include <unistd.h>
#include <hiredis/hiredis.h>
#include "utils.h"

using namespace std;

char* getValName(long long nodeId, long long roundId) {
    // char* res = new char[255];
    char* res = (char*)malloc(255 * sizeof(char));
    sprintf(res, "node_%lld_%lld ", nodeId, roundId); // Never hurt to add an space at the end
    if (debugLevel >= 30) printf("getValName->[nodeId, roundId, res]: %lld, %lld, '%s'\n", nodeId, roundId, res);
    return res;
}

char* getSetPath(long long nodeId, double value, long long roundId) {
    // char* res = new char[255];
    char* res = (char*)malloc(255 * sizeof(char));
    char* valName = getValName(nodeId, roundId);
    sprintf(res, "%s%lf ", valName, value); // Never hurt to add an space at the end
    if (debugLevel >= 30) printf("getSetPath->[nodeId, value, roundId, res]: %lld, %lf, %lld, '%s'\n", nodeId, value, roundId, res);
    free(valName);
    return res;
}

char* getValsCommand(long long* nodesId, long long nodesCount, long long roundId) {
    // TODO: Add maximum nodesCount
    // char* res = new char[28 * nodesCount]; // 28 stand for length of node_%s_%lf
    char* res = (char*)malloc(28 * nodesCount * sizeof(char));
    strcpy(res, "MGET ");
    for (long long i = 0; i < nodesCount; i++) {
        char* valName = getValName(nodesId[i], roundId);
        strCat(res, valName);
        free(valName);
    }
    return res;
}

char* delValsCommand(long long* nodesId, long long nodesCount, long long roundId) {
    // TODO: Add maximum nodesCount
    // char* res = new char[28 * nodesCount]; // 28 stand for length of node_%s_%lf
    char* res = (char*)malloc(28 * nodesCount * sizeof(char));
    strcpy(res, "DEL ");
    for (long long i = 0; i < nodesCount; i++) {
        char* valName = getValName(nodesId[i], roundId);
        strCat(res, valName);
        free(valName);
    }
    ++redisCommandCount;
    return res;
}

void delAllNodesAtRound(long long roundId, long long contextId = localWorkerId, long long currThread = 0) {
    redisContext* context = workersList[contextId].redis;
    // char* command = new char[100];
    char* command = (char*)malloc(100 * sizeof(char));
    sprintf(command, "KEYS *_%lld", roundId);
    redisReply* reply = (redisReply*)redisCommand(context, command); debugRedisReply(reply, command);
    if (debugLevel >= 30) printRedisReply(reply);
    free(command);
    // command = new char[5 + 28 * reply->elements];
    command = (char*)malloc(5 + 28 * reply->elements * sizeof(char));
    // TODO: use something faster than strcpy
    strcpy(command, "DEL ");
    for (long long i = 0; i < reply->elements; i++) {
        strCat(command, reply->element[i]->str);
        strCat(command, " ");
    }
    if (debugLevel >= 20) printf("delAllNodesAtRound->command: %s\n", command);
    freeReplyObject(reply);
    reply = (redisReply*)redisCommand(context, command); debugRedisReply(reply, command);
    freeReplyObject(reply);
    redisCommandCount += 2;
}

char* setValsCommand(long long* nodesId, double* values, long long nodesCount, long long roundId) {
    // TODO: Add maximum nodesCount
    // char* res = new char[28 * nodesCount];
    char* res = (char*)malloc(28 * nodesCount * sizeof(char));
    strcpy(res, "MSET ");
    for (long long i = 0; i < nodesCount; i++) {
        char* setPath = getSetPath(nodesId[i], values[i], roundId);
        strCat(res, setPath);
        free(setPath);
    }
    return res;
}

double* executeGetValsCommand(char* command, long long contextId = localWorkerId, long long currThread = 0) {
    redisContext* context = workersList[contextId].redis;
    if (debugLevel >= 20) {
        printf("executeGetValsCommand->command: %s\n", command);
    }
    auto t_start = std::chrono::high_resolution_clock::now();
    redisReply* reply = (redisReply*)redisCommand(context, command); debugRedisReply(reply, command);
    auto t_end = std::chrono::high_resolution_clock::now();
    redisGetCmdRunningTime += std::chrono::duration<double, std::milli>(t_end - t_start).count();
    // double* res = new double[reply->elements];
    double* res = (double*)malloc(reply->elements * sizeof(double));
    if (debugLevel >= 15) {
        printRedisReply(reply);
    }
    t_start = std::chrono::high_resolution_clock::now();
    for (long long i = 0; i < reply->elements; i++) {
        if (debugLevel >= 30) {
            printf("____ executeGetValsCommand->[i, strResult, strResultLen]: %lld %s\n", i, reply->element[i]->str);
        }
        if (reply->element[i]->str) {
            res[i] = atof(reply->element[i]->str);
        }
        else {
            res[i] = -1;
        }
    }
    t_end = std::chrono::high_resolution_clock::now();
    redisStringToDoubleConvertTime += std::chrono::duration<double, std::milli>(t_end - t_start).count();
    if (debugLevel >= 20) {
        printf("____ _____ executeGetValsCommand result: ");
        for (long long i = 0; i < reply->elements; i++) {
            printf("%lf ", res[i]);
        }
        printf("\n");
    }
    freeReplyObject(reply);
    ++redisCommandCount;
    ++redisGetCount;
    return res;
}

void executeSetValsCommand(char* command, redisContext* context = local, long long currThread = 0) {
    if (debugLevel >= 10) {
        printf("executeSetValsCommand->command: %s\n", command);
    }
    auto t_start = std::chrono::high_resolution_clock::now();
    redisReply* reply = (redisReply*)redisCommand(context, command); debugRedisReply(reply, command);
    auto t_end = std::chrono::high_resolution_clock::now();
    redisSetCmdRunningTime += std::chrono::duration<double, std::milli>(t_end - t_start).count();
    freeReplyObject(reply);
    ++redisCommandCount;
    ++redisSetCount;
    return;
}

void setNodesValRedis(long long* nodesId, double* values, long long nodesCount, long long roundId) {
    // TODO URGENT: Maximum set 10^5 nodes val at a time
    const int bulkSize = 10000;
    for (int i = bulkSize; i < nodesCount; i += bulkSize) {
        char* command = setValsCommand(&nodesId[i - bulkSize], &values[i - bulkSize], bulkSize, roundId);
        executeSetValsCommand(command);
        free(command);
        cout << "DONE " << i << " / " << nodesCount << "\n";
    }
    char* command = setValsCommand(
        &nodesId[nodesCount - nodesCount % bulkSize],
        &values[nodesCount - nodesCount % bulkSize],
        nodesCount % bulkSize,
        roundId
    );
    executeSetValsCommand(command);
    free(command);
}

void setNodeVal(long long nodeId, double value, long long roundId) {
    long long nodesId[] = { nodeId };
    double values[] = { value };
    setNodesValRedis(nodesId, values, 1, roundId);
}

double* getNodesValRedis(long long* nodesId, long long nodesCount, long long roundId) {
    // double* result = new double[nodesCount];
    double* result = (double*)malloc(nodesCount * sizeof(double));
    // TODO: Getting values from worker can work in parallel
    // TODO: Each round, start-end node for each worker might be different. Use data from last round to check where to ask for values.
    for (long long i = 0; i < workersCount; i++) {
        long long nodesInWorkersCount = 0;
        for (long long j = 0; j < nodesCount; j++) if (nodesId[j] >= workersList[i].startNode && nodesId[j] < workersList[i].endNode) ++nodesInWorkersCount;
        long long nodesInWorkers[nodesInWorkersCount], nextPos = 0, currPos = 0;
        for (long long j = 0; j < nodesCount; j++) {
            if (nodesId[j] >= workersList[i].startNode && nodesId[j] < workersList[i].endNode) {
                nodesInWorkers[nextPos] = nodesId[j];
                ++nextPos;
            }
        }
        if (debugLevel >= 20) {
            printWorker(workersList[i]);
            printf("Worker %s is contains %lld / %lld nodes\n", ip[i], nodesInWorkersCount, nodesCount);
        }
        char* command = getValsCommand(nodesInWorkers, nodesInWorkersCount, roundId);
        long long waitedCount = 0;
        while (1) {
            double* res = executeGetValsCommand(command, i);
            bool isDone = true;
            currPos = 0;
            for (long long j = 0; j < nodesCount; j++) {
                if (nodesId[j] >= workersList[i].startNode && nodesId[j] < workersList[i].endNode) {
                    if (res[currPos] < 0) isDone = false;
                    result[j] = res[currPos];
                    ++currPos;
                }
            }
            free(res);
            if (isDone) break;
            else {
                ++waitedCount;
                if (debugLevel >= 1) {
                    printf("getNodesValRedis: Worker %s is not yet finished round %lld. Waited %lld sec\n", ip[i], roundId, waitedCount / 2);
                }
                usleep(500000);
            }
        }
        free(command);
    }
    return result;
}

void distributeTask() {
    redisReply* reply = (redisReply*)redisCommand(mainWorkerRedis, "GET WORKERS_IP_ADDRESSES");
    workersCount = split(reply->str, ",", ip);
    cout << "\nworkersCount: " << workersCount << endl;
    for0(i, workersCount) cout << "ip: " << ip[i] << endl;
    freeReplyObject(reply);
    int startNode = 0, endNode = 0;
    double totalGetTimeByGetNode = 0;
    if(currentRoundId <=1){
        // First round, every node is equal
        for (int i = 0; i < workersCount; i++) {
            startNode = endNode;
            endNode = startNode + N / workersCount + 10;
            int currWorkerId = getWorkerById(i);
            (redisReply*)redisCommand(
                workersList[currWorkerId].redis, "MSET ROUND_%lld_START_NODE %lld ROUND_%lld_END_NODE %lld",
                currentRoundId, startNode,
                currentRoundId, endNode
            );
            printf("\nIP %d: %s: %d %d", i, workersList[currWorkerId].ip, startNode, endNode);
        }
    } else {
        // Check if all workers have finished last round, or this worker is new
        for (int i = 0; i < workersCount; i++) {
            int workerCurrRound = -1;
            int currWorkerId = getWorkerById(i);
            reply = (redisReply*)redisCommand(workersList[currWorkerId].redis, "MGET %s_AT_ROUND %s_CALCULATE_TIME_LAST_ROUND %s_GET_DATA_TIME_LAST_ROUND",
                workersList[currWorkerId].ip, workersList[currWorkerId].ip, workersList[currWorkerId].ip
            );
            printRedisReply(reply, "Check running time");
            if(reply->element[0]->str ) workerCurrRound = atoi(reply->element[0]->str);
            printf("\nip: %s, workerCurrRound: %lld - %s, currentRoundId: %lld\n", workersList[currWorkerId].ip, workerCurrRound, reply->element[0]->str, currentRoundId);
            if(workerCurrRound == 0 || workerCurrRound == currentRoundId){
                workersList[currWorkerId].lastRoundCalTime = atof(reply->element[1]->str);
                workersList[currWorkerId].lastRoundGetDataTime = atof(reply->element[2]->str);
                totalGetTimeByGetNode += workersList[currWorkerId].lastRoundGetDataTime / (workersList[currWorkerId].endNode - workersList[currWorkerId].startNode);
                printWorker(workersList[currWorkerId]);
                freeReplyObject(reply);
            } else {
                i--; // Check again
                usleep(500000);
            }
        }
        // Re-distribute task
        for (int i = 0; i < workersCount; i++) {
            startNode = endNode;
            int currWorkerId = getWorkerById(i);
            double currWorkerGetNodeTimeRate = workersList[currWorkerId].lastRoundGetDataTime / (workersList[currWorkerId].endNode - workersList[currWorkerId].startNode);
            endNode = min((long long)(startNode + N * currWorkerGetNodeTimeRate / totalGetTimeByGetNode+10), workersList[currWorkerId].loadEndNode);
            totalGetTimeByGetNode -= currWorkerGetNodeTimeRate;
            (redisReply*)redisCommand(
                workersList[currWorkerId].redis, "MSET ROUND_%lld_START_NODE %lld ROUND_%lld_END_NODE %lld",
                currentRoundId, startNode,
                currentRoundId, endNode
            );
            printf("\nIP %d: %s: %d %d", i, workersList[currWorkerId].ip, startNode, endNode);
        }
    }
    (redisReply*)redisCommand(mainWorkerRedis, "SET DISTRIBUTED_TASK_FOR_ROUND_%lld %lld", currentRoundId, 1);
}

void getTask() {
    // Announce BE about last result if have.
    if (roundCalTime > 0 and roundGetNodeTime > 0) {
        char* command = (char*)malloc(500);
        sprintf(command, "MSET %s_AT_ROUND %lld %s_CALCULATE_TIME_LAST_ROUND %lf %s_GET_DATA_TIME_LAST_ROUND %lf",
            LOCAL_IP_ADDRESS, currentRoundId,
            LOCAL_IP_ADDRESS, roundCalTime,
            LOCAL_IP_ADDRESS, roundGetNodeTime
        );
        printf("COMMAND %s", command);
        redisCommand(mainWorkerRedis, command);
        free(command);
    }
    printf("\n***** DONE SEND LAST ROUND DATA *****\n");
    if (!strcmp(LOCAL_IP_ADDRESS, MAIN_WORKER_IP_ADDRESS)) {
        distributeTask();
    }
    printf("\n***** DONE DISTRIBUTE ROUND TASK *****\n");
    while (1) {
        redisReply* reply = (redisReply*)redisCommand(mainWorkerRedis, "GET DISTRIBUTED_TASK_FOR_ROUND_%lld", currentRoundId);
        if (reply->str == NULL) {
            printf("Task for round %lld has not yet get distributed\n", currentRoundId);
            usleep(500000);
            continue;
        }
        reply = (redisReply*)redisCommand(mainWorkerRedis, "GET WORKERS_IP_ADDRESSES");
        workersCount = split(reply->str, ",", ip);
        cout << "workersCount: " << workersCount << endl;
        for0(i, workersCount) cout << "ip: " << ip[i] << endl;
        freeReplyObject(reply);
        for (int i = 0; i < workersCount; i++) {
            int currWorkerId = getWorkerById(i);
            reply = (redisReply*)redisCommand(
                workersList[currWorkerId].redis, "MGET ROUND_%lld_START_NODE  ROUND_%lld_END_NODE ",
                currentRoundId, currentRoundId
            );
            if (reply->element[0]->str == NULL || reply->element[1]->str == NULL) {
                --i;
                printf("Not yet able to get task for round %lld of worker %s\n", currentRoundId, workersList[currWorkerId].ip);
                usleep(500000);
            }
            else {
                workersList[currWorkerId].startNode = max(0, atoi(reply->element[0]->str));
                workersList[currWorkerId].endNode = atoi(reply->element[1]->str);
                if(workersList[currWorkerId].endNode >= N) workersList[currWorkerId].endNode = N-1;
                if(currentRoundId == 0){
                    II loadData = getLoadRange(workersList[currWorkerId].startNode, workersList[currWorkerId].endNode, 0, N-1);
                    workersList[currWorkerId].loadStartNode = loadData.first;
                    workersList[currWorkerId].loadEndNode = loadData.second;
                }
                printWorker(workersList[currWorkerId]);
                if (!strcmp(LOCAL_IP_ADDRESS, workersList[currWorkerId].ip)) {
                    localWorkerStartNode = workersList[currWorkerId].startNode;
                    localWorkerEndNode = workersList[currWorkerId].endNode;
                    localWorkerId = i;
                    cout << "LOCAL WORKER:";
                    printWorker(workersList[i]);
                }
            }
        }
        break;
    }
    printf("\n***** DONE GET ROUND TASK *****\n");
}

void getRunningEnv() {
    long long startNode, endNode;
    char* command = (char*)malloc(500);
    sprintf(command, "APPEND WORKERS_IP_ADDRESSES %s,", LOCAL_IP_ADDRESS);
    redisCommand(mainWorkerRedis, command);
    sprintf(
        command, "MSET %s_CALCULATE_TIME_LAST_ROUND 0 %s_GET_DATA_TIME_LAST_ROUND 0 %s_RAM_SIZE %lf %s_CLOCK_RATE %lf %s_CORES_COUNT %d %s_IP_ADDRESS %s",
        LOCAL_IP_ADDRESS,
        LOCAL_IP_ADDRESS,
        LOCAL_IP_ADDRESS, RAM_SIZE_GB,
        LOCAL_IP_ADDRESS, CLOCK_RATE_GHZ,
        LOCAL_IP_ADDRESS, PHYSICAL_CORES_COUNT,
        LOCAL_IP_ADDRESS, LOCAL_IP_ADDRESS
    );
    (redisReply*)redisCommand(mainWorkerRedis, command);
    free(command);
    // if (!strcmp(LOCAL_IP_ADDRESS, MAIN_WORKER_IP_ADDRESS)) {
    //     distributeTask();
    // }
}

bool __testRedis() {
    getRunningEnv();
    bool testResult = true;
    long long nodesCount = 7;
    long long roundId = 9;
    long long nodesId[] = { 1, 4, 6, 7 ,8, 20, 15 };
    double values[] = { 1.13412341432, 4.1132413241234, 6.412341321324, 7.541323234 ,8.713241234567, 20.7456, 15.456098765437 };
    setNodesValRedis(nodesId, values, nodesCount, roundId);
    double* getVals = getNodesValRedis(nodesId, nodesCount, roundId);
    for (long long i = 0; i < nodesCount; i++) {
        if (!isEqual(values[i], getVals[i])) testResult = false;
    }
    if (debugLevel >= 20) {
        for (long long i = 0; i < nodesCount; i++) {
            printf("Set value: %lf; Get value: %lf; Is correct: %d\n", values[i], getVals[i], isEqual(values[i], getVals[i]));
        }
        printf("\n");
    }
    delAllNodesAtRound(9);
    getVals = getNodesValRedis(nodesId, nodesCount, roundId);
    for (long long i = 0; i < nodesCount; i++) {
        if (!isEqual(-1, getVals[i])) testResult = false;
    }
    if (debugLevel >= 20) {
        for (long long i = 0; i < nodesCount; i++) {
            printf("Set value: null(deleted); Get value: %lf; Is correct: %d\n", getVals[i], isEqual(-1, getVals[i]));
        }
        printf("\n");
    }
    printf("__testRedis: Test result: %s\n", testResult ? "OK" : "FAIL");
    return testResult;
}
