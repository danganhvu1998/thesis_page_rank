#include <bits/stdc++.h>
#include <hiredis/hiredis.h>
#include <omp.h>
#include "dataReader.h"

using namespace std;

vector<double> roundResult;

void calculation(long long round) {
    currentRoundId = round;
    roundResult.clear();
    for (long long i = localWorkerStartNode; i < localWorkerEndNode; i++) {
        double weight = 0;
        for0(j, edgesTo[i].size()) {
            const int fromNode = edgesTo[i][j];
            // cout << fromNode << " " << toNodesCount[fromNode] << " " << nodeCachedValue[fromNode] << endl;
            weight += nodeCachedValue[fromNode] / toNodesCount[fromNode];
        }
        roundResult.push_back(weight);
    }
    double* values = &roundResult[0];
    long long* nodesId = new long long[roundResult.size()];
    for (long long i = localWorkerStartNode; i < localWorkerEndNode; i++) nodesId[i - localWorkerStartNode] = i;
    setNodesValRedis(nodesId, values, roundResult.size(), round);
}

int main() {
    debugLevel = 1;
    getRunningEnv(); // Annouce with main worker here
    freopen("data/soc-LiveJournal1.out", "r", stdin);
    // INPUT GRAPH
    cin >> N >> M;
    localWorkerEndNode = min(localWorkerEndNode, N + 1);
    for0(i, N) toNodesCount[i] = 0;
    for0(i, M) {
        long long a, b;
        cin >> a >> b; // From a we can go to b
        ++toNodesCount[a];
        // if (b >= localWorkerStartNode && b < localWorkerEndNode) {
        //     cout << b << " " << a << endl;
        // }
        edgesTo[b].push_back(a);
    }
    for0(i, N) nodeCachedValue[i] = 1;
    debugTime("Done READING + INIT");
    for1(i, MAX_ROUND) {
        auto r_start = std::chrono::high_resolution_clock::now();
        getTask();
        currentRoundId = i;
        if (i >= 3) delAllNodesAtRound(i - 3);
        calculation(i);
        getAllNodesValue(i);
        auto r_end = std::chrono::high_resolution_clock::now();
        roundTime = std::chrono::duration<double, std::milli>(r_end - r_start).count();
        runningTimesByRound.push_back(roundTime);
        totalRoundTime += roundTime;
    }
    __report();
    char* fileName = (char*)malloc(100);
    snprintf(fileName, 100, "./result/run_time_result_%lld_%lld_%ld_%lld_%lld.md", N, M, time(0), workersCount, localWorkerId);
    freopen(fileName, "w", stdout);
    __report();
    for0(i, 1000) cout << nodeCachedValue[i] << ' '; // To make sure the result is correct
}
