#include <bits/stdc++.h>
#include "dataReader.h"

int main(){
    context = redisConnect("127.0.0.1", 6379);
    if (!context) {
        fprintf(stderr, "Error:  Can't connect to Redis\n");
        return -1;
    }
    setNodeVal(100, 10, 104.412);
    setNodeVal(100, 11, 1.41213122);
    setNodeVal(100, 12, 10.3332412);
    double a = getNodeVal (100, 10);
    double b = getNodeVal (100, 11);
    double c = getNodeVal (100, 12);
    printf("%lf, %lf, %lf", a,b,c);
    // printf("%lf", a);
}
