// ========================================================================================
//  ExecutionGraph
//  Copyright (C) 2014 by Gabriel Nützi <gnuetzi (at) gmail (døt) com>
//
//  This Source Code Form is subject to the terms of the Mozilla Public
//  License, v. 2.0. If a copy of the MPL was not distributed with this
//  file, You can obtain one at http://mozilla.org/MPL/2.0/.
// ========================================================================================

#ifndef ExecutionGraph_graphs_ExecutionTreeInOut_hpp
#define ExecutionGraph_graphs_ExecutionTreeInOut_hpp

#include <algorithm>
#include <deque>
#include <unordered_set>

#include "ExecutionGraph/common/Asserts.hpp"
#include "ExecutionGraph/common/DemangleTypes.hpp"
#include "ExecutionGraph/common/StringFormat.hpp"
#include "ExecutionGraph/nodes/LogicCommon.hpp"
#include "ExecutionGraph/nodes/LogicNode.hpp"
#include "ExecutionGraph/nodes/LogicSocket.hpp"

#define EXEC_GRAPH_EXECTREE_SOLVER_LOG( message ) EXEC_GRAPH_DEBUG_ONLY(std::cout << message);
#define EXEC_GRAPH_EXECTREE_SOLVER_LOG_ON( message ) std::cout << message ;


namespace executionGraph
{
template<typename TConfig>
class ExecutionTreeInOut
{
public:
    EXEC_GRAPH_TYPEDEF_CONFIG(TConfig);

    enum NodeClassification : unsigned char
    {
        NormalNode = 0,
        InputNode  = 1,
        OutputNode = 2
    };

    using GroupId = unsigned int;

private:
    static const std::underlying_type_t<NodeClassification> nNodeClasses = 3;

    //! Internal Datastructure to store node related data.
    using NodePointer = std::unique_ptr<NodeBaseType>;
    struct NodeData
    {
        NodeData(NodePointer node, NodeClassification c)
            : m_node(std::move(node)), m_class(c)
        {}

        NodePointer m_node         = nullptr;
        NodeClassification m_class = NodeClassification::NormalNode;
        IndexType m_priority       = 0;     //! The priority of this node

        IndexType m_subGraphIdx    = -1;    //! The subgraph inde
        bool isInSubGraph(){ return m_subGraphIdx != -1; }

        enum TraversalFlags : int {};
        bool isFlagSet(TraversalFlags position){ return m_flags & (1<<position);}
        void setFlag(TraversalFlags position){ m_flags |= (1<<position);}
        void unsetFlag(TraversalFlags position) { m_flags &= ~(1<<position);}

        private:
            int m_flags = 0;     //! Some flags for graph traversal
    };

    //! A subgraph data structure to track the priority offset while performing a topological sort.
    struct SubGraphData
    {
        SubGraphData(NodeData* root) : m_root(root) {}
        NodeData* m_root = nullptr;
    };

    using NodeDataStorage    = std::unordered_map<NodeId, NodeData>;  // Rehashing does not invalidate pointers or references to elements.

    using NodeDataList       = std::vector<NodeData*>;
    using NodeDataSet        = std::unordered_set<NodeData*>;
    using GroupNodeMap       = std::unordered_map<GroupId, NodeDataSet>;

    using PrioritySet        = std::map<IndexType,NodeDataList,std::greater<IndexType>>;
    using GroupExecutionList = std::unordered_map<GroupId, PrioritySet>;

public:
    ExecutionTreeInOut()          = default;
    virtual ~ExecutionTreeInOut() = default;

    void setNodeClass(NodeId id, NodeClassification newType)
    {
        m_executionOrderUpToDate = false;

        auto it = m_nodeMap.find(id);
        EXEC_GRAPH_THROWEXCEPTION_IF(it == m_nodeMap.end(), "Node with id: " << id << " does not exist in tree!");

        NodeClassification& currType = it->second.m_class;
        if (currType == newType)
        {
            return;
        }

        NodeBaseType* node = it->second.m_node.get();

        m_nodeClasses[currType].erase(node);    // Remove from class.
        currType = newType;                     // Set new classification
        m_nodeClasses[currType].emplace(node);  // Add to class.
    }

    void setNodeClass(NodeBaseType& node, NodeClassification newType)
    {
        setNodeClass(node.getId(), newType);
    }

    //! Get a specific node with id \p id.
    NodeBaseType* getNode(NodeId id)
    {
        auto it = m_nodeMap.find(id);
        if (it == m_nodeMap.end())
        {
            return nullptr;
        }
        return it->second.m_node.get();
    }

    //! Get all nodes classified as \p type.
    const NodeDataSet& getNodes(NodeClassification type) const { return m_nodeClasses[type]; }

    //! Get all nodes in the group with id \p groupId.
    const NodeDataSet& getNodes(GroupId groupId) const
    {
        auto it = m_nodeGroups.find(groupId);
        EXEC_GRAPH_THROWEXCEPTION_IF(it == m_nodeGroups.end(),
                                     "Group with id: " << groupId << " is not part of tree!");
        return it->second;
    }

    //! Adds a node to the execution tree and classifies it as \p type.
    virtual void addNode(NodePointer node,
                         NodeClassification type = NodeClassification::NormalNode,
                         GroupId groupId         = 0)
    {
        m_executionOrderUpToDate = false;

        if (node == nullptr)
        {
            EXEC_GRAPH_ASSERT(false, "Nullptr added!")
            return;
        }
        auto id = node->getId();

        // If we throw here, the node managed by the unique pointer is deleted.
        EXEC_GRAPH_THROWEXCEPTION_IF(m_nodeMap.find(id) != m_nodeMap.end(),
                                     "Node id: " << node->getId() << " already added in tree!");

        // Add to group
        m_nodeClasses[type].emplace(node.get());
        // Add to storage map
        m_nodeMap.emplace(id, NodeData{std::move(node), type});
        // Add node to group
        addNodeToGroup(id, groupId);
    }

    //! Assigns the node with id \p nodeId to the group with id \p groupId.
    virtual void addNodeToGroup(NodeId nodeId, GroupId groupId)
    {
        m_executionOrderUpToDate = false;

        auto it = m_nodeMap.find(nodeId);
        EXEC_GRAPH_THROWEXCEPTION_IF(it == m_nodeMap.end(), "Node with id: " << nodeId << " does not exist in tree!");
        // Add node to the group
        auto res = m_nodeGroups[groupId].emplace(&it->second);
        EXEC_GRAPH_THROWEXCEPTION_IF(!res.second, "Node with id: " << nodeId << " does not exist in tree!");
    }

    //! Constructs a Get-Link to get the data from output socket at index \p outS
    //! of logic node \p outN at the input socket at index \p inS.
    virtual void makeGetLink(NodeId outN, SocketIndex outS, NodeId inN, SocketIndex inS)
    {
        auto outNit = m_nodeMap.find(outN);
        auto inNit  = m_nodeMap.find(inN);
        EXEC_GRAPH_THROWEXCEPTION_IF(outNit == m_nodeMap.end() || inNit == m_nodeMap.end(),
                                     "Node: " << outN << " or " << inN << " does not exist!")
        NodeBaseType::setGetLink(*outNit->second.m_node, outS, *inNit->second.m_node, inS);
    }

    //! Constructs a Write-Link to write the data of output socket at index \p
    //! outS of logic node \p outN to the input socket at index \p inS of logic node \p
    //! inN.
    virtual void makeWriteLink(NodeId outN, SocketIndex outS, NodeId inN, SocketIndex inS)
    {
        auto outNit = m_nodeMap.find(outN);
        auto inNit  = m_nodeMap.find(inN);
        if (outNit == m_nodeMap.end() || inNit == m_nodeMap.end())
        {
            EXEC_GRAPH_THROWEXCEPTION("Node: " << outN << " or " << inN << " does not exist!");
        }
        NodeBaseType::addWriteLink(*outNit->second.m_node, outS, *inNit->second.m_node, inS);
    }

    //! Reset all nodes in group with id: \p groupId.
    virtual void reset(unsigned int groupId)
    {
        // Execute in determined order!
        auto it = m_groupExecList.find(groupId);
        EXEC_GRAPH_THROWEXCEPTION_IF(it == m_groupExecList.end(),
                                     "ExecutionTree does not contain a group with id: " << groupId);
        executePrioritySet(it->second, [](NodeBaseType& node){node.reset();} );
    }

    /** Reset all groups */
    virtual void resetAll()
    {
        for(auto& group : m_groupExecList)
        {
            executePrioritySet(group.second, [](NodeBaseType& node){node.reset();} );
        }
    }

    //! Execute all nodes in group with id: \p groupId in their determined order.
    virtual void execute(unsigned int groupId)
    {
        // Execute in determined order!
        auto it = m_groupExecList.find(groupId);
        EXEC_GRAPH_THROWEXCEPTION_IF(it == m_groupExecList.end(),
                                     "ExecutionTree does not contain a group with id: " << groupId);
        executePrioritySet(it->second, [](NodeBaseType& node){node.compute();} );
    }

    //! Execute all groups.
    virtual void executeAll()
    {
        EXEC_GRAPH_THROWEXCEPTION_IF(m_groupExecList.empty(),"ExecutionTree has not been setup!")
        for(auto& group : m_groupExecList)
        {
            executePrioritySet(group.second, [](NodeBaseType& node){node.compute();} );
        }
    }

    //    /** Finalize group */
    //    virtual void finalize(unsigned int groupId) {
    //        for(auto & n : m_groupExecList[groupId]) {
    //            // Execute in determined order!
    //            n->compute();
    //        }
    //    }

    virtual void setup(bool checkResults = false)
    {
        // Allways check results in Debug mode.
        EXEC_GRAPH_DEBUG_ONLY(checkResults = true;);

        if (m_nodeClasses[NodeClassification::OutputNode].size() == 0)
        {
            EXEC_GRAPH_THROWEXCEPTION("No output nodes specified!");
        }

        // Solve execution order for every group
        // Each group has its own execution order!
        ExecutionOrderSolver solver(m_nodeMap);
        m_groupExecList.clear();

        for (auto& p : m_nodeGroups)
        {
            // Get the nodeDatas
            NodeDataSet& nodeDatas = p.second;
            // Get the eventually sorted nodeDatas
            PrioritySet& sortedNodeDatas = m_groupExecList[p.first];

            // Sort nodes topologically
            solver.solve(nodeDatas, sortedNodeDatas);
        }
        // Check if each output node reaches at least one input, if not print warning!
        ReachNodeCheck c;
        for (auto* outNode : m_nodeClasses[NodeClassification::OutputNode])
        {
            bool outputReachedInput = false;

            for (auto* inNode : m_nodeClasses[NodeClassification::InputNode])
            {
                if (c.check(outNode, inNode))
                {
                    outputReachedInput = true;
                    break;
                }
            }
            EXEC_GRAPH_WARNINGMSG(outputReachedInput, "WARNING: Output id: " << outNode->getId() << " did not reach any input!")
        }
    }

    std::string getExecutionOrderInfo(std::string suffix = "\t\t")
    {
        // Print execution order
        std::stringstream s;
        std::string fmtH = "%-15s  |  %-6s  | %-8s   |  %-20s";
        std::string fmt =  "%-15s  |  %-6i  | %-8i   |  %-20s";
        for (auto& g : m_groupExecList)
        {
            s << "Execution order for group id: " << g.first << std::endl;
            s << suffix << stringFormat(fmtH, "Name" ,"NodeId", "Priority", "NodeType") << std::endl;
            s << suffix << stringFormat(fmtH, "---------------","------", "--------", "--------") << std::endl;
            for (auto& p : g.second)
            {
                for(NodeData* nodeData : p.second)
                {
                    auto* n = nodeData->m_node.get();
                    s << suffix
                      << stringFormat(fmt, n->getName().c_str(),  n->getId(), nodeData->m_priority, n->getTypeName().c_str())
                      << std::endl;
                }
            }
            s << suffix << stringFormat(fmtH, "---------------","------", "--------", "--------") << std::endl;
        }
        return s.str();
    }

protected:

    //! Execute all nodes in PrioritySet.
    template<typename Functor>
    inline void executePrioritySet(PrioritySet & prioritySet, Functor&& func)
    {
        for(auto& p : prioritySet)
        {
            // Execute all nodes with this priority
            for(NodeData* nodeData : p.second)
            {
                func(*nodeData->m_node);
            }
        }
    }

    class ExecutionOrderSolver
    {
    public:
        ExecutionOrderSolver(NodeDataStorage& nodeMap)
            : m_nodeMap(nodeMap)
        {
        }

        /*! Solved the execution order for an input node set `nodes` and
            outputs the ordered execution list `orderedNodes`.

            This algorithm is based on https://en.wikipedia.org/wiki/Topological_sorting#Depth-first_search.
            We visit each node once during this algorithm.
            From each node `a` in `nodes` we start a depth-first recursion, basically exploring the whole subtree `S`
            of each node `a`. While exploring, we assign local priorities to each node in the subtree (root = `a`)
            , e.g. NodeData::m_priority. Each subtree we explore has a priority offset which makes it possible to increase
            all priorities of all nodes below a subtree. Such increases happen when we start a depth-first recursion
            from a new node `a` and we find a node `b` in its subgraph which has already been visited
            (meaning `b` is in a subgraph which is already explored) and `getPriority(b) > getPriority(a) == false`
            we optimaly need to increase all priorities of `b`'s subgraph. This would need another depth traversal.
            Instead we increase the priority offset (SubGraphData.m_priorityOffset) of the subgraph to
            which `b` is found before.
        */
        void solve(NodeDataSet nodes, PrioritySet& orderedNodes, bool checkResults = true)
        {
            m_subGraphData.clear();

            orderedNodes.clear();
            NodeData* visitingNode = nullptr;

            // Loop over all nodes and start a depth-first-search
            for(NodeData* nodeData: nodes)
            {
                // If the node is already in a subgraph, we know that topological ordering (priorities)
                // below him is correct, so skip this one
                if(nodeData->isInSubGraph())
                {
                    EXEC_GRAPH_EXECTREE_SOLVER_LOG("DFS Start: Node id: " << nodeData->m_node->getId()
                                                   << " in subGraph -> skip it." << std::endl;);
                    continue;
                }

                EXEC_GRAPH_EXECTREE_SOLVER_LOG("DFS Start: Node id: " << nodeData->m_node->getId() << std::endl;);

                // Start a depth-first recursion from this node (exploring this subtree)
                m_dfsStack.clear();
                m_visitedNodes.clear();
                auto visitEndIt = m_visitedNodes.end();
                // Insert root node
                m_dfsStack.push_back(nodeData);

                 // Allocate a subgraph data for this DFS recursion.
                m_subGraphData.emplace_back(nodeData);
                nodeData->m_subGraphIdx = m_subGraphData.size()-1; // set the subgraph index

                NodeData* currentNode;
                bool backTracking = false;
                bool visited = false;
                std::size_t currentSize;

                auto doBackTracking = [&](){
                    auto itStart = m_dfsStack.rbegin();
                    auto itEnd = m_dfsStack.rend();
                    auto it = itStart;
                    while(it != itEnd)
                    {
                        if(m_visitedNodes.find(*it) == visitEndIt)
                        {
                            // Not visited, backtracking finished!
                            // Remove the range [itStart,itLast) from the stack
                            break;
                        }
                        ++it;
                    }
                    EXEC_GRAPH_ASSERT(std::distance(itStart,it) <= m_dfsStack.size(),
                                      "Removing: " << std::distance(itStart,it) << " from " << m_dfsStack.size());
                    // Convert backward iterator to forward
                    m_dfsStack.erase(it.base(),itStart.base());
                };

                while(!m_dfsStack.empty())
                {
                    EXEC_GRAPH_EXECTREE_SOLVER_LOG("DFS Stack:" << getStackInfo() << (backTracking? "<--" : " ") << std::endl);

                    currentNode = m_dfsStack.back();

                    // We are doing depth first search and found another already visited node
                    // meaning we have a cycle.
                    EXEC_GRAPH_THROWEXCEPTION_IF(m_visitedNodes.find(currentNode) != visitEndIt,
                                                 "Your execution logic graph contains a cycle! "
                                                 "Current traversal stack: " << getTraversalInfo()
                                                 << "leads to next node: " << currentNode->m_node->getId());

                    currentSize = m_dfsStack.size();
                    visit(*currentNode); // Visits neighbors and add them to m_dfsStack

                    // If no nodes have been added, down traversal is finished, we do now backtracking
                    if(currentSize == m_dfsStack.size())
                    {
                        //doBackTracking(); // Removing all visited nodes up the stack
                        doBackTracking();
                    }
                }
            }

            // Clean up
            m_subGraphData.clear();

            // Loop over all nodes and make priority sets:
            for(NodeData* nodeData: nodes)
            {
                nodeData->m_subGraphIdx = -1;
                orderedNodes[nodeData->m_priority].emplace_back(nodeData);
            }

            // Check execution order by checking all inputs of all nodes
            if(checkResults)
            {
                this->checkResults(nodes);
            }


        }

    private:

        //! Return current DFS stack, but only nodes which are visited.
        std::string getTraversalInfo()
        {
            std::stringstream ss;
            auto it = m_dfsStack.begin();
            if(it != m_dfsStack.end() && m_visitedNodes.find(*it) != m_visitedNodes.end() )
            {
                ss << (*(it++))->m_node->getId();
            }
            for(;it != m_dfsStack.end(); ++it)
            {
                if(m_visitedNodes.find(*it) != m_visitedNodes.end() )
                {
                    ss << " ---> " << (*it)->m_node->getId();
                }
            }
            return ss.str();
        }

        //! Return current DFS stack, visited nodes are marked with "*".
        std::string getStackInfo()
        {
            std::stringstream ss;
            ss << "[ " ;
            for(auto* nodeData : m_dfsStack)
            {
                ss << nodeData->m_node->getId() << ((m_visitedNodes.find(nodeData) != m_visitedNodes.end())? "*" : " ") << ", ";
            }
            ss << " ]";
            return ss.str();
        }


        /**
         * This is the visit function during Depth-First-Search,
         * It adds nodes to the dfsStack if the priority is lower then the one of `nodeData`.
         */
        void visit(NodeData& nodeData)
        {

            EXEC_GRAPH_EXECTREE_SOLVER_LOG("visit: " << nodeData.m_node->getId() << std::endl;);

            auto addParentsToStack = [&](auto* socket){
                // Get NodeData of parentNode
                auto& parentNode = socket->getParent();
                auto itParent = m_nodeMap.find(parentNode.getId());
                EXEC_GRAPH_THROWEXCEPTION_IF(itParent == m_nodeMap.end(),
                                             "Node with id: " << parentNode.getId() << " has not been added to the execution tree!");
                NodeData* parentNodeData = &itParent->second;

                if(parentNodeData->m_priority <= nodeData.m_priority)
                {
                    // Parent needs a other priority (because its computation becomes before nodeData)

                    // Force this node to our SubGraph index
                    parentNodeData->m_subGraphIdx = nodeData.m_subGraphIdx;
                    // Increase parents priority by 1
                    parentNodeData->m_priority = nodeData.m_priority + 1;
                    m_dfsStack.push_back(parentNodeData); // Add to stack and explore its subgraph
                }
            };

            // Follow all links
            auto& inSockets = nodeData.m_node->getInputs();
            for (auto& socket : inSockets)
            {
                // Try adding the get link to the stack
                if (socket->hasGetLink())
                {
                    addParentsToStack(socket->followGetLink());
                }
                // Try adding all writing links to the stack
                for(auto* outputSocket: socket->getWritingSockets())
                {
                    addParentsToStack(outputSocket);
                }
            }

            // Mark this node as visited
            m_visitedNodes.emplace(&nodeData);
        }

        void checkResults(const NodeDataSet& nodes)
        {

            auto check = [&](auto* socket, NodeData* nodeDataWithLowerPrio){
                // Get NodeData of parentNode
                auto& parentNode = socket->getParent();
                auto itParent = m_nodeMap.find(parentNode.getId());
                EXEC_GRAPH_THROWEXCEPTION_IF(itParent == m_nodeMap.end(),
                                             "Node with id: " << parentNode.getId() << " has not been added to the execution tree!");
                NodeData* parentNodeData = &itParent->second;

                EXEC_GRAPH_THROWEXCEPTION_IF(parentNodeData->m_priority <= nodeDataWithLowerPrio->m_priority,
                                             "Parent node id: " << parentNodeData->m_node->getId() << "[prio: " << parentNodeData->m_priority << " ]"
                                             << "has not a higher priority as node id: " <<  nodeDataWithLowerPrio->m_node->getId() << " which is wrong!");
            };


            for(auto* nodeData :  nodes)
            {
                // Follow all links
                auto& inSockets = nodeData->m_node->getInputs();
                for (auto& socket : inSockets)
                {
                    // Try adding the get link to the stack
                    if (socket->hasGetLink())
                    {
                        check(socket->followGetLink(), nodeData);
                    }
                    // Try adding all writing links to the stack
                    for(auto* outputSocket: socket->getWritingSockets())
                    {
                        check(outputSocket, nodeData);
                    }
                }
            }
        }

        bool m_inputReachable     = false;
        NodeBaseType* m_reachNode = nullptr;
        NodeDataStorage& m_nodeMap;  //! All NodeDatas of the execution tree.

        std::deque<NodeData*> m_dfsStack; //! Depth-First-Search Stack
        NodeDataSet m_visitedNodes;       //! Tracking the visited nodes during DFS
        std::vector<SubGraphData> m_subGraphData; //! Storage for all subgraph data.

    };

    // Only for directed graphs, does not detect cycles -> endless loop!
    class ReachNodeCheck
    {
    public:
        // From end to start node
        bool check(NodeBaseType* endNode, NodeBaseType* startNode)
        {
            if (endNode == startNode)
            {
                return true;
            }

            m_start   = startNode;
            m_reached = false;

            std::deque<NodeBaseType*> currentNodes;  // Breath first search
            currentNodes.push_back(endNode);

            // visit current front node, as long as currentNode list is not empty or
            // start has not yet been found!
            while (currentNodes.size() != 0 && m_reached == false)
            {
                visit(currentNodes.front(), currentNodes);
                currentNodes.pop_front();
            }

            return m_reached;
        }

    private:
        /**
         * Breath first search:
         */
        void visit(NodeBaseType* node, std::deque<NodeBaseType*>& queu)
        {
            // visit all input sockets and their node!
            auto& inSockets = node->getInputs();
            for (auto& socket : inSockets)
            {
                if (socket->hasGetLink())
                {
                    auto* outputSocket = socket->followGetLink();
                    auto& parentNode      = outputSocket->getParent();
                    // If we reached the start node, return!
                    if (m_start == &parentNode)
                    {
                        m_reached = true;
                        return;
                    }

                    queu.push_back(&parentNode);
                }
            }
        }

        bool m_reached        = false;
        NodeBaseType* m_start = nullptr;
    };

    std::set<NodeBaseType*> m_nodeClasses[nNodeClasses];  ///< the input/ouput and normal nodes

    NodeDataStorage m_nodeMap;  ///< all nodes in the tree

    // Additional group list
    GroupNodeMap m_nodeGroups;

    GroupExecutionList m_groupExecList;

    bool m_executionOrderUpToDate = false;
};
}

#undef EXEC_GRAPH_EXECTREE_SOLVER_LOG
#endif  // ExecutionTreeInOut_hpp
