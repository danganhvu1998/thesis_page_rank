#include <bits/stdc++.h>
#include <hiredis/hiredis.h>
using namespace std;
void debugTime(string debugString) {
    auto currTime = chrono::system_clock::now();
    time_t humanTime = chrono::system_clock::to_time_t(currTime);
    cout << "At " << ctime(&humanTime) << "    " << debugString << "\n";
}

void strCat(char* destString, char* srcString) {
    int currPos = strlen(destString);
    int srcLen = strlen(srcString);
    for (int i = 0; i < srcLen; i++) {
        destString[currPos] = srcString[i];
        ++currPos;
    }
    destString[currPos] = '\0';
}

char dest[10000000] = "";
char src[] = "copy_this";


int main(){
    for(int i=0;i<1000000;i++){
        strCat(dest,src);
        if(i%100000==0){
            debugTime("DONE NEXT 100000\n");
        }
    }
    return 0;
}

// At Mon Nov  1 10:52:57 2021
//     DONE NEXT 100000

// At Mon Nov  1 10:52:57 2021
//     DONE NEXT 100000

// At Mon Nov  1 10:52:59 2021
//     DONE NEXT 100000

// At Mon Nov  1 10:53:03 2021
//     DONE NEXT 100000

// At Mon Nov  1 10:53:07 2021
//     DONE NEXT 100000

// At Mon Nov  1 10:53:13 2021
//     DONE NEXT 100000

// At Mon Nov  1 10:53:21 2021
//     DONE NEXT 100000

// At Mon Nov  1 10:53:33 2021
//     DONE NEXT 100000

// At Mon Nov  1 10:53:51 2021
//     DONE NEXT 100000

// At Mon Nov  1 10:54:20 2021
//     DONE NEXT 100000