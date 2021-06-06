#include <bits/stdc++.h>
#include <hiredis/hiredis.h>
#include "redis.h"

#define II pair<int, int>

long long currentRoundId = 0;
long long const maxNodes = 100000000007; // 1e11
// TODO: Get ram size and use only 20-30% of it
long long const maxCaches = 10000007; // 1e7
std::map<long long, double> nodesCache; // Will incorrect if nodesCount * nodesRound > long long max
std::map<long long, double>::iterator it;


double getValueFromCache(long long nodeId, long long roundId){
    long long key = roundId * maxNodes + nodeId;
    it = nodesCache.find( key );
    if( it!=nodesCache.end() ){ // FOUND
        return it->second;
    } else { // NOT FOUND
        return -1;
    }
}

void clearCache(long long removeCaches = 0){
    if(removeCaches){
        it = nodesCache.begin();
        std::advance(it, removeCaches);
        nodesCache.erase(nodesCache.begin(), it);
    }
}

void addCache(long long nodeId, long long roundId, double value){
    long long key = roundId * maxNodes + nodeId;
    nodesCache[key] = value;
    // TODO: Reduce number of time check this by using random
    long long removeCaches = nodesCache.size()-maxCaches;
    if( removeCaches > 100000 )  clearCache(removeCaches);
}

// TODO: Not only get its value, but its number of children is also important
double* getNodesVal(long long* nodesId, long long nodesCount, long long roundId){ 
    double* res = new double[nodesCount];
    long long notCachedNodesCount = 0;
    // Check Cache
    for(int i=0; i<nodesCount; i++){
        res[i] = getValueFromCache(nodesId[i], roundId);
        if( res[i] < 0 ){  // Not found
            ++notCachedNodesCount;
        }
    }
    if(debugLevel >= 20){
        printf("getNodesVal->[notCachedNodesCount, nodeCached]: %lld %lld\n", notCachedNodesCount, nodesCount-notCachedNodesCount);
    }
    if( notCachedNodesCount == 0 ) return res;
    // Check Redis
    long long currPos = 0;
    long long* notCachedNodesId = new long long[notCachedNodesCount];
    for(int i=0; i<nodesCount; i++){
        if( res[i] < 0 ){  // Not found
            notCachedNodesId[ currPos ] = nodesId[ i ];
            ++currPos;
        }
    }
    double* resultFromRedis = getNodesValRedis( notCachedNodesId, notCachedNodesCount, roundId);
    currPos = 0;
    for(int i=0; i<nodesCount; i++){
        if( res[i] < 0 ){  // Not found
            res[i] = resultFromRedis[currPos];
            addCache(nodesId[i], roundId, res[i]);
            ++currPos;
        }
    }
    free(resultFromRedis);
    return res;
    // Check Other Workers: Not yet implemented
}

double getNodeVal(long long nodeId, long long roundId){
    long long nodesId[] = {nodeId};
    double *resArray = getNodesVal(nodesId, 1, roundId ); 
    double res = resArray[0];
    free(resArray);
    return res;
}

bool __testDataReader(){
    bool testResult = true;
    long long nodesCount = 7;
    long long roundId = 9;
    long long nodesId[] = {1, 4, 6, 7 ,8, 20, 15};
    double values[] = {1.13412341432, 4.1132413241234, 6.412341321324, 7.541323234 ,8.713241234567, 20.7456, 15.456098765437};
    setNodesValRedis(nodesId, values, nodesCount, roundId);
    double *getVals = getNodesVal(nodesId, nodesCount, roundId);
    for(int i=0; i<nodesCount; i++){
        if(! isEqual(values[i], getVals[i]) ) testResult = false;
    }
    if(debugLevel >= 20){
        for(int i=0; i<nodesCount; i++){
            printf("Set value: %lf; Get value: %lf; Is correct: %d\n", values[i], getVals[i], isEqual(values[i], getVals[i]));
        }
        printf("\n");
    }
    free(getVals);
    getVals = getNodesVal(nodesId, nodesCount, roundId);
    for(int i=0; i<nodesCount; i++){
        if(! isEqual(values[i], getVals[i]) ) testResult = false;
    }
    if(debugLevel >= 20){
        for(int i=0; i<nodesCount; i++){
            printf("Set value: %lf; Get value: %lf; Is correct: %d\n", values[i], getVals[i], isEqual(values[i], getVals[i]));
        }
        printf("\n");
    }
    free(getVals);
    getVals = getNodesVal(nodesId, nodesCount, 11);
    for(int i=0; i<nodesCount; i++){
        if(! isEqual(getVals[i], -1) ) testResult = false;
    }
    if(debugLevel >= 20){
        for(int i=0; i<nodesCount; i++){
            printf("Set value: null; Get value: %lf; Is correct: %d\n", getVals[i], isEqual(values[i], -1));
        }
        printf("\n");
    }
    printf("__testDataReader: Test result: %s\n", testResult ? "OK" : "FAIL");
    return testResult;
}