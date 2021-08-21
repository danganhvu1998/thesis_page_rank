#include <bits/stdc++.h>
#include <hiredis/hiredis.h>
#include "env.h"

using namespace std;

#define II pair<long long, long long>
#define III pair<II, long long>
#define X first.first
#define Y first.second
#define Z second
#define all(a) (a).begin(), (a).end()
#define for0(i, n) for (long long i = 0; i < n; i++)
#define for1(i, n) for (long long i = 1; i <= n; i++)

typedef struct _worker {
    char* ip;
    redisContext* redis;
    double clockRate, ram, coresCount, lastRoundRunningTime;
    long long startNode, endNode;
} worker;

// SETTING PARAMS
long long const MAX_ROUND = 7;
double const ACCEPT_ERROR = 0.0001;
long long const oo = 1000000007, e5 = 100007, e6 = 1000007;
long long const MAXIMUM_NODE_SUPPORT = 5 * e6; // Accept maximum e6 nodes
long long const MAX_WORKERS = 100;
long long const MAX_THREADS = 15;

// Setting workers
char* ip[MAX_WORKERS];
long long workersNodeStart[MAX_WORKERS];
long long workersNodeEnd[MAX_WORKERS];
long long workersCount = 1;
long long maxThreads = 1;

// For redis communication
redisContext* local = redisConnect("127.0.0.1", 6379);
redisContext* mainWorkerRedis = redisConnect(MAIN_WORKER_IP_ADDRESS, 6379);
redisContext* workersContext[MAX_WORKERS][MAX_THREADS];
worker workersList[MAX_WORKERS];

// VARIABLES
vector<double> runningTimesByRound;
vector<long long> edgesTo[MAXIMUM_NODE_SUPPORT]; // edgesTo[i] contain list of nodes that can go to i
long long toNodesCount[MAXIMUM_NODE_SUPPORT];
long long N, M;
long long lastRound = 0;

// If support openMP
long long CURRENT_THREAD = 0;

// Local setting
long long localWorkerStartNode, localWorkerEndNode;
long long localWorkerId = 0;
long long redisGetCount = 0, redisSetCount = 0, redisCommandCount = 0;
long long debugLevel = 100;

// For checking running status
double readTime, calculateTime, prepareTime, totalRoundTime, roundTime, setTime, cacheTime, redisReadTime;
double redisSetCmdRunningTime, redisGetCmdRunningTime, redisStringToDoubleConvertTime;

// 
long long currentRoundId = 0;
long long const maxNodes = 100000000007; // 1e11
// TODO: Get ram size and use only 20-30% of it
long long const maxCaches = 50000007; // 5 * 1e7
int cacheHitCount = 0, cacheMissCount = 0, reportCacheHitAfterTimes = 1000; // Set to -1 to have no report
vector<int> cacheHitByRound[MAX_ROUND + 2];


int currentDateTime() {
    return time(0);  // t is an integer type
}

void __report() {

    cout << "\n# SIZE: NODE_COUNT: " << N << "; EDGE_COUNT: " << M << "; ROUND_COUNT: " << MAX_ROUND << "; WORKER_COUNT: " << workersCount << "; WORKER_ID: " << localWorkerId;

    cout << "\n\n## REPORT BY RUNNING TIME (MS)";
    cout << "\n\n ### REDIS COMMAND:\n";
    cout << "\n  + SET CMD RUNNING TIME: " << redisSetCmdRunningTime << "\n    + AVERAGE SET CMD: " << redisSetCmdRunningTime / redisSetCount;
    cout << "\n  + SET CMD RUNNING TIME: " << redisGetCmdRunningTime << "\n    + AVERAGE GET CMD: " << redisGetCmdRunningTime / redisGetCount;

    cout << "\n\n ### RUNNING TIME BY FUNCTIONS:\n";
    cout << "\n  + TOTAL READ TIME: " << readTime;
    cout << "\n    + CACHE TIME: " << cacheTime;
    cout << "\n    + REDIS READ TIME: " << redisReadTime;
    cout << "\n  + TOTAL CALCULATION TIME: " << calculateTime;
    cout << "\n  + TOTAL SET TIME: " << setTime;

    cout << "\n\n ### RUNNING TIME BY ROUND:\n";
    cout << "\n  + ALL ROUNDS: " << totalRoundTime << "\n  + AVERAGE: " << totalRoundTime / runningTimesByRound.size();
    for (int i = 0; i < runningTimesByRound.size(); i++) {
        cout << "\n     + ROUND " << i + 1 << ": " << runningTimesByRound[i];
    }

    cout << "\n\n## REPORT BY PERCENTAGE RUNNING TIME";
    cout << "\n\n ### REDIS COMMAND:\n";
    cout << "\n  + SET CMD RUNNING TIME: " << redisSetCmdRunningTime / totalRoundTime * 100 << "\n    + AVERAGE SET CMD: " << redisSetCmdRunningTime / redisSetCount / totalRoundTime * 100;
    cout << "\n  + GET CMD RUNNING TIME: " << redisGetCmdRunningTime / totalRoundTime * 100 << "\n    + AVERAGE GET CMD: " << redisGetCmdRunningTime / redisGetCount / totalRoundTime * 100;

    cout << "\n\n ### RUNNING TIME BY FUNCTIONS:\n";
    cout << "\n  + TOTAL READ TIME: " << readTime / totalRoundTime * 100;
    cout << "\n    + CACHE TIME: " << cacheTime / totalRoundTime * 100;
    cout << "\n    + REDIS READ TIME: " << redisReadTime / totalRoundTime * 100;
    cout << "\n  + TOTAL CALCULATION TIME: " << calculateTime / totalRoundTime * 100;
    cout << "\n  + TOTAL SET TIME: " << setTime / totalRoundTime * 100;;

    cout << "\n\n ### RUNNING TIME BY ROUND:\n";
    cout << "\n  + ALL ROUNDS: " << totalRoundTime / totalRoundTime * 100 << "\n  + AVERAGE: " << totalRoundTime / runningTimesByRound.size() / totalRoundTime * 100;
    for (int i = 0; i < runningTimesByRound.size(); i++) {
        cout << "\n     + ROUND " << i + 1 << ": " << runningTimesByRound[i] / totalRoundTime * 100;
    }
    cout << "\n\n ### CACHE HIT RATE:\n";
    for1(i, MAX_ROUND) {
        cout << "\n ROUND " << i << ": [";
        for0(j, cacheHitByRound[i].size()) cout << cacheHitByRound[i][j] << ",";
        cout << "]\n";
    }

}

void debugTime(string debugString) {
    auto currTime = chrono::system_clock::now();
    time_t humanTime = chrono::system_clock::to_time_t(currTime);
    cout << "At " << ctime(&humanTime) << "    " << debugString << "\n";
}

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

void strCat(char* destString, char* srcString) {
    int currPos = strlen(destString);
    int srcLen = strlen(srcString);
    for (int i = 0; i < srcLen; i++) {
        destString[currPos] = srcString[i];
        ++currPos;
    }
    destString[currPos] = '\0';
}

int split(char* str, char* seperator, char** res) {
    char* ptr = strtok(str, seperator);
    int curr = 0;
    while (ptr != NULL)
    {
        free(res[curr]);
        res[curr] = new char[50];
        strcpy(res[curr], ptr);
        ++curr;
        ptr = strtok(NULL, seperator);
    }
    return curr;
}

void createNewWorker(char* ipAddress, int pos) {
    workersList[pos].ip = new char[50];
    strcpy(workersList[pos].ip, ipAddress);
    workersList[pos].redis = redisConnect(ipAddress, 6379);
}

int getWorkerById(int id) {
    if (workersList[id].ip == NULL) createNewWorker(ip[id], id);
    return id;
}

int getWorkerByIp(char* ipAddress) {
    for (int i = 0; i < MAX_WORKERS;i++) {
        if (!strcmp(workersList[i].ip, ipAddress)) {
            return getWorkerById(i);
        }
    }
}

void printWorker(worker w) {
    printf("\nWORKER %s at ROUND %lld\nSTART - END: %lld - %lld\n", w.ip, currentRoundId, w.startNode, w.endNode);
}
