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

int main() {
    ios_base::sync_with_stdio(false); cin.tie(0);
    omp_set_num_threads(12);
    getRunningEnv(); debugLevel = 0;
    char* command = "MSET node_1_999 1.134123 node_4_999 4.113241 node_6_999 6.412341 node_7_999 7.541323 node_8_999 8.713241 node_20_999 20.745600 node_15_999 15.456099";

# pragma omp parallel for default(shared) schedule(guided)
    for0(roundId, 1000) {
        // redisContext* context = redisConnect("127.0.0.1", 6379);
        // redisReply* reply = (redisReply*)redisCommand(context, command);
        redisReply* reply = (redisReply*)redisCommand(local, command);
        debugRedisReply(reply);
        freeReplyObject(reply);
        // free(context);
    }
}
