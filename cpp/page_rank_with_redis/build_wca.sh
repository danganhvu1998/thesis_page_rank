rm env.h
cp ./envs/wca1.h env.h
g++ pageRank.cpp -o pagerank_wca1.out -lhiredis -fopenmp