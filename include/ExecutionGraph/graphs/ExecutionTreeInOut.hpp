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
        NodePointer m_node         = nullptr;
        NodeClassification m_class = NodeClassification::NormalNode;
        IndexType m_priority       = 0;     //! The priority of this node

        IndexType m_subGraphIdx    = -1;    //! The subgraph index

        enum TraversalFlags : int { Visited = 1 , Marked = 2};
        int m_flags                = 0;     //! Some flags for graph traversal
        bool getFlag(TraversalFlags position){ return m_flags & (1<<position);}
        void setFlag(TraversalFlags position){ m_flags |= (1<<position);}
        void unsetFlag(TraversalFlags position) { m_flags &= ~(1<<position);}
    };

    //! A subgraph data structure to track the priority offset while performing a topological sort.
    struct SubGraphData
    {
        SubGraphData(NodeData* root) : m_root(root), m_index(root->m_node->getId()) {}
        NodeData* m_root = nullptr;
        const IndexType m_index;
        IndexType m_priorityOffset = 0;
    };

    using NodeDataStorage = std::unordered_map<NodeId, NodeData>;  // Rehashing does not invalidate pointers or references to elements.

    using NodeDataList       = std::vector<NodeData*>;
    using NodeDataSet        = std::unordered_set<NodeData*>;
    using GroupNodeMap       = std::unordered_map<GroupId, NodeDataSet>;
    using GroupExecutionList = std::unordered_map<GroupId, NodeDataList>;

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
        return it->m_node.get();
    }

    //! Get all nodes classified as \p type.
    const NodeDataSet& getNodes(NodeClassification type) const { return m_nodeClasses[type]; }

    //! Get all nodes in the group with id \p groupId.
    const NodeDataSet& getNodes(GroupId groupId) const
    {
        auto it = m_nodeGroups.find(groupId);
        EXEC_GRAPH_THROWEXCEPTION_IF(it == m_nodeGroups.end(), "Group with id: " << groupId << " is not part of tree!");
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
        // Reset in no order!
        for (auto* nodeData : m_nodeGroups[groupId])
        {
            nodeData->m_node->reset();
        }
    }

    //! Execute all nodes in group with id: \p groupId.
    virtual void execute(unsigned int groupId)
    {
        // Execute in determined order!
        for (auto& nodeData : m_groupExecList[groupId])
        {
            nodeData->m_node->compute();
        }
    }

    //    /** Finalize group */
    //    virtual void finalize(unsigned int groupId) {
    //        for(auto & n : m_groupExecList[groupId]) {
    //            // Execute in determined order!
    //            n->compute();
    //        }
    //    }

    //! Execute all groups.
    virtual void executeAll()
    {
        for (auto& group : m_groupExecList)
        {
            for (auto* nodeData : group.second)
            {
                nodeData->m_node->compute();
            }
        }
    }

    /** Reset all groups */
    virtual void resetAll()
    {
        for (auto& group : m_groupExecList)
        {
            for (auto* nodeData : group.second)
            {
                nodeData->m_node->reset();
            }
        }
    }

    virtual void setup()
    {
        if (m_nodeClasses[NodeClassification::OutputNode].size() == 0)
        {
            EXEC_GRAPH_THROWEXCEPTION("No output nodes specified!");
        }

        // Solve execution order for every group
        // Each group has its own execution order!
        ExecutionOrderSolver solver(m_nodeMap);
        m_groupExecList.clear();

        IndexType maxPrio = 0;
        for (auto& p : m_nodeGroups)
        {
            // Get the nodeDatas
            NodeDataSet& nodeDatas = p.second;
            // Get the eventually sorted nodeDatas
            NodeDataList& sortedNodeDatas = m_groupExecList[p.first];

            // Sort nodes topologically
            solver.solve(nodeDatas, sortedNodeDatas);

            // Save max. priority
            maxPrio = std::max(maxPrio, sortedNodeDatas.back()->m_priority);
        }

        // Do some manicure: invert all priorities such that lowest is now the
        // highest
        for (auto& p : m_groupExecList)
        {
            for (auto* nodeData : p.second)
            {
                nodeData->m_priority = maxPrio - nodeData->m_priority;
            }
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
            for (auto* nodeData : g.second)
            {
                auto* n = nodeData->m_node.get();
                s << suffix
                  << stringFormat(fmt, n->getName().c_str(),  n->getId(), nodeData->m_priority, n->getTypeName().c_str())
                  << std::endl;
            }
            s << suffix << stringFormat(fmtH, "---------------","------", "--------", "--------") << std::endl;
        }
        return s.str();
    }

protected:
    class ExecutionOrderSolver
    {
    public:
        ExecutionOrderSolver(NodeDataStorage& nodeMap)
            : m_nodeMap(nodeMap)
        {
        }

        void solve(NodeDataSet nodes, NodeDataList& orderedNodes)
        {
            m_subGraphData.clear();
            orderedNodes.clear();
            NodeData* visitingNode = nullptr;

            // Loop over all nodes and start a depth-first-search
            for(NodeData* nodeData: nodes)
            {
                if(!nodeData->getFlag(NodeData::Visited))
                {
                    // Start a depth-first recursion from this node (exploring this subtree)
                    m_dfsStack.clear();

                    nodeData->unsetFlag(NodeData::Marked);
                    m_dfsStack.push_back(nodeData);

                    // Allocate a subgraph data for this DFS recursion.
                    m_subGraphData.emplace_back(nodeData);
                    nodeData->m_subGraphIdx = m_subGraphData.back().m_index;

                    bool backTracking = false;
                    NodeData* currentNode;
                    while(!m_dfsStack.empty())
                    {
                        currentNode = m_dfsStack.back();

                        std::cout << getTraversalInfo(false) << std::endl;

                        // Check if node is already marked
                        if(currentNode->getFlag(NodeData::Marked))
                        {

                            if(backTracking)
                            {
                                // unmark, pop and continue
                                currentNode->unsetFlag(NodeData::Marked);
                                m_dfsStack.pop_back();
                                continue;
                            }
                            else
                            {
                                // We are doing depth first search in the subtree,
                                // and found another already marked node -> meaning
                                // we have a cycle.
                                EXEC_GRAPH_THROWEXCEPTION("Your execution logic graph contains a cylce! "
                                                          "Current traversal stack: "
                                                          << getTraversalInfo());
                            }
                        }
                        else
                        {
                            // Reset backtracking
                            if(backTracking)
                            {
                                backTracking = false;
                            }

                            // Mark this node:
                            currentNode->setFlag(NodeData::Marked);
                        }

                        visit(*currentNode); // Visits neighbors and adds them to m_dfsStack (if not visited)

                        // If no nodes have been added (we are at a leave -> do backtracking)
                        if(currentNode == m_dfsStack.back())
                        {
                            backTracking = true;
                        }

                        // Set node as visited
                        currentNode->setFlag(NodeData::Visited);
                    }
                }
            }

            // Loop over all nodes and set the final priority:
            for(NodeData* nodeData: nodes)
            {
                nodeData->m_priority = getPriority(*nodeData);
                orderedNodes.emplace_back(nodeData);
            }

            // Sort all nodes according to priority (asscending) (lowest is most
            // important)
            std::sort(orderedNodes.begin(), orderedNodes.end(), [](auto* const& a, auto* const& b) {
                return a->m_priority < b->m_priority;
            });
        }

    private:

        //! Return current DFS stack, but only nodes which are marked
        std::string getTraversalInfo(bool onlyMarked = true)
        {
            std::stringstream ss;
            auto it = m_dfsStack.begin();
            if(it != m_dfsStack.end() && (onlyMarked && (*it)->getFlag(NodeData::Marked) || !onlyMarked))
            {
                ss << (*it++)->m_node->getId();
            }
            for(;it != m_dfsStack.end(); ++it)
            {
                if((onlyMarked && (*it)->getFlag(NodeData::Marked) || !onlyMarked))
                {
                    ss << " ---> " << (*it)->m_node->getId();
                }
            }
            return ss.str();
        }

        /**
         * Depth first search: this function returns recursively the priority
         * lowest number has the highest priority
         */

        void visit(NodeData& nodeData)
        {


            auto addToStack = [&](auto* socket){
                // Get NodeData of adjNode
                auto& adjNode = socket->getParent();
                auto itAdj = m_nodeMap.find(adjNode.getId());
                EXEC_GRAPH_THROWEXCEPTION_IF(itAdj == m_nodeMap.end(),
                                             "Node with id: " << adjNode.getId() << " has not been added to the execution tree!");
                NodeData* adjNodeData = &itAdj->second;

                // If we have not already visited this node, add to stack
                if(!adjNodeData->getFlag(NodeData::Visited))
                {
                    adjNodeData->m_priority = nodeData.m_priority + 1;  // Increment local priority
                    adjNodeData->m_subGraphIdx = nodeData.m_subGraphIdx; // Adjacent node has same subgraph idx!
                    m_dfsStack.push_back(adjNodeData);                   // Add to stack
                }
                else
                {
                    EXEC_GRAPH_THROWEXCEPTION_IF(adjNodeData->m_subGraphIdx == -1,"Error: Node with id: "
                                                 << adjNodeData->m_node->getId() << " is visited and should have a subgraph index!");

                    // Compare the global priority (local priority + subgraph offset)
                    if(getPriority(*adjNodeData) <= getPriority(nodeData))
                    {
                       // adjNodeData needs a higher priority, so increment its subgraph priority offset
                       m_subGraphData[adjNodeData->m_subGraphIdx].m_priorityOffset += 1;
                    }
                }
            };

            // Follow all links
            auto& inSockets = nodeData.m_node->getInputs();
            for (auto& socket : inSockets)
            {
                // Try adding the get link to the stack
                if (socket->hasGetLink())
                {
                    addToStack(socket->getGetLink());
                }
                // Try adding all writing links to the stack
                for(auto* outputSocket: socket->getWritingParents())
                {
                    addToStack(outputSocket);
                }
            }
        }

        IndexType getPriority(NodeData& nodeData)
        {
            if(nodeData.m_subGraphIdx != -1)
            {
                return nodeData.m_priority + m_subGraphData[nodeData.m_subGraphIdx].m_priorityOffset;
            }
            return nodeData.m_priority;
        }

        bool m_inputReachable     = false;
        NodeBaseType* m_reachNode = nullptr;
        NodeDataStorage& m_nodeMap;  //! All NodeDatas of the execution tree.

        std::deque<NodeData*> m_dfsStack; //! Depth-First-Search Stack
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
                    auto* outputSocket = socket->getGetLink();
                    auto& adjNode      = outputSocket->getParent();
                    // If we reached the start node, return!
                    if (m_start == &adjNode)
                    {
                        m_reached = true;
                        return;
                    }

                    queu.push_back(&adjNode);
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

#endif  // ExecutionTreeInOut_hpp
