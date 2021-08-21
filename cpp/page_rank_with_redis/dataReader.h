#include <bits/stdc++.h>
#include <hiredis/hiredis.h>
#include "redis.h"

#define II pair<int, int>

std::unordered_map<long long, double> nodesCache;
std::unordered_map<long long, double>::iterator it;


double getValueFromCache(long long nodeId, long long roundId) {
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

bool __testDataReader() {
    bool testResult = true;
    long long nodesCount = 7;
    long long roundId = 9;
    long long nodesId[] = { 1, 4, 6, 7 ,8, 20, 15 };
    double values[] = { 1.13412341432, 4.1132413241234, 6.412341321324, 7.541323234 ,8.713241234567, 20.7456, 15.456098765437 };
    setNodesValRedis(nodesId, values, nodesCount, roundId);
    double* getVals = getNodesVal(nodesId, nodesCount, roundId);
    for (int i = 0; i < nodesCount; i++) {
        if (!isEqual(values[i], getVals[i])) testResult = false;
    }
    if (debugLevel >= 20) {
        for (int i = 0; i < nodesCount; i++) {
            printf("Set value: %lf; Get value: %lf; Is correct: %d\n", values[i], getVals[i], isEqual(values[i], getVals[i]));
        }
        printf("\n");
    }
    free(getVals);
    getVals = getNodesVal(nodesId, nodesCount, roundId);
    for (int i = 0; i < nodesCount; i++) {
        if (!isEqual(values[i], getVals[i])) testResult = false;
    }
    if (debugLevel >= 20) {
        for (int i = 0; i < nodesCount; i++) {
            printf("Set value: %lf; Get value: %lf; Is correct: %d\n", values[i], getVals[i], isEqual(values[i], getVals[i]));
        }
        printf("\n");
    }
    free(getVals);
    getVals = getNodesVal(nodesId, nodesCount, 11);
    for (int i = 0; i < nodesCount; i++) {
        if (!isEqual(getVals[i], -1)) testResult = false;
    }
    if (debugLevel >= 20) {
        for (int i = 0; i < nodesCount; i++) {
            printf("Set value: null; Get value: %lf; Is correct: %d\n", getVals[i], isEqual(values[i], -1));
        }
        printf("\n");
    }
    printf("__testDataReader: Test result: %s\n", testResult ? "OK" : "FAIL");
    return testResult;
}