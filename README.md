# ExecutionGraph
Fast Execution Graph consisting of Execution Nodes


Be able to design and run such input/output graphs, such as this one used in [http://gabyx.github.io/GRSFramework/#videos] :
![Stuff like this](https://cdn.rawgit.com/gabyx/GRSFramework/b1414aa0/simulations/examples/jobs/simulationStudies/avalanche1M-Tree-SimStudy/analyzeStartJob/analyzerLogic/FindStart.svg)

The execution graph implemented in `ExecutionTree` is a directed acyclic graph consisting of several connected logic nodes derived from `LogicNode` which define a simple input/output control flow.
Each logic node in the execution graph contains several input/output sockets (`LogicSocket`) with a certain type out of the predefined types defined in `LogicSocketTypes`. 
An execution graph works in the way that each logic node contains a specific `compute` routine which provides values for the output sockets by using the values from the input sockets. 
Each output of a logic node can be linked to an input of the same type of another logic node. This means an output socket of the arithmetic type `double` cannot be linked to an input socket of integral type `int` for example. 
Each logic node can be assigned to one or more execution groups which are collections of logic nodes and form directed acyclic subgraphs. 
For each execution group, an execution order is computed such that the data flow defined by the input/output links in the group is respected.
An execution order of an execution group is called a *topological ordering* in computer science, and such an ordering always exists for a directed acyclic graph, despite being non-unique. A topological ordering of an execution group is an ordering of all logic nodes such that for all connections from a logic node `A` to `B`, `A` precedes `B` in the ordering. Each execution graph network consists of several input logic nodes whose output sockets are initialized before executing the network. 
The implementation in `LogicSocket` allows two types of directed links between an input and output socket, namely a *get* and a *write* connection. 
A *write* link is a link from an output socket `i` of a node `A` to an input socket `j` of some node `B`, denoted as `{A,i} -> {j,B}`.
A *write* link basically duplicates a write request to the output socket `i` of `A` also to an additional write request to the input socket `j` of `B`.
A *get* link is the exact opposite and is a link from an input socket `j` of a node `B` to an output socket `i` of a node `A`, denoted as 
`{A,i} <- {j,B}`. 
A *get* link basically forwards any read access on the input socket `j` of `B` to a read access on the input socket `i` of `A`.
Most of the time only *get* nodes are necessary but as soon as the execution graph becomes more complex and certain switching behavior should be reproduced, the additional *write* links are a convenient tool to realize this. 
Cyclic paths between logic nodes are detected and result in an error when building the execution network.
The write and read access of input and output sockets is implemented using a fast static type dispatch system in `LogicSocket`.
Static type dispatching avoids the use of virtual calls when using polymorphic objects in object-oriented programming languages.



More to come!
