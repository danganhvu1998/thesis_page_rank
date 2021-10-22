#include <bits/stdc++.h>
#include <hiredis/hiredis.h>
#include <omp.h>
#include "redis.h"

#define II pair<int, int>

std::unordered_map<long long, double> nodesCache;
std::unordered_map<long long, double>::iterator it;

double getValueFromCache(long long nodeId, long long roundId) {
    if (nodeId < maxIndexCachedSize) return nodeCachedValue[nodeId];
    printf("REACHED HERE %lld >= %lld", nodeId, maxIndexCachedSize);
    long long key = roundId * maxNodes + nodeId;
    if (cacheHitCount + cacheMissCount == reportCacheHitAfterTimes) {
        cacheHitByRound[roundId].push_back(cacheHitCount);
        cacheHitCount = 0; cacheMissCount = 0;
    }
    it = nodesCache.find(key);
    if (it != nodesCache.end()) { // FOUND
        ++cacheHitCount;
        return it->second;
    }
    else { // NOT FOUND
        ++cacheMissCount;
        return -1;
    }
}

void clearCache(long long removeCaches = 0) {
    if (removeCaches) {
        it = nodesCache.begin();
        std::advance(it, removeCaches);
        nodesCache.erase(nodesCache.begin(), it);
    }
}

void addCache(long long nodeId, long long roundId, double value) {
    long long key = roundId * maxNodes + nodeId;
    nodesCache[key] = value;
    // TODO: Reduce number of time check this by using random
    long long removeCaches = nodesCache.size() - maxCaches;
    if (removeCaches > 1000000)  clearCache(removeCaches * 3); // To not clean cache very often
}

// TODO: Not only get its value, but its number of children is also important
double* getNodesVal(long long* nodesId, long long nodesCount, long long roundId) {
    // double* res = new double[nodesCount];
    double* res = (double*)malloc(nodesCount * sizeof(double));
    long long notCachedNodesCount = 0;
    // Check Cache
    auto t_start = std::chrono::high_resolution_clock::now();
    for (int i = 0; i < nodesCount; i++) {
        res[i] = getValueFromCache(nodesId[i], roundId);
        if (res[i] < 0) {  // Not found
            ++notCachedNodesCount;
        }
    }
    auto t_end = std::chrono::high_resolution_clock::now();
    cacheTime += std::chrono::duration<double, std::milli>(t_end - t_start).count();
    if (debugLevel >= 20) {
        printf("getNodesVal->[notCachedNodesCount, nodeCached]: %lld %lld\n", notCachedNodesCount, nodesCount - notCachedNodesCount);
    }
    if (notCachedNodesCount == 0) return res;
    // Check Redis
    long long currPos = 0;
    // long long* notCachedNodesId = new long long[notCachedNodesCount];
    long long* notCachedNodesId = (long long*)malloc(notCachedNodesCount * sizeof(long long));
    for (int i = 0; i < nodesCount; i++) {
        if (res[i] < 0) {  // Not found
            notCachedNodesId[currPos] = nodesId[i];
            ++currPos;
        }
    }
    t_start = std::chrono::high_resolution_clock::now();
    double* resultFromRedis = getNodesValRedis(notCachedNodesId, notCachedNodesCount, roundId);
    t_end = std::chrono::high_resolution_clock::now();
    redisReadTime += std::chrono::duration<double, std::milli>(t_end - t_start).count();
    currPos = 0;
    for (int i = 0; i < nodesCount; i++) {
        if (res[i] < 0) {  // Not found
            res[i] = resultFromRedis[currPos];
            addCache(nodesId[i], roundId, res[i]);
            ++currPos;
        }
    }
    free(resultFromRedis);
    return res;
}

double getNodeVal(long long nodeId, long long roundId) {
    long long nodesId[] = { nodeId };
    double* resArray = getNodesVal(nodesId, 1, roundId);
    double res = resArray[0];
    free(resArray);
    return res;
}

void getAllNodesValue(long long roundId) {
    printf("\n=======================================\n\nSTART GET INDEX CACHED VALUE\n");
# pragma omp parallel for default(shared)
    for (int i = 0; i < workersCount; i++) {
        long long nodesId[MAX_SIZE_BULK_GET + 5];
        int threadId = omp_get_thread_num();
        // int threadId = 0;
        printf("Thread %d will handle getting data from worker %s\n", threadId, workersList[i].ip);
        int next = workersList[i].startNode;
        for (int j = workersList[i].startNode; j < workersList[i].endNode; j++) {
            nodesId[j - next] = j;
            if (j - next >= MAX_SIZE_BULK_GET || j == workersList[i].endNode - 1 || j == N) {
                // SEND TO GET THE DATA
                char* command = getValsCommand(nodesId, j - next + 1, roundId);
                long long waitedCount = 0;
                double executedTimeMilisecond, executedTimeMilisecond2;
                while (1) {
                    auto t_start = std::chrono::high_resolution_clock::now();
                    double* res = executeGetValsCommand(command, i);
                    auto t_end = std::chrono::high_resolution_clock::now();
                    executedTimeMilisecond = std::chrono::duration<double, std::milli>(t_end - t_start).count();
                    int minusValueCount = 0;
                    for (long long k = 0; k < j - next + 1; k++) {
                        if (res[k] < 0) {
                            ++minusValueCount;
                        }
                        nodeCachedValue[next + k] = res[k];
                    }
                    free(res);
                    if (!minusValueCount) break;
                    else {
                        ++waitedCount;
                        if (debugLevel >= 1) {
                            printf("getNodesValRedis: Worker %s is not yet finished round %lld. Cannot find weight of %d nodes . Waited %lld sec\n",
                            workersList[i].ip, roundId, minusValueCount, waitedCount);
                        }
                        usleep(1000000);
                    }
                    auto t_end_2 = std::chrono::high_resolution_clock::now();
                    executedTimeMilisecond2 = std::chrono::duration<double, std::milli>(t_end_2 - t_start).count();
                }
                printf("getNodesValRedis: got %lld / %lld nodes from worker %s. Last command executed in %lfms - %lfms by thread %d\n",
                    j - workersList[i].startNode + 1,
                    workersList[i].endNode - workersList[i].startNode,
                    workersList[i].ip,
                    executedTimeMilisecond, executedTimeMilisecond2,
                    threadId
                );
                free(command);
                next = j + 1;
            }
        }
    }
    printf("\nDONE GET INDEX CACHED VALUE\n\n=======================================\n");
}