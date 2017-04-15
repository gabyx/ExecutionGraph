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
#include <unordered_set>

#include "ExecutionGraph/common/Asserts.hpp"
#include "ExecutionGraph/common/DemangleTypes.hpp"

#include "ExecutionGraph/nodes/LogicNode.hpp"
#include "ExecutionGraph/nodes/LogicSocket.hpp"

template<typename TConfig>
class ExecutionTreeInOut
{
public:
    EXEC_GRAPH_TYPEDEF_CONFIG(TConfig);

    enum class NodeClassification : unsigned char {
        NormalNode = 0,
        InputNode  = 1,
        OutputNode = 2
    };

    using GroupId = unsigned int;

private:
    static const std::underlying_type_t<NodeClassification> nNodeClasses = 3;

    using NodePointer = std::unique_ptr<NodeBaseType>;
    struct NodeData{

        std::size_t getPriority(){ return m_priority;}
        void setPriority(std::size_t p){ m_priority = p;}

        private:
        NodePointer m_node          = nullptr;
        NodeClassification  m_class = NodeClassification::NormalNode;
        std::size_t m_priority      = std::numeric_limits<std::size_t>::max();
    };

    using NodeStorage        = std::unordered_map<NodeId, NodeData>; // Rehashing does not invalidate pointers or references to elements.

    using NodeList           = std::vector<NodeData*>;
    using NodeSet            = std::unordered_set<NodeData*>;
    using GroupNodeMap       = std::unordered_map<GroupId, NodeSet>;
    using GroupExecutionList = std::unordered_map<GroupId, NodeList>;

public:
    ExecutionTreeInOut() = default;
    ~ExecutionTreeInOut() = default;

    void setNodeClass(NodeId id, NodeClassification newType)
    {
        m_executionOrderUpToDate = false;

        auto it = m_nodeMap.find(id);
        EXEC_GRAPH_THROWEXCEPTION_IF(it == m_nodeMap.end(), "Node with id: " << id << " does not exist in tree!");

        NodeClassification& currType = it->second.second;
        if(currType == newType)
        {
            return;
        }

        NodeBaseType* node = it->second.first;

        m_nodeClasses[currType].remove(node);   // Remove from class.
        currType = newType;                     // Set new classification
        m_nodeClasses[currType].add(node);      // Add to class.

    }

    //! Get a specific node with id \p id.
    NodeBaseType& getNode(NodeId id)
    {
        return *m_nodeMap[id].m_node;
    }

    //! Get all nodes classified as \p type.
    const NodeSet& getNodes(NodeClassification type) const { return m_nodeClasses[type]; }

    //! Get all nodes in the group with id \p groupId.
    const NodeSet& getNodes(GroupId groupId) const
    {
        auto it = m_nodeGroups.find(groupId);
        EXEC_GRAPH_THROWEXCEPTION_IF(it == m_nodeGroups.end(),"Group with id: " << groupId << " is not part of tree!");
        return it->second;
    }

    //! Adds a node to the execution tree and classifies it as \p type.
    virtual void addNode(NodePointer node,
                         NodeClassification type = NodeClassification::NormalNode)
    {
        m_executionOrderUpToDate = false;

        if(node==nullptr)
        {
            EXEC_GRAPH_ASSERTMSG(false,"Nullptr added!")
            return;
        }
        auto id = node->getId();

        // If we throw here, the node managed by the unique pointer is deleted.
        EXEC_GRAPH_THROWEXCEPTION_IF( m_nodeMap.find(id) != m_nodeMap.end(),
                                     "Node id: " << node->getId() << " already added in tree!");

        // Add to group
        m_nodeClasses[type].emplace(node.get());
        // Add to storage map
        m_nodeMap.emplace(id, {std::move(node),type} );
    }

    //! Assigns the node with id \p nodeId to the group with id \p groupId.
    virtual void addNodeToGroup(NodeId nodeId, GroupId groupId)
    {
        m_executionOrderUpToDate = false;

        auto it = m_nodeMap.find(nodeId);
        EXEC_GRAPH_THROWEXCEPTION_IF(it == m_nodeMap.end(), "Node with id: " << nodeId << " does not exist in tree!");
        // Add node to the group
        auto res = m_nodeGroups[groupId].emplace(it->m_node.get());
        EXEC_GRAPH_THROWEXCEPTION_IF(!res.second, "Node with id: " << nodeId << " does not exist in tree!");
    }

    //! Constructs a Get-Link to get the data from output socket at index \p outS
    //! of logic node \p outN at the input socket at index \p inS.
    virtual void makeGetLink(NodeId outN, SocketId outS, NodeId inN, SocketId inS)
    {
        auto outNit = m_nodeMap.find(outN);
        auto inNit  = m_nodeMap.find(inN);
        EXEC_GRAPH_THROWEXCEPTION_IF( outNit == m_nodeMap.end() || inNit == m_nodeMap.end() ,
                                     "Node: " << outN << " or " << inN << " does not exist!")
        NodeBaseType::makeGetLink(*outNit->m_node, outS, *inNit->m_node, inS);
    }

    //! Constructs a Write-Link to write the data of output socket at index \p
    //! outS of logic node \p outN to the input socket at index \p inS of logic node \p
    //! inN.
    virtual void makeWriteLink(unsigned int outN, unsigned int outS, unsigned int inN, unsigned int inS)
    {
        auto outNit = m_nodeMap.find(outN);
        auto inNit  = m_nodeMap.find(inN);
        if (outNit == m_nodeMap.end() || inNit == m_nodeMap.end())
        {
            EXEC_GRAPH_ERRORMSG("Node: " << outN << " or " << inN << " does not exist!")
        }
        NodeBaseType::makeWriteLink(*outNit->m_node, outS, *inNit->m_node, inS);
    }

    //! Reset all nodes in group with id: \p groupId.
    virtual void reset(unsigned int groupId)
    {
        // Reset in no order!
        for (auto& n : m_nodeGroups[groupId])
        {
            n->reset();
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
            EXEC_GRAPH_ERRORMSG("No output nodes specified!")
        }

        // Solve execution order for every group list!
        // Each group has its own execution order!
        ExecutionOrderSolver s;
        m_groupExecList.clear();
        std::size_t maxPrio = 0;
        for (auto& p : m_nodeGroups)
        {
            // fill nodes into execution list
            auto& l = m_groupExecList[p.first];
            std::for_each(
                p.second.begin(), p.second.end(), [&l](const auto* nodeData) { l.push_back(nodeData); });

            s.solve(l, l);

            maxPrio = std::max(maxPrio, l.back()->getPriority());
        }

        // Do some manicure: invert all priorities such that lowest is now the
        // highest
        for (auto& p : m_groupExecList)
        {
            for (auto* nodeData : p.second)
            {
                nodeData->setPriority(maxPrio - nodeData->getPriority());
            }
        }

        // Check if input is reachable from all outputs
        ReachNodeCheck c;
        for (auto* outNode : m_nodeClasses[NodeClassification::OutputNode])
        {
            bool outputReachedInput = false;
            // each outputnode should reach at least one input, if not print warning!
            for (auto* inNode : m_nodeClasses[NodeClassification::InputNode])
            {
                if (c.check(outNode, inNode))
                {
                    outputReachedInput = true;
                    break;
                }
            }
            EXEC_GRAPH_WARNINGMSG(outputReachedInput, "WARNING: Output id: " << outNode->m_id << " did not reach any input!")
        }
    }

    std::string getExecutionOrderInfo(std::string suffix = "\t\t")
    {
        // Print execution order
        std::stringstream s;
        for (auto& g : m_groupExecList)
        {
            s << "Execution order for group id: " << g.first << std::endl;
            s << suffix << "NodeId\t|\tPriority\t|\tType" << std::endl;
            for (auto* nodeData : g.second)
            {
                auto* n = nodeData->m_node;
                s << suffix
                  << Utilities::stringFormat("%4i \t|\t %4i \t|\t %s", n->m_id, n->getPriority(), demangle::type(n))
                  << std::endl;
            }
            s << suffix << "==============================" << std::endl;
        }
        return s.str();
    }

protected:
    class ExecutionOrderSolver
    {
    public:
        void solve(NodeList& inputNodes, NodeList& orderedNodes)
        {
            // Solve Execution order,
            // start a depth first search recursion for all nodes in c which
            // determines an execution order by setting
            // the priority
            for (auto* nodeData : inputNodes)
            {
                std::unordered_set<unsigned int> nodesCurrDepth;
                // start recursion from this node
                solveRec(nodeData->m_node, nodesCurrDepth);
            }

            // Sort all nodes according to priority (asscending) (lowest is most
            // important)
            std::sort(orderedNodes.begin(), orderedNodes.end(), [](auto* const& a, auto* const& b) {
                return a->getPriority() < b->getPriority();
            });
        }

    private:
        /**
         * Depth first search: this function returns recursively the priority
         * lowest number has the highest priority
         */

        unsigned int solveRec(NodeBaseType* node, std::unordered_set<unsigned int>& nodesCurrDepth)
        {
            nodesCurrDepth.insert(node->m_id);

            // visit all input sockets and their node!
            auto& inSockets = node->getInputs();
            for (auto* s : inSockets)
            {
                if (s->isLinked())
                {
                    auto* fsock = s->getFrom();

                    auto* adjNode = fsock->getParent();
                    if (nodesCurrDepth.find(adjNode->m_id) != nodesCurrDepth.end())
                    {
                        EXEC_GRAPH_ERRORMSG("Your execution logic graph contains a cylce from node: " << node->m_id
                                            << " socket: " << s->m_id << " to node: " << adjNode->m_id << " socket: "
                                            << fsock->m_id);
                    }

                    unsigned int prioAdj = solveRec(adjNode, nodesCurrDepth);

                    // Set the current node to the priority of the below tree +1
                    node->setPriority(std::max(prioAdj + 1, node->getPriority()));
                }
            }

            nodesCurrDepth.erase(node->m_id);

            return node->getPriority();
        }
        bool m_inputReachable  = false;
        NodeBaseType* m_reachNode = nullptr;
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
            for (auto* s : inSockets)
            {
                if (s->isLinked())
                {
                    auto* fsock = s->getFrom();
                    EXEC_GRAPH_ASSERTMSG(fsock, "linked but from ptr null");

                    auto* adjNode = fsock->getParent();
                    EXEC_GRAPH_ASSERTMSG(adjNode, "Adj node null");

                    // If we reached the start node, return!
                    if (m_start == adjNode)
                    {
                        m_reached = true;
                        return;
                    }

                    queu.push_back(adjNode);
                }
            }
        }

        bool m_reached     = false;
        NodeBaseType* m_start = nullptr;
    };


    void setupNode(NodeBaseType& node, NodeClassification type)
    {
        if (type == NodeClassification::InputNode)
        {
            m_inputNodes.emplace(&node);
        }
        else if (type == NodeClassification::OutputNode)
        {
            m_outputNodes.emplace(&node);
        }
    }

    std::set<NodeBaseType*> m_nodeClasses[nNodeClasses]; ///< the input/ouput and normal nodes

    NodeStorage m_nodeMap;  ///< all nodes in the tree

    // Additional group list
    GroupNodeMap m_nodeGroups;

    GroupExecutionList m_groupExecList;

    bool m_executionOrderUpToDate = false;
};

#endif  // ExecutionTreeInOut_hpp
