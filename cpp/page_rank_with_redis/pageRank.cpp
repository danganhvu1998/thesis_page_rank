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

long long const MAX_ROUND = 5;
double const ACCEPT_ERROR = 0.0001;
long long const oo = 1000000007, e5 = 100007, e6 = 1000007;
long long const MAXIMUM_NODE_SUPPORT = 5 * e6; // Accept maximum e6 nodes
double readTime, calculateTime, prepareTime, freeRunningTime;

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
    for0(i, N) {
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
        t_start = std::chrono::high_resolution_clock::now();
        free(values);
        t_end = std::chrono::high_resolution_clock::now();
        freeRunningTime += std::chrono::duration<double, std::milli>(t_end - t_start).count();
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

int main() {
    getRunningEnv(); debugLevel = 1;
    freopen("graph_10e5.out", "r", stdin);
    // INPUT GRAPH
    cin >> N >> M;
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
        if (i >= 3) delAllNodesAtRound(i - 3);
        calculation(i);
        debugTime("Done round " + to_string(i));
        lastRound = i;
    }
    cout << '\n' << lastRound << " " << redisGetCount << " " << redisSetCount << " " << redisCommandCount;
    cout << "\nSET CMD RUNNING TIME: " << redisSetCmdRunningTime << "\nGET CMD RUNNING TIME: " << redisGetCmdRunningTime << "\nCONVERT TIME: " << redisStringToDoubleConvertTime << "\n";
    cout << "\nAVERAGE GET CMD: " << redisGetCmdRunningTime / redisGetCount;
    cout << "\nAVERAGE SET CMD: " << redisSetCmdRunningTime / redisSetCount;
    cout << "\n\nTOTAL PREPARE TIME: " << prepareTime << "\nTOTAL READ TIME: " << readTime << "\nTOTAL CALCULATION TIME: " << calculateTime << "\nFREE RUNNING CMD" << freeRunningTime;
    freopen("result_redis_10e6.out", "w", stdout);
    for0(i, N) cout << getNodeVal(i, lastRound) << ' ';
}
