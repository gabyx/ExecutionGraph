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
private:

    enum class NodeClassification {
        InputNode  = 0,
        OutputNode = 1,
        NormalNode = 2
    };
    static const unsigned int nNodeClasses = 3;

    using NodeBasePointerType    = std::unique_ptr<NodeBaseType>
    using NodeListType           = std::vector<NodeBaseType*>;
    using NodeStorageType        = std::unordered_map<NodeIdType, std::pair<NodeBasePointerType,NodeClassification>;
    using NodeSetType            = std::unordered_set<NodeBaseType*>;
    using GroupNodeMapType       = std::unordered_map<NodeIdType, NodeSetType>;
    using GroupExecutionListType = std::unordered_map<NodeIdType, NodeListType>;
public:



    ExecutionTreeInOut() = default;
    ~ExecutionTreeInOut() = default;

    void setNodeClass(NodeIdType id, NodeClassification newType)
    {
        auto it = m_nodeMap.find(id);
        EXEC_GRAPH_THROWEXCEPTION_IF(it != m_nodeMap.end(), "Node with id: " << id << " does not exist in tree!")

        NodeClassification& currType = it->second.second;
        if(currType == type)
        {
            return;
        }

        NodeBaseType* node = it->second.first;

        m_nodes[currType].remove(node); // Remove
        currType = newType;             // Set new classifcatin
        m_nodes[currType].add(node);    // Add

    }

    const NodeSetType& getInputNodes() const { return m_inputNodes; };
    const NodeSetType& getOutputNodes() const { return m_outputNodes; }

    virtual void addNode(NodeBasePointerType node,
                         NodeClassification type = NodeClassification::NormalNode)
    {
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
        m_nodes[type].emplace(node.get());
        // Add to storage map
        m_nodeMap.emplace(id, std::make_pair(std::move(node),type));
    }
//
//    virtual void addNodeToGroup(unsigned int nodeId, unsigned int groupId)
//    {
//        auto inNodeIt = m_nodeMap.find(nodeId);
//        if (inNodeIt == m_nodeMap.end())
//        {
//            EXEC_GRAPH_ERRORMSG("Node with id: " << nodeId << " has not been added to the tree!")
//        }
//        // Add to group
//        auto res = m_groupNodes[groupId].insert(inNodeIt->second);
//        if (!res.second)
//        {
//            EXEC_GRAPH_ERRORMSG("Node id: " << inNodeIt->second->m_id << " already in group: " << groupId);
//        }
//    }
//
//    virtual void makeGetLink(unsigned int outN, unsigned int outS, unsigned int inN, unsigned int inS)
//    {
//        auto outNit = m_nodeMap.find(outN);
//        auto inNit  = m_nodeMap.find(inN);
//        if (outNit == m_nodeMap.end() || inNit == m_nodeMap.end())
//        {
//            EXEC_GRAPH_ERRORMSG("Node: " << outN << " or " << inN << " does not exist!")
//        }
//        NodeBaseType::makeGetLink(outNit->second, outS, inNit->second, inS);
//    }
//
//    virtual void makeWriteLink(unsigned int outN, unsigned int outS, unsigned int inN, unsigned int inS)
//    {
//        auto outNit = m_nodeMap.find(outN);
//        auto inNit  = m_nodeMap.find(inN);
//        if (outNit == m_nodeMap.end() || inNit == m_nodeMap.end())
//        {
//            EXEC_GRAPH_ERRORMSG("Node: " << outN << " or " << inN << " does not exist!")
//        }
//        NodeBaseType::makeWriteLink(outNit->second, outS, inNit->second, inS);
//    }
//
//    /** Init group */
//    virtual void reset(unsigned int groupId)
//    {
//        for (auto& n : m_groupNodes[groupId])
//        {
//            // Reset in no order!
//            n->reset();
//        }
//    }
//
//    /** Execute group */
//    virtual void execute(unsigned int groupId)
//    {
//        for (auto& n : m_groupExecList[groupId])
//        {
//            // Execute in determined order!
//            n->compute();
//        }
//    }
//
//    //    /** Finalize group */
//    //    virtual void finalize(unsigned int groupId) {
//    //        for(auto & n : m_groupExecList[groupId]) {
//    //            // Execute in determined order!
//    //            n->compute();
//    //        }
//    //    }
//
//    /** Execute all groups */
//    virtual void executeAll()
//    {
//        for (auto& g : m_groupExecList)
//        {
//            for (auto& n : g.second)
//            {
//                n->compute();
//            }
//        }
//    }
//
//    /** Init all groups */
//    virtual void resetAll()
//    {
//        for (auto& g : m_groupExecList)
//        {
//            for (auto& n : g.second)
//            {
//                n->reset();
//            }
//        }
//    }
//
//    virtual void setup()
//    {
//        if (m_outputNodes.size() == 0)
//        {
//            EXEC_GRAPH_ERRORMSG("No output node specified")
//        }
//
//        // Solve execution order for every group list!
//        // Each group has its own execution order!
//        ExecutionOrderSolver s;
//        m_groupExecList.clear();
//        unsigned int maxPrio = 0;
//        for (auto& p : m_groupNodes)
//        {
//            // fill nodes into execution list
//            auto& l = m_groupExecList[p.first];
//            std::for_each(
//                p.second.begin(), p.second.end(), [&l](const typename NodeSetType::value_type& n) { l.push_back(n); });
//
//            s.solve(l, l);
//
//            maxPrio = std::max(maxPrio, l.back()->getPriority());
//        }
//
//        // do some manicure: invert all priorities such that lowest is now the
//        // highest
//        for (auto& p : m_groupExecList)
//        {
//            for (auto& n : p.second)
//            {
//                n->setPriority(maxPrio - n->getPriority());
//            }
//        }
//
//        // Check if input is reachable from all outputs
//        ReachNodeCheck c;
//        for (auto& o : m_outputNodes)
//        {
//            bool outputReachedInput = false;
//            // each outputnode should reach at least one input, if not print warning!
//            for (auto& i : m_inputNodes)
//            {
//                if (c.check(o, i))
//                {
//                    outputReachedInput = true;
//                    break;
//                }
//            }
//            if (!outputReachedInput)
//            {
//                WARNINGMSG(false, "WARNING: Output id: " << o->m_id << " did not reach any input!")
//            }
//        }
//    }
//
//    std::string getExecutionOrderInfo(std::string suffix = "\t\t")
//    {
//        // Print execution order
//        std::stringstream s;
//        for (auto& g : m_groupExecList)
//        {
//            s << "Execution order for group id: " << g.first << std::endl;
//            s << suffix << "NodeId\t|\tPriority\t|\tType" << std::endl;
//            for (auto n : g.second)
//            {
//                s << suffix
//                  << Utilities::stringFormat("%4i \t|\t %4i \t|\t %s", n->m_id, n->getPriority(), demangle::type(n))
//                  << std::endl;
//            }
//            s << suffix << "==============================" << std::endl;
//        }
//        return s.str();
//    }
//
//protected:
//    class ExecutionOrderSolver
//    {
//    public:
//        void solve(NodeListType& c, NodeListType& orderedNodes)
//        {
//            // Solve Execution order,
//            // start a depth first search recursion for all nodes in c which
//            // determines an execution order by setting
//            // the priority
//            for (auto& p : c)
//            {
//                std::unordered_set<unsigned int> nodesCurrDepth;
//                // start recursion from this node
//                solveRec(p, nodesCurrDepth);
//            }
//
//            // Sort all nodes according to priority (asscending) (lowest is most
//            // important)
//            std::sort(orderedNodes.begin(), orderedNodes.end(), [](NodeBaseType* const& a, NodeBaseType* const& b) {
//                return a->getPriority() < b->getPriority();
//            });
//        }
//
//    private:
//        /**
//         * Depth first search: this function returns recursively the priority
//         * lowest number has the highest priority
//         */
//
//        unsigned int solveRec(NodeBaseType* node, std::unordered_set<unsigned int>& nodesCurrDepth)
//        {
//            nodesCurrDepth.insert(node->m_id);
//
//            // visit all input sockets and their node!
//            auto& inSockets = node->getInputs();
//            for (auto* s : inSockets)
//            {
//                if (s->isLinked())
//                {
//                    auto* fsock = s->getFrom();
//
//                    auto* adjNode = fsock->getParent();
//                    if (nodesCurrDepth.find(adjNode->m_id) != nodesCurrDepth.end())
//                    {
//                        EXEC_GRAPH_ERRORMSG("Your execution logic graph contains a cylce from node: " << node->m_id
//                                                                                                << " socket: "
//                                                                                                << s->m_id
//                                                                                                << " to node: "
//                                                                                                << adjNode->m_id
//                                                                                                << " socket: "
//                                                                                                << fsock->m_id);
//                    }
//
//                    unsigned int prioAdj = solveRec(adjNode, nodesCurrDepth);
//
//                    // Set the current node to the priority of the below tree +1
//                    node->setPriority(std::max(prioAdj + 1, node->getPriority()));
//                }
//            }
//
//            nodesCurrDepth.erase(node->m_id);
//
//            return node->getPriority();
//        }
//        bool m_inputReachable  = false;
//        NodeBaseType* m_reachNode = nullptr;
//    };
//
//    // Only for directed graphs, does not detect cycles -> endless loop!
//    class ReachNodeCheck
//    {
//    public:
//        // From end to start node
//        bool check(NodeBaseType* endNode, NodeBaseType* startNode)
//        {
//            if (endNode == startNode)
//            {
//                return true;
//            }
//
//            m_start   = startNode;
//            m_reached = false;
//
//            std::deque<NodeBaseType*> currentNodes;  // Breath first search
//            currentNodes.push_back(endNode);
//
//            // visit current front node, as long as currentNode list is not empty or
//            // start has not yet been found!
//            while (currentNodes.size() != 0 && m_reached == false)
//            {
//                visit(currentNodes.front(), currentNodes);
//                currentNodes.pop_front();
//            }
//
//            return m_reached;
//        }
//
//    private:
//        /**
//         * Breath first search:
//         */
//        void visit(NodeBaseType* node, std::deque<NodeBaseType*>& queu)
//        {
//            // visit all input sockets and their node!
//            auto& inSockets = node->getInputs();
//            for (auto* s : inSockets)
//            {
//                if (s->isLinked())
//                {
//                    auto* fsock = s->getFrom();
//                    EXEC_GRAPH_ASSERTMSG(fsock, "linked but from ptr null");
//
//                    auto* adjNode = fsock->getParent();
//                    EXEC_GRAPH_ASSERTMSG(adjNode, "Adj node null");
//
//                    // If we reached the start node, return!
//                    if (m_start == adjNode)
//                    {
//                        m_reached = true;
//                        return;
//                    }
//
//                    queu.push_back(adjNode);
//                }
//            }
//        }
//
//        bool m_reached     = false;
//        NodeBaseType* m_start = nullptr;
//    };
//

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

    NodeSetType[nNodeClasses] m_nodes;         ///< the input/ouput and normal nodes

    NodeStorageType m_nodeMap;  ///< all nodes in the tree

    // Additional group list
    GroupNodeMapType m_groupNodes;

    GroupExecutionListType m_groupExecList;
};

#endif  // ExecutionTreeInOut_hpp
