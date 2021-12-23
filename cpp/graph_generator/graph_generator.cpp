// https://codeforces.com/contest/1336/problem/C
#include <bits/stdc++.h>
using namespace std;

#define II pair<long long, long long>
#define III pair<II, long long>
#define X first.first
#define Y first.second
#define Z second
#define all(a) (a).begin(), (a).end()
#define for0(i, n) for (long long i = 0; i < n; i++)
#define for1(i, n) for (long long i = 1; i <= n; i++)

long long const oo = 1000000007, e5 = 100007, e6 = 1000007;
long long NODE_COUNT = 5000000;
long long EDGE_PER_NODE = 10;
long long const EDGE_COUNT = NODE_COUNT * EDGE_PER_NODE;
long long seed = 621998;

void printEdge(long long from, long long to) {
    cout << from << ' ' << to << '\n';
}

long long randNode(long long fromNode = -1) {
    long long toNode = fromNode;
    while (toNode == fromNode) toNode = rand() % NODE_COUNT;
    return toNode;
}

int main(int argc, char **argv) {
    NODE_COUNT = stoi(argv[1]);
    EDGE_PER_NODE = stoi(argv[2]);
    char* fileName = (char*)malloc(100);
    snprintf(fileName, 100, "graph_%lld_%lld.out", NODE_COUNT, EDGE_PER_NODE);
    srand(seed);
    freopen(fileName, "w", stdout);
    for0(i, NODE_COUNT) {
        for0(j, EDGE_PER_NODE) {
            long long fromNode = i;
            long long toNode = randNode(fromNode);
            printEdge(fromNode, toNode);
        }
    }
}
