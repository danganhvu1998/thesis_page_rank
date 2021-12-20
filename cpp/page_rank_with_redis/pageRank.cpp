#include <bits/stdc++.h>
#include <hiredis/hiredis.h>
// #include <omp.h>
#include "dataReader.h"

using namespace std;

double roundResult[MAXIMUM_NODE_SUPPORT];

void calculation(long long round) {
    currentRoundId = round;
# pragma omp parallel for default(shared)
    for (long long i = localWorkerStartNode; i < localWorkerEndNode; i++) {
        double weight = 0;
        for0(j, edgesTo[i-localLoadStartNode].size()) {
            const int fromNode = edgesTo[i-localLoadStartNode][j];
            // cout << fromNode << " " << toNodesCount[fromNode] << " " << nodeCachedValue[fromNode] << endl;
            weight += nodeCachedValue[fromNode] / toNodesCount[fromNode];
        }
        roundResult[i-localLoadStartNode] = weight;
    }
    cout << "\nSTART SET VALUE TO REDIS\n";
    long long nodeCount = localWorkerEndNode - localWorkerStartNode;
    double* values = &roundResult[0];
    long long* nodesId = new long long[nodeCount];
    for (long long i = localWorkerStartNode; i < localWorkerEndNode; i++) nodesId[i - localWorkerStartNode] = i;
    setNodesValToAllRedis(nodesId, roundResult, nodeCount, round);
    free(nodesId);
    cout << "\nDONE SET VALUE TO REDIS\n";
}

int main() {
    debugLevel = 1;
    getRunningEnv(); // Annouce with main worker here
    // init local connections
    for(int i=0; i<PHYSICAL_CORES_COUNT; i++) localConnections[i] = redisConnect("127.0.0.1", 6379);
    freopen("data/soc-LiveJournal1.out", "r", stdin);
    // freopen("data/graph_1000.data", "r", stdin);    
    // INPUT GRAPH
    cin >> N >> M;
    // N = 5000000;
    // M = N * 10;
    getTask();
    localWorkerEndNode = min(localWorkerEndNode, N + 1);
    II loadNodeData = getLoadRange(localWorkerStartNode, localWorkerEndNode, 0, N-1);
    localLoadStartNode = loadNodeData.first;
    localLoadEndNode = loadNodeData.second;
    printf("localLoadStartNode: %lld - localLoadEndNode: %lld\n", localLoadStartNode, localLoadEndNode);
    for0(i, N+4) toNodesCount[i] = 0;
    for0(i, M) {
        long long a, b;
        cin >> a >> b; // From a we can go to b
        ++toNodesCount[a];
        if (b >= localLoadStartNode && b < localWorkerEndNode) {
            // cout << b << " " << a << endl;
            edgesTo[b].push_back(a);
        }
        // edgesTo[b].push_back(a);
    }
    debugLevel = 1;
    for0(i, N) nodeCachedValue[i] = 1;
    debugTime("Done READING + INIT");
    for1(i, MAX_ROUND) {
        auto r_start = std::chrono::high_resolution_clock::now();
        currentRoundId = i;
        getTask();
        // if (i >= 3) delAllNodesAtRound(i - 3);
        calculation(i);
        auto cal_end = std::chrono::high_resolution_clock::now();
        getAllNodesValue(i);
        auto r_end = std::chrono::high_resolution_clock::now();
        cout<<endl;
        roundTime = std::chrono::duration<double, std::milli>(r_end - r_start).count();
        roundGetNodeTime = std::chrono::duration<double, std::milli>(r_end - cal_end).count();
        roundCalTime = std::chrono::duration<double, std::milli>(cal_end - r_start).count();
        runningTimesByRound.push_back(roundTime);
        calculationTimesByRound.push_back(roundCalTime);
        gettingDataTimesByRound.push_back(roundGetNodeTime);
        totalRoundTime += roundTime;
        calculateTime += roundCalTime;
        prepareTime += roundGetNodeTime;
    }
    __report();
    char* fileName = (char*)malloc(100);
    snprintf(fileName, 100, "./result/run_time_result_%lld_%lld_%ld_%lld_%lld.md", N, M, time(0), workersCount, localWorkerId);
    freopen(fileName, "w", stdout);
    __report();
    for0(i, 1000) cout << nodeCachedValue[i] << ' '; // To make sure the result is correct
}
