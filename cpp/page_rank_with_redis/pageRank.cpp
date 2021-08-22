#include <bits/stdc++.h>
#include <hiredis/hiredis.h>
#include <omp.h>
#include "dataReader.h"

using namespace std;

void calculation(long long round) {
    currentRoundId = round;
    for (long long i = localWorkerStartNode; i < localWorkerEndNode; i++) {
        double weight = 0;
        for0(j, edgesTo[i].size()) {
            const int fromNode = edgesTo[i][j];
            // cout << fromNode << " " << toNodesCount[fromNode] << " " << nodeCachedValue[fromNode] << endl;
            weight += nodeCachedValue[fromNode] / toNodesCount[fromNode];
        }
        setNodeVal(i, weight, round);
    }
}

int main() {
    debugLevel = 1;
    getRunningEnv(); // Annouce with main worker here
    freopen("data/graph_10e5.out", "r", stdin);
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
        getTask();
        currentRoundId = i;
        if (i >= 3) delAllNodesAtRound(i - 3);
        calculation(i);
        getAllNodesValue(i);
    }
    char* fileName = (char*)malloc(100);
    snprintf(fileName, 100, "./result/run_time_result_%lld_%lld_%ld_%lld_%lld.md", N, M, time(0), workersCount, localWorkerId);
    freopen(fileName, "w", stdout);
    for0(i, 20) cout << getNodeVal(i, MAX_ROUND - 1) << ' '; // To make sure the result is correct
}
