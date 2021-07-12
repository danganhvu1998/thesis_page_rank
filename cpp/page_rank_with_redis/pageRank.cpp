#include <bits/stdc++.h>
#include <hiredis/hiredis.h>
#include <omp.h>
#include "dataReader.h"

using namespace std;

#define II pair<long long, long long>
#define III pair<II, long long>
#define X first.first
#define Y first.second
#define Z second
#define all(a) (a).begin(), (a).end()
#define for0(i, n) for (long long i = 0; i < n; i++)
#define for1(i, n) for (long long i = 1; i <= n; i++)

long long const MAX_ROUND = 7;
double const ACCEPT_ERROR = 0.0001;
long long const oo = 1000000007, e5 = 100007, e6 = 1000007;
long long const MAXIMUM_NODE_SUPPORT = 5 * e6; // Accept maximum e6 nodes
double readTime, calculateTime, prepareTime, totalRoundTime, roundTime;
vector<double> runningTimesByRound;

void debugTime(string debugString) {
    auto currTime = chrono::system_clock::now();
    time_t humanTime = chrono::system_clock::to_time_t(currTime);
    cout << "At " << ctime(&humanTime) << "    " << debugString << "\n";
}

vector<long long> edgesTo[MAXIMUM_NODE_SUPPORT]; // edgesTo[i] contain list of nodes that can go to i
long long toNodesCount[MAXIMUM_NODE_SUPPORT];
long long N, M;
long long lastRound = 0;

void calculation(long long round) {
    int lastRound = round - 1;
    for (long long i = localWorkerStartNode; i < localWorkerEndNode; i++) {
        double weight = 0;
        // #################################
        auto t_start = std::chrono::high_resolution_clock::now();
        long long* nodesId = new long long[edgesTo[i].size()];
        for0(j, edgesTo[i].size()) {
            nodesId[j] = edgesTo[i][j];
        }
        auto t_end = std::chrono::high_resolution_clock::now();
        prepareTime += std::chrono::duration<double, std::milli>(t_end - t_start).count();
        // #################################
        t_start = std::chrono::high_resolution_clock::now();
        double* values = getNodesVal(nodesId, edgesTo[i].size(), lastRound);
        t_end = std::chrono::high_resolution_clock::now();
        readTime += std::chrono::duration<double, std::milli>(t_end - t_start).count();
        free(nodesId);
        // #################################
        t_start = std::chrono::high_resolution_clock::now();
        for0(j, edgesTo[i].size()) {
            const int fromNode = edgesTo[i][j];
            weight += values[j] / toNodesCount[fromNode];
        }
        t_end = std::chrono::high_resolution_clock::now();
        calculateTime += std::chrono::duration<double, std::milli>(t_end - t_start).count();
        free(values);
        setNodeVal(i, weight, round);
    }
}

bool isAcceptErrorSatisfied() {
    for0(i, N) {
        double error = abs(getNodeVal(i, lastRound) - getNodeVal(i, lastRound - 1));
        if (error > ACCEPT_ERROR) {
            return false;
        }
    }
    return true;
}

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

    cout << "\n\n ### RUNNING TIME BY ROUND:\n";
    cout << "\n  + ALL ROUNDS: " << totalRoundTime << "\n  + AVERAGE: " << totalRoundTime / runningTimesByRound.size();
    for (int i = 0; i < runningTimesByRound.size(); i++) {
        cout << "\n     + ROUND " << i + 1 << ": " << runningTimesByRound[i];
    }

    cout << "\n\n## REPORT BY PERCENTAGE RUNNING TIME";
    cout << "\n\n ### REDIS COMMAND:\n";
    cout << "\n  + SET CMD RUNNING TIME: " << redisSetCmdRunningTime / totalRoundTime * 100 << "\n    + AVERAGE SET CMD: " << redisSetCmdRunningTime / redisSetCount / totalRoundTime * 100;
    cout << "\n  + SET CMD RUNNING TIME: " << redisGetCmdRunningTime / totalRoundTime * 100 << "\n    + AVERAGE GET CMD: " << redisGetCmdRunningTime / redisGetCount / totalRoundTime * 100;

    cout << "\n\n ### RUNNING TIME BY FUNCTIONS:\n";
    cout << "\n  + TOTAL READ TIME: " << readTime / totalRoundTime * 100;
    cout << "\n    + CACHE TIME: " << cacheTime / totalRoundTime * 100;
    cout << "\n    + REDIS READ TIME: " << redisReadTime / totalRoundTime * 100;
    cout << "\n  + TOTAL CALCULATION TIME: " << calculateTime / totalRoundTime * 100;

    cout << "\n\n ### RUNNING TIME BY ROUND:\n";
    cout << "\n  + ALL ROUNDS: " << totalRoundTime / totalRoundTime * 100 << "\n  + AVERAGE: " << totalRoundTime / runningTimesByRound.size() / totalRoundTime * 100;
    for (int i = 0; i < runningTimesByRound.size(); i++) {
        cout << "\n     + ROUND " << i + 1 << ": " << runningTimesByRound[i] / totalRoundTime * 100;
    }
    cout << "\n\n";
}

int main() {
    getRunningEnv(); debugLevel = 1;
    redisCommand(local, "FLUSHALL");
    freopen("graph_1000.data", "r", stdin);
    // INPUT GRAPH
    cin >> N >> M;
    localWorkerEndNode = min(localWorkerEndNode, N);
    for0(i, N) toNodesCount[i] = 0;
    for0(i, M) {
        long long a, b;
        cin >> a >> b; // From a we can go to b
        ++toNodesCount[a];
        if (b >= localWorkerStartNode && b < localWorkerEndNode) edgesTo[b].push_back(a);
    }
    debugTime("Done Reading");
    // INIT WEIGHT
    for0(i, N) setNodeVal(i, 1.0, 0);
    for1(i, MAX_ROUND) {
        auto r_start = std::chrono::high_resolution_clock::now();
        nodesCache.clear();
        if (i >= 3) delAllNodesAtRound(i - 3);
        calculation(i);
        debugTime("Done round " + to_string(i));
        lastRound = i;
        auto r_end = std::chrono::high_resolution_clock::now();
        roundTime = std::chrono::duration<double, std::milli>(r_end - r_start).count();
        runningTimesByRound.push_back(roundTime);
        totalRoundTime += roundTime;
    }
    __report();
    char* fileName = (char*)malloc(100);
    snprintf(fileName, 100, "./result/run_time_result_%lld_%lld_%d_%lld_%lld.md", N, M, time(0), workersCount, localWorkerId);
    freopen(fileName, "w", stdout);
    __report();
    for0(i, 20) cout << getNodeVal(i, lastRound) << ' '; // To make sure the result is correct
    // debugTime("Done!");
}
