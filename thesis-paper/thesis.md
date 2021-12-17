## 1. Abstract

It is an obvious fact that we have actively accumulated a tremendous amount of data. Before 2000, it is unthinkable that we now have the power to access zettabytes of high accuracy data from all the fields from humans, medical, or even tomatoes. As the volume of data increased, so does the need of analyzing the data for insight that can turn back to benefit us. Graph analysis is not an exeption. In graph analysis, the data can be presented as a graph with nodes presenting the object and edges presenting the connection or relationship between objects. And by applying various algorithms to the data set, we can extract much valuable insight that is not obvious at first. But there is a problem: as the size of the data set gets bigger and bigger, it is now impractical for a regular computer to process a large data set that can be hundreds of times bigger than its ram capacity. A common method for dealing with this problem is to use multiple machines to work together on the same data set. In this approach, each computer will load only a small part of the data set that can fit into its ram capacity, and only execute part of the big process. The final result will be aggregated later. And with the development of multiple cloud computing services like AWS, to cut the cost even further, it is very common that each computer will have different specs, some might even be a virtual one in the cloud. However, this approach faces 2 big problems: The first one is network overhead: communication via the network explode while running, thus leading to waste of computing power as the machine has to wait for its needed data. And the second one is that it is impossible to know in advance how to distribute tasks optimally. In some cases, a computer with less compute power can finish the same task faster than a more powerful one. So to resolve that, in this paper, we propose 2 methods to reduce network communication that can speed up the process: one aims to reduce communication when computing, and another aims for better load balancing. Although we only apply these 2 methods to the Page-rank algorithm, we believe that they can be developed further for other graph-related algorithms as well.

## 2. Introduction

Recently, as it is become much easier and cheaper to gather a large amount of data with high precision, the need to extract insight from them also getting bigger. Graph-related data is also not an exception. In graph analysis, the data can be presented as a graph with nodes presenting the object and edges presenting the connection or relationship between objects. An example of this is Facebook friends where each account is a node, and each of the friend connections is an edge. As Facebook has almost 3 billion active users, with each having an average of 340 friends, this data set can have 3 billion nodes with more than 1000 billion edges. By applying various algorithms to the data set, we can extract much valuable insight that is not obvious at first: who is the one with high influences, what is trending now between a certain type of users, etc. [TODO: More about why this is interesting]
However, applying these algorithms to a big graph data set with high efficiency can be challenging. A typical large graph data set will have 4 following characteristics[1]:
  1. The size of the data set is bigger than the ram capacity of a single machine, making it impractical to process the data using only one machine
  2. Ram accesses are also very random and unpredictable, making it impossible to pre-load the data into ram beforehand.
  3. The ratio between workload computation and communication is small, leading to most of the time is for communicating between machines instead of processing the data.
  4. There is a large degree of inherent parallelism. It is possible to set up thousand of independent computers run together on the same data set without conflict. 

Another characteristic of graph processing is that: many graph algorithms like Page Rank, we can divide the process into multiple rounds that each depend on the computing result of the last one. To be clear, we use the result of round i-th to calculate the result of round i+1, then using the result of round i+1 to calculate round i+2, and so on. The goal of each round is to calculate a value for all the nodes following a pre-defined formula. In the case of Page Rank, this value is nodes' weight, and the formula is: [TODO: Add the formula here]

So for a round-based algorithm, a machine in the cluster will need to kind of data to finish its task in each round:
  1. Graph structure that related to its nodes set `S`. The size of this data is very big. But it will never change through the running process.
  2. Result of nodes that connected to all nodes `s` belong to `S` from the last round. This data has much smaller in size but constantly changes after each round. So it is necessary for a machine to update this data after finishing every round.

A general processing step for a round-based algorithm running on clusters will like below:

+ ![TODO: Remake the image about process]

There are 2 points to note about the above process:
  1. Step 2 is optional, it is only needed if the system applies dynamic load balancing: update task for all machines after each round aim to get better load balancing. As we mentioned above, graph data is heavy, so transferring graph data between machines will take a lot of time, depending on the degree of how unbalanced the last round is.
  2. The last round values of nodes get updated only when the value is referred to. For example, if the node `n` is not in the machine `m`, then `n`'s value only gets updated when a node belonging to `m` needed its value. This will result in all the machines in the cluster sending too many small requests to ask for the value of a single node. This leads to network overload. 

As described above, one of the problems with graph processing is network overheating due to too many small requests. This leads to most of the methods aiming to increase the efficiency of graph processing is about reducing communication between computers, which is very slow due to latency and network capacity. And to do that, one of the easiest and also the most effective is sending/receiving data in bulk. Instead of asking or sending data for 1 node at a time, we can do it for 10000 nodes. 

To make the problem worse, balancedly distributing tasks to all computers with very different specs has been proved to be very challenging as sometimes even a smaller computer outperforms the bigger one with a specific algorithm and data set. To reduce the unbalance between machines in the cluster, we can dynamically re-assign tasks between each round. The idea is simple, slower machines will send their task to faster machines at the end of each round so that they can all finish at the same time next round. It is expected that for the first few rounds, the difference between the slowest machine and the fastest machine is big, but will get smaller and smaller after each round

+ ![Error](./images/actual_run_time.png)
[TODO: Add explanation about the image]

Over many years, multiple graph frameworks have been developed like GraphX or PGX.D, but there is still much room for improvement. In this paper, we introduce 2 methods that can even further reduce total network communication: one method aims to reduce network data when processing and another is for dynamic load balancing with under 1Kb data communication needed. 

The structure of the paper is as follow:
In section 3, we will provide further detail about the facing problem of graph processing. 
In section 4, we will describe our ideas, and how did we apply them to our system.
After that, in section 5, we will evaluate the system by comparing it with GraphX, a well-known graph processing framework.

In this paper, our main contributions are:

  1. A method base on PGX.D's ghost node that can reduce the communication volume by merging them in to much bigger requests.  
  2. A method for load balancing with minimal communication needed. 


## 4. Ideas and system design

+ ![Error](./images/4workers.png)

### 4.1 Dynamic Load Balancing with limited communication

As we mentioned above, dynamic load balancing can take a long time due to the slower machine have to send a large amount of data about graph structure to the faster machine. To reduce the communication volume, each machine can load extra data from the begining. For example, if a task of a machine is compute the value of nodes belong to array INIT_NODE, it will not only load the edges that contain nodes from INIT_NODE, but also the nodes that belong to array EXTRA_NODE. When that machine have to receive more task from slower machines, it will only accept nodes that come from EXTRA_NODE. By this, no data about the graph structure is needed, only data about extra task. The bigger the size of EXTRA_NODE, the more flexible the system is. For example, if EXTRA node is as big as INIT_NODE, then the faster machine can receive as much as 2 times inital task. Depend on the ram of the machine, and how confident we are about the initial load balancing, we can set up the size of EXTRA_NODE accordingly.
This method is of course, have some drawbacks. For example, if the initial load balancing is highly unbalanced, then if the fast machine is run 5 time faster than the slower machine, it is impossible to it to get more task than 2 times currently. So this method is only able to modify task distribution to a certain degree.

[TODO: add image to explain the point above]

### 4.2 Complete Nodes Copy

In page rank algorithm on a heterogeneous cluster, very often that each machine will have to download 95% of all the nodes from all other machines [TODO: More explanation]. And also, the cost of storing node value is relatively small compared with the graph size. For example, a real-life graph data set with 787.801.471 nodes and 47.614.527.250 is about 11GB, but to be able to store all the values for all the nodes, we just need less than 1GB if using double, or 0.5GB if using float. [https://law.di.unimi.it/webdata/uk-2014/]

So instead of waiting until a certain node value is needed, each machine will actively download all the node values from other machines. This helps to simplify the program and enables us to send/get in much bigger bulks. 

The idea is that we decided a bulk_size in advance that is the maximum number of nodes that can be sent in one pack. Then for each machine that is in charge of all the nodes from A to B, it will divide all its nodes into (B-A+1) / bulk_size packs, and send them to other machines. 

### 4.2 System Overview


There are some important point in our system is that:

1. Instead of randomly distribute nodes to machine as usual [TODO: np hard problem to do better], we assign nodes to a machine by 2 number `A` and `B` means that machine's task is calculate value for all the node from `A` to `B`. The value of `A` and `B` can be updated depend on the running time of last round.

#### 4.2.1 Flow

+ ![Error](./images/system_design.png)

Flow of a worker that initially assigned nodes from A to B
1. Step 1: Load graph structure data for the initially assigned node, and also buffer data from Hard Drive.
   1. Assigned Data: From A to B    
   2. Buffer Data: From `A-(B-A)/2` to `A`, and `B` to `B+(B-A)/2`
   3. So the worker will load graph structure from node `A -(B-A)/2` to `B+(B-A)/2`
2. Step 2: Get all nodes’ weight from the last round. 
3. Step 3: Get the assigned node for the coming round. It is guaranteed that all the nodes are from `A - (B-A)/2` to `B+ (B-A)/2` so that no new graph data need to update.
4. Step 4: Calculate the new weight if assigned nodes of this round
5. Step 5: Load the calculated result to local Redis for other workers to read in step 2 of next round
6. Step 6: Go back to step 2 if not the final round. Otherwise end the program.

How to re-distribute task between round, example with a worker that initially assigned nodes from A to B
1. The worker already has graph structure data from node `A - (B-A)/2` to `B+ (B-A)/2`. 
2. At the beginning of each round, the worker will receive the task for that round represented by 2 numbers `A’` and `B’`. This means for this round the worker will calculate the weight of nodes from  `A’` to `B’`. It is guaranteed that `A - (B-A)/2` <= `A’` < `B’` <= `B+ (B-A)/2`. 
3. By this, the worker does not need to read any new data from Hard Drive or have to ask other workers.  => re-distribute tasks is very easy and fast.
4. But this also means that it cannot go beyond A - (B-A)/2 and B+ (B-A)/2, thus reducing the flexibility of the system.

## Evaluation

f