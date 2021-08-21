#include <bits/stdc++.h>
#include <hiredis/hiredis.h>
#include <omp.h>
#include "dataReader.h"

using namespace std;

void calculation(long long round) {
    int lastRound = round - 1;
    currentRoundId = round;
    for (long long i = localWorkerStartNode; i < localWorkerEndNode; i++) {
        double weight = 0;
        // Prepare nodes list that can to to i
        auto t_start = std::chrono::high_resolution_clock::now();
        long long* nodesId = new long long[edgesTo[i].size()];
        for0(j, edgesTo[i].size()) {
            nodesId[j] = edgesTo[i][j];
        }
        auto t_end = std::chrono::high_resolution_clock::now();
        prepareTime += std::chrono::duration<double, std::milli>(t_end - t_start).count();
        // Get nodes value from last round
        t_start = std::chrono::high_resolution_clock::now();
        double* values = getNodesVal(nodesId, edgesTo[i].size(), lastRound);
        // for0(j, edgesTo[i].size()) {
        //     cout << "ROUND " << round << ": " << i << " " << j << " " << edgesTo[i][j] << " " << values[j] << endl;
        // }
        t_end = std::chrono::high_resolution_clock::now();
        if ((i - localWorkerStartNode) % 10000 == 0) {
            cout << i - localWorkerStartNode << "th -> " << std::chrono::duration<double, std::milli>(t_end - t_start).count() << " -> " << 1.0 * cacheHitCount / (cacheHitCount + cacheMissCount) << "% " << cacheHitCount << "\n";
        }
        readTime += std::chrono::duration<double, std::milli>(t_end - t_start).count();
        free(nodesId);
        // Calculate value of node i in this round
        t_start = std::chrono::high_resolution_clock::now();
        for0(j, edgesTo[i].size()) {
            const int fromNode = edgesTo[i][j];
            weight += values[j] / toNodesCount[fromNode];
        }
        t_end = std::chrono::high_resolution_clock::now();
        calculateTime += std::chrono::duration<double, std::milli>(t_end - t_start).count();
        free(values);
        t_start = std::chrono::high_resolution_clock::now();
        setNodeVal(i, weight, round);
        t_end = std::chrono::high_resolution_clock::now();
        setTime += std::chrono::duration<double, std::milli>(t_end - t_start).count();
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

int main() {
    debugLevel = 1;
    getRunningEnv(); // Annouce with main worker here
    freopen("data/soc-LiveJournal1.out", "r", stdin);
    // INPUT GRAPH
    cin >> N >> M;
    localWorkerEndNode = min(localWorkerEndNode, N);
    for0(i, N) toNodesCount[i] = 0;
    for0(i, M) {
        long long a, b;
        cin >> a >> b; // From a we can go to b
        ++toNodesCount[a];
        // if (b >= localWorkerStartNode && b < localWorkerEndNode) edgesTo[b].push_back(a);
        edgesTo[b].push_back(a);
    }
    debugTime("Done Reading");
    // INIT WEIGHT
    for0(i, N) setNodeVal(i, 1.0, 0);
    debugTime("Done INIT");
    for1(i, MAX_ROUND) {
        cacheHitCount = 0; cacheMissCount = 0;
        currentRoundId = i;
        getTask();
        debugTime("Start round " + to_string(i));
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
        // __report();
    }
    __report();
    char* fileName = (char*)malloc(100);
    snprintf(fileName, 100, "./result/run_time_result_%lld_%lld_%ld_%lld_%lld.md", N, M, time(0), workersCount, localWorkerId);
    freopen(fileName, "w", stdout);
    __report();
    for0(i, 20) cout << getNodeVal(i, lastRound) << ' '; // To make sure the result is correct
    debugTime("Done!");
}
