rm env.h
cp ./envs/wca1.h env.h
g++ pageRank.cpp -o pagerank_wca1 -lhiredis -fopenmp

rm env.h
cp ./envs/wca2.h env.h
g++ pageRank.cpp -o pagerank_wca2 -lhiredis -fopenmp

rm env.h
cp ./envs/wca3.h env.h
g++ pageRank.cpp -o pagerank_wca3 -lhiredis -fopenmp

rm env.h
cp ./envs/wca4.h env.h
g++ pageRank.cpp -o pagerank_wca4 -lhiredis -fopenmp

rm env.h
cp ./envs/wca5.h env.h
g++ pageRank.cpp -o pagerank_wca5 -lhiredis -fopenmp

rm env.h
cp ./envs/wca6.h env.h
g++ pageRank.cpp -o pagerank_wca6 -lhiredis -fopenmp

rm env.h
cp ./envs/wca7.h env.h
g++ pageRank.cpp -o pagerank_wca7 -lhiredis -fopenmp