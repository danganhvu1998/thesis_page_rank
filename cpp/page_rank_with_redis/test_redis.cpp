#include <bits/stdc++.h>
#include "dataReader.h"

int main() {
    setNodeVal(100, 10, 104.412);
    setNodeVal(100, 11, 1.41213122);
    setNodeVal(100, 12, 10.3332412);
    double a = getNodeVal(100, 10);
    double b = getNodeVal(100, 11);
    double c = getNodeVal(100, 12);
    printf("%lf, %lf, %lf", a, b, c);
    // printf("%lf", a);
}
