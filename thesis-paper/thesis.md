## 1. Abstract

+ TODO: Add comparison between the application with Spark on Page Rank with the same data set.

In recent years, it is not hard to notice that with the development of big data, we have actively accumulated a tremendous amount of data. In 2020, this number is about 64 zettabytes, and 2021 estimate to be 79 zettabytes[1]. As a result, number of data set, the size of the data set, and also the need to extract important insides from the data have been increased significantly. And among these, many are graph related: friends connections, website links, etc. As the size of the data set increased dramatically, it is now almost impossible for any single machine to handle a big data set with a reasonable amount of times, and most of the time is not even for processing, but to load the data in and out of the ram.

A common approach for over size data set is to use multiple machines to form a cluster to work on the same problem and data set. In side the cluster, depend on the compute power of the machine, each will receive a small part of the problems. But this approach have a problem: network overheated. As each machine only contains part of the data set, they have to constantly asking for data from other machines, and thus lead to the problem.

So to resolve this problem, in this paper, we propose 2 simple methods aim to reduce the column of communication data between machines, and thus lead to better performance of the application.

[1]: https://www.statista.com/statistics/871513/worldwide-data-created/

## 2. Introduction

Recently, as it is become much easier and cheaper to gather a large amount of data with a high precision, the need to extract insight from them also getting bigger. Graph data is also not an exception. In graph analysis, the data can be present as a graph with nodes present the object, and edges present the connection, or relationship between objects. Some example for this type of data is Facebook Friends where each account is a node, and each of the friend connection is and edge. As Facebook has almost 3 billions active user, with each has in average 340 friends, this data set can has 3 billions nodes with more than 1000 billions edges. By applying various algorithm to the data set, we can extract many valuable insight that is not obvious at first.

However, applying these algorithms to a big graph data set with high efficiency can be challenging. A typical large graph data set will have 4 following characteristics[1]:
  1. The size of the data set is bigger than the ram capacity of a single machine, making it impractical to process the data using only one machine
  2. Ram accesses are also very random and unpredictable, making it impossible to pre-load the data into ram beforehand.
  3. The ratio between workload computation and communication is small, leading to most of the time is for communicating between machines instead of processing the data.
  4. There is a large degree of inherent parallelism. It is possible to set up thousand of independent computers run together on the same data set without conflict. 

It is worth pointing out that the second and the third characteristics are very different from classic computing applications that normally computing dominated and much easier to predict ram access. It is a fact that in most frameworks, the application spends more time communicating than actually doing the computation.

And to make the problem worse, balancedly distributing tasks to all computers with very different specs has been proved to be very challenging as sometimes even a smaller computer outperforms the bigger one with a specific algorithm and data set. [2]

In this paper, our main contributions are:

  1. A method base on PGX.D's ghost node that can reduce the communication volume by merging them in to much bigger requests.  
  2. A method for load balancing with minimal communication needed. 

[TODO: Number instead of `SIGNIFICANTLY`]

## 3. The Problem

As we mentioned above, it is impossible for a machine to handle a big data set alone. But it is doable by using multiple machines to form a cluster, each will only have to calculate the value for a portion of nodes. For a machine, in stead of loading the whole data set, it only needs to load part of the data, enough to finish its own task. In the case of Page Rank, a machine with a nodes list `S` will need to load all the edges have node `s` belong to `S` as the destination. Or [TODO: add pseudo code to explain clearer]

Note that in this way, an edge will be loaded twice by 2 different machines if each of its nodes is not belong to the same machine. Thus, the total loaded data in the cluster always bigger than the size of the data set. 

Another characteristic of graph processing is that: many graph algorithms like Page Rank, we can divide the process into multiple rounds that each depend on the computing result of the last one. To be clear, we use the result of round i-th to calculate the result of round i+1, then using the result of round i+1 to calculate round i+2, and so on. The goal of each round is to calculate a value for all the nodes follow a pre-defined formula. In the case of Page Rank, this value is nodes' weight, and the formula is: [TODO: Add the formula here]

So for a round based algorithm, a machine in the cluster will need to kind of data to finish its task in each round:
  1. Graph structure that related to its nodes set `S`. The size of this data is very big. [TODO: add estimate compare with the size of data set]. But it will never change though the running process.
  2. Result of nodes that connected to all nodes `s` belong to `S` from last round. This data has much smaller in side, but constantly changes after each round. So it is necessary for a machine to update this data after finished every round.

A general processing step for a round based algorithm running on clusters will like below:

+ ![Error](./images/normal_round_execution.png)

There are 2 point to note about above process:
  1. Step 2 is optional, it only needed if the system applies dynamic load balancing: update task for all machines after each round aim to get better load balancing. As we mentioned above, graph data is heavy, so transfer graph data between machines in cluster will 
  2. The last round value of node get updated only when the value is referred to. For example, if the node `n` not in the machine `m`, then `n`'s value only get updated when an node belong to `m` needed its value. This will result in all the machines in cluster send too much small request ask for the value of a single node. Thus, lead to network overload. 


Processing a graph data set on a heterogeneous cluster [TODO: Heterogeneous cluster] will have 2 following problems:
1. How to limit the communication data between computer
2. How to distribute task between computer so that all the machine finish the round at the same time. 


## 4. Ideas and system design

### 4.1 :

### 4.1 Dynamic Load Balancing with limited communication



### 5. System Overview


The application is a distributed in-memory graph processing where a large graph is divided over multiple machine memory in a cluster

As many different graph processing framework, we also divide the process into multiple rounds. Each round will calculate depend on the result of the last round. The task in each round will be divided again into multiple smaller parts for multiple machines beling to the cluster. In this application we use we use Bulk Synchronous Parallel(BSP), which is each worker will have to wait other worker to finish the same round before continue to the next round. This approach will result in lower error but at the cost of higher running time (compare with Asynchronous Parallel)

In each round, the task will be again divided into multiple smaller part that can run in parallel by multiple machines in the cluster. The following image show a cluster of 4 machines, each will only incharge of processing a portion of the task for a round.

+ ![Error](./images/4workers.png)

In here, we havea cluster of 4 machines to process a graph consis of N nodes. In a round, the first machine will in charge of calculating the result of nodes 1 to A, the second is from A+1 to B, and so on. Note that for each round, the value of A, B, C will be modified for better load balanncing. 

### 3.2. Detail in each machine

+ ![Error](./images/1worker.png)

#### 3.2.1. TODO: expain about data for each node (value + structure)

#### 3.2.2. Processor

In charge of calculation the result for each node. Get the data from data data manager. The implementation of processor is quite straigh forward since from its point of view, where is the data and how to get it is not important as the data manager will take care for all of that.

#### 3.2.3. Data Manager

As for most of the graph algorithm when running on a cluster, most of the work is communication related insteading processing. So the data manger is the most important part in the application, where all of the data read / write occur.

Data manager has 3 mains task:

1. Answer request from processor about the value of nodes. To do this, data manager can always ask Local Data for the value. 
2. Update calculated result of nodes A to B from processor to both Local Data, and Local Redis
3. Get calculated result of nodes other than from A to B from other machines, and update it to Local Data


#### 3.2.4. Local Data

Simply contain all the values of all the nodes from last round
#### 3.2.5. Local Redis

Local Redis have only one task is to answer request from Data Manager of other machines in the cluster.

#### 3.2.6. Initial Load Balancing

Can be modify depend on the setup and algorithm. Will be more in depth in section [TODO: update section name when finish it]

### 3.3. Connection detail [TODO: too hard to undestand, rewrite in a better way]

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
