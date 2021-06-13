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

long long const MAX_ROUND = 30;
double const ACCEPT_ERROR = 0.0001;
long long const oo = 1000000007, e5 = 100007, e6 = 1000007;
long long const MAXIMUM_NODE_SUPPORT = 5 * e6; // Accept maximum e6 nodes

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
    long long lastRound = round - 1;
# pragma omp parallel for default(shared) schedule(guided)
    for0(i, N) {
        // printf("NODE %lld THREAD %d\n", i, omp_get_thread_num());
        double weight = 0;
        // long long* nodesId = new long long[edgesTo[i].size()];
        long long* nodesId = (long long*)malloc(edgesTo[i].size() * sizeof(long long));
        for0(j, edgesTo[i].size()) {
            nodesId[j] = edgesTo[i][j];
        }
        double* values = getNodesVal(nodesId, edgesTo[i].size(), lastRound);
        free(nodesId);
        for0(j, edgesTo[i].size()) {
            const int fromNode = edgesTo[i][j];
            weight += values[j] / toNodesCount[fromNode];
        }
        free(values);
        setNodeVal(i, weight, round);
    }
}

bool isAcceptErrorSatisfied() {
    for0(i, N) {
        double error = abs(getNodeVal(i, lastRound) - getNodeVal(i, lastRound - 1));
        if (error > ACCEPT_ERROR) {
            // printf("        Round %lld Node %lld Error %lf\n", lastRound, i, error);
            return false;
        }
    }
    return true;
}

int main() {
    omp_set_num_threads(1);
    getRunningEnv(); debugLevel = 00;
    freopen("graph_100.data", "r", stdin);
    freopen("result_redis_openMP_1.out", "w", stdout);
    // INPUT GRAPH
    cin >> N >> M;
    for0(i, N) toNodesCount[i] = 0;
    for0(i, M) {
        long long a, b;
        cin >> a >> b; // From a we can go to b
        ++toNodesCount[a];
        if (b >= localWorkerStartNode && b < localWorkerEndNode) edgesTo[b].push_back(a);
    }
    // debugTime("Done Reading");
    // INIT WEIGHT
    for0(i, N) setNodeVal(i, 1.0, 0);
    for1(i, MAX_ROUND) {
        if (i >= 3) delAllNodesAtRound(i - 3);
        calculation(i);
        // debugTime("Done round " + to_string(i));
        lastRound = i;
        for0(i, N) cout << getNodeVal(i, lastRound) << ' '; cout << "\n";
        if (isAcceptErrorSatisfied()) break;
        // usleep(2000000);
    }
    // cout << '\n' << lastRound << " " << redisGetCount << " " << redisSetCount << " " << redisCommandCount;
}
