Todo list
====

## Project Purpose

- Speed up graph computing on Heterogeneous Clusters by sharing workload between workers
  - Expect speed up by 10-30% when using BSP. Probablly not compatible with SSP or ASP 
    - There is no proof yet, and there is a chance this target is not possible to make
      - Depend on how much workload the slow workers send to fast worker
  - But we also adding a stage for each round, that should not take much time. But still a point to pay attendtion to
- Allow adding clusters if needed even when computation is going

## Work to do

### Implementation

- [ ] Setup a working version of page rank algorithm between multiple `workers` run in same computer, using `edge partition` as it is proved to be than `node partition`
  - [ ] edge partition 
  - [x] page rank algo (https://moocs.iniad.org/courses/2020/CS116/2020_08/10-03 - Page 16)
  - [x] Communication between `node workers` (using c socket?) [Currently working on]
  - [x] Connect with redis as place to store data on ram
  - [ ] Implement some ultilize method mentioned from paper (optional later)
    - [ ] Ghost node
    - [ ] Merge multiple request into a bigger one to save internet bandwidth
  - [ ] Allow multiple thread in 1 `worker`: [INIAD C Multiple Thread](https://moocs.iniad.org/courses/2020/CS113/Week-02/Attendance%20for%20Week%202)
- [ ] Allow exchange node between `workers`
- [ ] Allow new node join the work

### Ec2 Setup for testing
- [ ] Dockerization and script to able to setup to EC2 in one command
- [ ] Read doc to findout how to use `EC2 on demand` service: [link](https://aws.amazon.com/ec2/pricing/on-demand/)

## General system architecture

We will probably have 3 main path in each each `worker`:
- `Processor`: doing the computing thing. Depend on `Data reader` for data
- `Data reader`: In charge of getting data for `Processor`
  - reading data from local machine if exist (Redis with Mongo DB?)
  - If data not exist in local machine, ask `Communicator` for data.
- `Communicator`: In charge of communicate with other `workers`
  - Answer request from `Data reader`
  - If asked for local data by another `worker`, ask `Data reader` for data

### Processor

- Setup.
- Split into thread
  - Each thread in charge of 1 node, and will be created/remove gradually // Each thead in charge of multiple pre-determinded node


## Problems
### Current facing problems
- How to store data as a whole in 1 machine? (using redis and mongo?)
- Is using c will be too hard to implement later? (probably is, considering using Python instead. But still, currenly, the biggest problems is it is very hard to using object in C. But the data a node have is not that complicate)
  - C library:
    -  [x] redis: hiredis
    -  [ ] mongo: check [link](https://github.com/mongodb/mongo-c-driver)
- 

### Probably facing later problems

- It is impossible to load the whole thing in ram. But then which `node` should stay in ram, which should be in hard drive?


todo: 
+ how many time access redis
+ how much data is sent via network
+ volumn computing reduce after adding a node
+ 