#include <stdio.h>
#include <bits/stdc++.h>
#include <unistd.h>
#include <hiredis/hiredis.h>

using namespace std;
long long const MAX_WORKERS = 100;
long long const MAX_THREADS = 15;
long long CURRENT_THREAD = 0;

redisContext* local = redisConnect("127.0.0.1", 6379);
redisContext* workersContext[MAX_WORKERS][MAX_THREADS];
long long workersCount = 1;
long long maxThreads = 1;
char* ip[MAX_WORKERS] = { "192.168.1.64", "192.168.1.89" };
long long workersNodeStart[MAX_WORKERS] = { 0, 2500000 };
long long workersNodeEnd[MAX_WORKERS] = { 2500000, 9999999 };
long long localWorkerStartNode, localWorkerEndNode;
long long localWorkerId = 0;
long long redisGetCount = 0, redisSetCount = 0, redisCommandCount = 0;
long long debugLevel = 100;
double redisSetCmdRunningTime, redisGetCmdRunningTime, redisStringToDoubleConvertTime;

void debugRedisReply(redisReply* reply, char* command = "") {
    if (debugLevel >= 10) printf("debugRedisReply->redisReply's CMD: '%s'; REPLY: %p %ld;", command, reply, reply->elements);
}

void printRedisReply(redisReply* reply, char* startStr = "") {
    printf("%s================================================\n", startStr);
    printf("%sTYPE: %d\n", startStr, reply->type);
    printf("%sINTEGER: %lld\n", startStr, reply->integer);
    printf("%sSTRING: %s\n", startStr, reply->str);
    printf("%sELEMENTS: %ld\n", startStr, reply->elements);
    for (long long i = 0; i < reply->elements; i++) {
        printRedisReply(reply->element[i], "____ ");
    }
    printf("%s================================================\n", startStr);
}

bool isEqual(double a, double b, double acceptError = 0.00001) {
    return abs(a - b) < acceptError;
}

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
        strcat(res, valName);
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
        strcat(res, valName);
        free(valName);
    }
    ++redisCommandCount;
    return res;
}

void delAllNodesAtRound(long long roundId, long long contextId = localWorkerId, long long currThread = 0) {
    redisContext* context = workersContext[contextId][currThread];
    // char* command = new char[100];
    char* command = (char*)malloc(100 * sizeof(char));
    sprintf(command, "KEYS *_%lld", roundId);
    redisReply* reply = (redisReply*)redisCommand(context, command); debugRedisReply(reply, command);
    if (debugLevel >= 30) printRedisReply(reply);
    free(command);
    // command = new char[5 + 28 * reply->elements];
    command = (char*)malloc(5 + 28 * reply->elements * sizeof(char));
    strcpy(command, "DEL ");
    for (long long i = 0; i < reply->elements; i++) {
        strcat(command, reply->element[i]->str);
        strcat(command, " ");
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
        strcat(res, setPath);
        free(setPath);
    }
    return res;
}

double* executeGetValsCommand(char* command, long long contextId = localWorkerId, long long currThread = 0) {
    redisContext* context = workersContext[contextId][currThread];
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

void executeSetValsCommand(char* command, long long contextId = localWorkerId, long long currThread = 0) {
    redisContext* context = workersContext[contextId][currThread];
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
    char* command = setValsCommand(nodesId, values, nodesCount, roundId);
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
    for (long long i = 0; i < workersCount; i++) {
        long long nodesInWorkersCount = 0;
        for (long long j = 0; j < nodesCount; j++) if (nodesId[j] >= workersNodeStart[i] && nodesId[j] < workersNodeEnd[i]) ++nodesInWorkersCount;
        long long nodesInWorkers[nodesInWorkersCount], nextPos = 0, currPos = 0;
        for (long long j = 0; j < nodesCount; j++) {
            if (nodesId[j] >= workersNodeStart[i] && nodesId[j] < workersNodeEnd[i]) {
                nodesInWorkers[nextPos] = nodesId[j];
                ++nextPos;
            }
        }
        if (debugLevel >= 20) {
            printf("Worker %s is contains %lld / %lld nodes\n", ip[i], nodesInWorkersCount, nodesCount);
        }
        char* command = getValsCommand(nodesInWorkers, nodesInWorkersCount, roundId);
        long long waitedCount = 0;
        while (1) {
            double* res = executeGetValsCommand(command, i);
            bool isDone = true;
            currPos = 0;
            for (long long j = 0; j < nodesCount; j++) {
                if (nodesId[j] >= workersNodeStart[i] && nodesId[j] < workersNodeEnd[i]) {
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

void getRunningEnv() {
    long long startNode, endNode;
    // freopen(".env", "r", stdin);
    // cin >> workersCount;
    maxThreads = 1;
    for (long long i = 0; i < workersCount; i++) {
        for (long long j = 0; j < maxThreads; j++) {
            workersContext[i][j] = redisConnect(ip[i], 6379);
            printf("CONTEXT: %p; i,j: %lld %lld\n", workersContext[i][j], i, j);
            redisReply* reply = (redisReply*)redisCommand(workersContext[i][j], "PING");
            printRedisReply(reply);
            freeReplyObject(reply);
        }
    }
    localWorkerStartNode = workersNodeStart[localWorkerId];
    localWorkerEndNode = workersNodeEnd[localWorkerId];
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
