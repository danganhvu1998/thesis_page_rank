THESIS - Page Rank on Heterogeneous Clusters
=====

# Abstract

+ TODO: Add comparation between the application with Spark on Page Rank with the same data set.

In recent years, it is not hard to notice that with the development of big data, we have actively accumulated a tremedous amount of data. In 2020, this number is about 64 zettabytes, and 2021 estimate to be 79 zettabytes[1]. As a result, number of data set, the size of the data set, and also the need to extract important insides from the data have been increased significantly. And among these, many are graph related: friends connections, website links, etc. As the size of the data set increased dramatically, it is now almost imporsible for any single machine to handle a big data set with a reasonable amount of times, and most of the time is not even for processing, but to load the data in and out of the ram.

To deal with this problem with low cost, a common method is to use multiple machines with different spec, each will load / process only part of the data set and in constance communication. Depend on the computing power, each computer will be assigned bigger or smaller task(Heterogeneous Clusters). However, this approce faces 2 big problems: Communicate via network is much slower, and how to distribute task balancely.

In this paper, we propose a method to redude the network communication, and a new approche about load balancing that will lead to better perfromance.

[1]: https://www.statista.com/statistics/871513/worldwide-data-created/

# Introduction

Getting insides from a big graph related data set can be challenging. An typical large graph data set will have 4 following characteristics:
  1. The side of the data set is very big, bigger than the ram capacity of a single machine, make it impractical to process the data using only one machine
  2. Ram accesses are also very ramdom and umpredictable, make it imposible to know which part of the data set should be loaded in advance. This make pre-loading a segmant of the data set only be impossible.
  3. The ratio between work load computation and communication is extremly small, leading to it is more practical to improve communication strategy rather than impove the computation process.
  4. There is very large degree of inherent parallelism. With the right setup, thousand of independence machines can easily process the same data set without any conflict.

As the result of these above characteristics, one common approche is to use a cluster of machine to work together on the same data set. Many famost frameworks are using this approach such as GraphLab, or GraphX, but the performance is disapoting ([From: PGX.D, but will need actual data from experiment](../paper/PGX.D.pdf)). Another framework is PGX.D included many technique to reduce data sent via network, claims that they can out perform a single machine with enough RAM with a cluster from 2-16 computers. But this result was built on top of a very expensive network systems with high bradwith (56Gb/s) and ultra low latancy(1us), which is not something everyone can affort, make the penalty of sending data via network significanly lower than normal setup.

In this paper, we present 2 simple methods that can significantly reduce the network traffict being sent during the process, and thus lead to reduce running time as a whole. Although these 2 method are not able to significantly improve the running time in all setup and all algorithms, 



+ [Cite: PGX.D Point](../paper/PGX.D.pdf)
  + Recently, graph analysis has been drawing a lot of attention fromboth industry and academia, as a powerful method for data analy-sis.   In  graph  analysis,  the  underlying  data  set  is  represented  asa graph such that the arbitrary, fine-grained relationships betweendata entities are explicitly captured as the edges in the graph.  By following these edges, graph analysis naturally considers even theindirect, multi-hop relationships across the whole dataset.  Conse-quently, graph analysis can discover valuable, non-obvious infor-mation about the original data set.
  + However,  applying graph analysis on a very large dataset in afast and scalable manner can be challenging.  A typical large-scalegraph analysis exhibits the following characteristics:
    + The size of the graph is very large – larger than the capacityof a single machine’s main memory.
    + Many small-sized random memory accesses over the wholegraph  instance  are  performed,  especially  during  neighbor-hood iterations and graph traversals.
    + The  computation-to-communication  ratio  is  low  –  i.e.,  theamount of computation is small compared to the amount ofdata movement.
    + There is a large degree of inherent parallelism
+ The same characteristics are also correct with page rank. TODO: Greater detail about its with detail number  (eg: 99% spend on communication related work)
  + TODO: Some special characteristic like increase task will reduce time on each round.
+ TODO: Deeper comparation between the application with Spark

<!-- Write down method that use in the application here, and also general number how much faster -->
<!-- Consider to move some points to method instead of introduction -->
1. Load all the nodes weight at the beginning
   1. TODO: Compare with bulk request
   2. TODO: Compare with Ghost Nodes 
   3. TODO: Why this is doable for page rank
      1. How about other algorithm.
2. Load balancing
   1. Algo behind it
   2. Greatly depend on the algorithm ( ratio of computing time / communicating time )
   3. Preload extra data
      1. Good: reduce data has to be sent significantly
      2. Bad: Can - regulate to the better state, cannot go to optimal point if the task distributed too unbalance at first.

# The Application: <!-- Explain the process flow in detail, with example -->

## Overall Structure, Connection

+ ![Error](./images/4workers.png)

## Connection in detail

![Error](./images/1worker.png)

<!-- TODO: Simplify following explanation -->
+ Connection (1): Between `Processor` and `Data Reader`
  + GET: `Data Reader` answers request from processor about requested data. `Processor` will ask question like: What is the value of ```nodesList = [1, 4, 400, 123104, 6021998, 20041998]``` after ```round = 15```. Processor will not care if all the nodes are from the same workers, or in different workers.
  + SET: `Processor` will request sth like: Set the value of  ```nodesList = [15, 16, 17, 1001, 1002, 1003]``` with ```values = [0.5, 0.456, 0.789, 0.568, 0.154, 2.465]```. It guarantees that all node in `nodesList` is belong to the current worker.

+ Connection (2):  Between `Data Reader` and `Local Cache`
  + GET: `Data Reader` request the `Local Cache` to answer question like: What is the value of ```nodesList = [1, 4, 400, 123104, 6021998, 20041998]``` after ```round = 15```. It is exactly the same with the question `Processor` asks `Data Reader` before. Node that it cannot be sure that `The Cache` has all the needed data. So the answer would be sth like: ```values = [0.5, -1, -1, 0.154, 2.465, -1]``` with -1 mean the `Local Cache` does not have needed value.
  + SET: Add data to the `Local Cache`. If the number of records inside the `Local Cache` becomes greater than a threshold, it will remove some oldest records. SET will be called after `Data Reader` get new data from `Local Redis` of current worker, or `Local Redis` from other workers

+ Connection (3 + 4): Between `Data Reader` and `Local Redis`
  + GET: `Data Reader` request the `Local Redis` to answer question like: What is the value of ```nodesList = [1, 4, 400, 123104, 6021998, 20041998]``` after ```round = 15```. It is exactly the same with GET request from (2), but expected to receive all the answer.
  + SET: `Data Reader` will request the `Local Redis` sth like: Set the value of  ```nodesList = [15, 16, 17, 1001, 1002, 1003]``` with ```values = [0.5, 0.456, 0.789, 0.568, 0.154, 2.465]```. It is same as SET in (1). It is guarantee that `Data Reader` will never request SET for other workers' `Local Redis`

+ Connection (5): It's the same as GET in (4). The only different is that it answers the request from `Data Reader` of other workers

## Flow

+ Flow of a worker that initially assigned nodes from A to B
  + Step 1: Load the initially  assigned data + buffer data from Hard Drive.
    + Assigned Data: From A to B    
    + Buffer Data: From A - (B-A)/2 to A, and B+ (B-A)/2
    + From (i+ii), the worker will load from node A - (B-A)/2 to B+ (B-A)/2
  + Step 2: Get all nodes’ weight from the last round. 
  + Step 3: Get the assigned node for the coming round. It is guaranteed that all the nodes are from A - (B-A)/2 to B+ (B-A)/2
  + Step 4: Calculate the new weight if assigned nodes of this round
  + Step 5: Load the calculated result to local Redis for other workers to read in step 2 of next round
  + Step 6: Go back to step 2 if not the final round. Otherwise end the program.
+ How to re-distribute task between round, example with a worker that initially assigned nodes from A to B
  + The worker already loaded data from node A - (B-A)/2 to B+ (B-A)/2. 
  + At the beginning of each round, the worker will receive the task for that round represented by 2 numbers A’ and B’. This means for this round the worker will calculate the weight of nodes from  A’ to B’.
  + It is guaranteed that A - (B-A)/2 <= A’ < B’ <= B+ (B-A)/2. 
    + By this, the worker does not need to read any new data from Hard Drive or have to ask other workers.  => re-distribute tasks is very easy and fast. (Good)
    + But this also means that it cannot go beyond A - (B-A)/2 and B+ (B-A)/2, thus reducing the flexibility of the system. (Bad)

# Method to reduce network data <!-- Deeper info about each method. Check the method in Introduction -->

## Method 1

## Method 2

## Method 3

# Result
<!-- Compare with existing framework -->
<!-- Compare the application with and without each method -->

# Discussion
<!-- ???  -->

# Conclusion
<!-- ???  -->

# Related Work
<!-- ???  -->

# References
<!-- ???  -->

# NOTE

+ Writing Style: https://www.cs.columbia.edu/~hgs/etc/writing-style.html
+ How to write a paper: https://onlinelibrary.wiley.com/doi/epdf/10.1002/adma.200400767
+ How to read a paper: [link](../paper/HowtoReadPaper.pdf)