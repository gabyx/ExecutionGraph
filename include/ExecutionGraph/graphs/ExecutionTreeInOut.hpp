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
#include <set>
#include <unordered_set>

#include "ExecutionGraph/common/Asserts.hpp"
#include "ExecutionGraph/common/DemangleTypes.hpp"
#include "ExecutionGraph/common/StringFormat.hpp"
#include "ExecutionGraph/nodes/LogicCommon.hpp"
#include "ExecutionGraph/nodes/LogicNode.hpp"
#include "ExecutionGraph/nodes/LogicSocket.hpp"

#define EXEC_GRAPH_EXECTREE_SOLVER_LOG(message) EXEC_GRAPH_DEBUG_ONLY(std::cout << message);
#define EXEC_GRAPH_EXECTREE_SOLVER_LOG_ON(message) std::cout << message;

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
    static const std::underlying_type_t<NodeClassification> m_nNodeClasses = 3;

    //! Internal Datastructure to store node related data.
    using NodePointer = std::unique_ptr<NodeBaseType>;
    struct NodeData
    {
        NodeData(NodePointer node, NodeClassification c)
            : m_node(std::move(node)), m_class(c)
        {
        }

        NodePointer m_node         = nullptr;
        NodeClassification m_class = NodeClassification::NormalNode;
        std::unordered_set<GroupId> m_groups;  //! To which group ids this node belongs.
        IndexType m_priority = 0;              //! The priority of this node

        void resetTraversalParameters() { m_flags = 0; }
        enum TraversalFlags : int
        {
            Visited,          //!< All visited nodes (globally)
            OnCurrentDFRPath  //!< This mark is set for all nodes on the current depth-first recursion path.
        };
        bool isFlagSet(TraversalFlags position) { return m_flags & (1 << position); }
        void setFlag(TraversalFlags position) { m_flags |= (1 << position); }
        void unsetFlag(TraversalFlags position) { m_flags &= ~(1 << position); }

    private:
        int m_flags = 0;  //! Some flags for graph traversal
    };

    using NodeDataStorage = std::unordered_map<NodeId, NodeData>;  // Rehashing does not invalidate pointers or references to elements.

    using NodeDataList = std::vector<NodeData*>;
    using NodeDataSet  = std::unordered_set<NodeData*>;
    using GroupNodeMap = std::unordered_map<GroupId, NodeDataSet>;

    using PrioritySet        = std::map<IndexType, NodeDataList, std::greater<IndexType>>;
    using GroupExecutionList = std::unordered_map<GroupId, PrioritySet>;

public:
    ExecutionTreeInOut()          = default;
    virtual ~ExecutionTreeInOut() = default;

    //! Set the node class of a specific node id \p nodeId.
    //! Invalidates the execution order.
    void setNodeClass(NodeId nodeId, NodeClassification newType)
    {
        m_executionOrderUpToDate = false;

        auto it = m_nodeMap.find(nodeId);
        EXEC_GRAPH_THROWEXCEPTION_IF(it == m_nodeMap.end(), "Node with id: " << nodeId << " does not exist in tree!");

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

    //! Set the node class of a specific \p node.
    //! Invalidates the execution order.
    void setNodeClass(NodeBaseType& node, NodeClassification newType)
    {
        setNodeClass(node.getId(), newType);
    }

    //! Get a specific node with id \p nodeId if it exists, nullptr otherwise.
    //! This invalidates the execution order, since we cannot guarantee that the caller added other links.
    //! To circumvent that, use the const method.
    NodeBaseType* getNode(NodeId nodeId)
    {
        auto it = m_nodeMap.find(nodeId);
        if (it == m_nodeMap.end())
        {
            return nullptr;
        }
        m_executionOrderUpToDate = false;
        return it->second.m_node.get();
    }
    //! Get a specific node with id \p nodeId if it exists, nullptr otherwise.
    //! Does not invalidate execution order.
    const NodeBaseType* getNode(NodeId nodeId) const
    {
        auto* p                  = static_cast<ExecutionTreeInOut<Config> const*>(this)->getNode(nodeId);
        m_executionOrderUpToDate = true;
        return p;
    }

    //! Get all nodes classified as \p type.
    const NodeDataSet& getNodes(NodeClassification type) const { return m_nodeClasses[type]; }

    //! Get all nodes in the group with id \p groupId.
    const NodeDataSet& getNodes(GroupId groupId) const
    {
        auto it = m_nodeGroups.find(groupId);
        EXEC_GRAPH_THROWEXCEPTION_IF(it == m_nodeGroups.end(),
                                     "Group with id: " << groupId << " is not part of the tree!");
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
        const auto& p = m_nodeMap.emplace(id, NodeData{std::move(node), type});
        m_allNodes.emplace(&(p.first->second));
        // Add node to group
        addNodeToGroup(id, groupId);
    }

    //! Add the node with id \p nodeId to the group with id \p groupId.
    virtual void addNodeToGroup(NodeId nodeId, GroupId groupId)
    {
        m_executionOrderUpToDate = false;

        auto it = m_nodeMap.find(nodeId);
        EXEC_GRAPH_THROWEXCEPTION_IF(it == m_nodeMap.end(), "Node with id: " << nodeId << " does not exist in tree!");
        // Add node to the group
        it->second.m_groups.emplace(groupId);
        m_nodeGroups[groupId].emplace(&it->second);
    }

    //! Constructs a Get-Link to get the data from output socket at index \p outS
    //! of logic node \p outN at the input socket at index \p inS.
    virtual void makeGetLink(NodeId outN, SocketIndex outS, NodeId inN, SocketIndex inS)
    {
        m_executionOrderUpToDate = false;
        auto outNit              = m_nodeMap.find(outN);
        auto inNit               = m_nodeMap.find(inN);
        EXEC_GRAPH_THROWEXCEPTION_IF(outNit == m_nodeMap.end() || inNit == m_nodeMap.end(),
                                     "Node with id: " << outN << " or " << inN << " does not exist!")
        NodeBaseType::setGetLink(*outNit->second.m_node, outS, *inNit->second.m_node, inS);
    }

    //! Constructs a Write-Link to write the data of output socket at index \p
    //! outS of logic node \p outN to the input socket at index \p inS of logic node \p
    //! inN.
    virtual void makeWriteLink(NodeId outN, SocketIndex outS, NodeId inN, SocketIndex inS)
    {
        m_executionOrderUpToDate = false;
        auto outNit              = m_nodeMap.find(outN);
        auto inNit               = m_nodeMap.find(inN);
        if (outNit == m_nodeMap.end() || inNit == m_nodeMap.end())
        {
            EXEC_GRAPH_THROWEXCEPTION("Node: " << outN << " or " << inN << " does not exist!");
        }
        NodeBaseType::addWriteLink(*outNit->second.m_node, outS, *inNit->second.m_node, inS);
    }

    //! Reset all nodes in group with id: \p groupId.
    virtual void reset(unsigned int groupId)
    {
        EXEC_GRAPH_THROWEXCEPTION_IF(!m_executionOrderUpToDate,
                                     "ExecutionTree's execution order is not up to date!");
        // Execute in determined order!
        auto it = m_groupExecList.find(groupId);
        EXEC_GRAPH_THROWEXCEPTION_IF(it == m_groupExecList.end(),
                                     "ExecutionTree does not contain a group with id: " << groupId);
        executePrioritySet(it->second, [](NodeBaseType& node) { node.reset(); });
    }

    //! Reset the whole graph.
    virtual void reset()
    {
        executePrioritySet(m_execList, [](NodeBaseType& node) { node.reset(); });
    }

    //! Execute all nodes in group with id: \p groupId in their determined order.
    virtual void execute(unsigned int groupId)
    {
        EXEC_GRAPH_THROWEXCEPTION_IF(!m_executionOrderUpToDate,
                                     "ExecutionTree's execution order is not up to date!");
        // Execute in determined order!
        auto it = m_groupExecList.find(groupId);
        EXEC_GRAPH_THROWEXCEPTION_IF(it == m_groupExecList.end(),
                                     "ExecutionTree does not contain a group with id: " << groupId);
        executePrioritySet(it->second, [](NodeBaseType& node) { node.compute(); });
    }

    //! Execute the whole graph.
    virtual void execute()
    {
        EXEC_GRAPH_THROWEXCEPTION_IF(!m_executionOrderUpToDate,
                                     "ExecutionTree's execution order is not up to date!")

        executePrioritySet(m_execList, [](NodeBaseType& node) { node.compute(); });
    }

    //! Setups the execution tree by building its execution order.
    virtual void setup(bool checkResults = false)
    {
        // Allways check results in Debug mode.
        EXEC_GRAPH_DEBUG_ONLY(checkResults = true;);

        if (m_nodeClasses[NodeClassification::OutputNode].size() == 0)
        {
            EXEC_GRAPH_THROWEXCEPTION("No output nodes specified!");
        }

        // Solve execution order globally over all groups!
        // Each group has its own execution order based on the the global computed one!
        ExecutionOrderSolver solver(m_nodeMap, m_allNodes);
        solver.solve(m_execList, m_groupExecList);

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

        m_executionOrderUpToDate = true;
    }

    //! Get execution order information.
    std::string getExecutionOrderInfo(std::string suffix = "\t\t")
    {
        // Print execution order
        std::stringstream s;
        std::string fmtH = "%-15s  |  %-6s  | %-8s   |  %-20s";
        std::string fmt  = "%-15s  |  %-6i  | %-8i   |  %-20s";
        for (auto& g : m_groupExecList)
        {
            s << "Execution order for group id: " << g.first << std::endl;
            s << suffix << stringFormat(fmtH, "Name", "NodeId", "Priority", "NodeType") << std::endl;
            s << suffix << stringFormat(fmtH, "---------------", "------", "--------", "--------") << std::endl;
            for (auto& p : g.second)
            {
                for (NodeData* nodeData : p.second)
                {
                    auto* n = nodeData->m_node.get();
                    s << suffix
                      << stringFormat(fmt, n->getName().c_str(), n->getId(), nodeData->m_priority, n->getTypeName().c_str())
                      << std::endl;
                }
            }
            s << suffix << stringFormat(fmtH, "---------------", "------", "--------", "--------") << std::endl;
        }
        return s.str();
    }

protected:
    //! Execute all nodes in PrioritySet.
    template<typename Functor>
    inline void executePrioritySet(PrioritySet& prioritySet, Functor&& func)
    {
        for (auto& p : prioritySet)
        {
            // Execute all nodes with this priority
            for (NodeData* nodeData : p.second)
            {
                func(*nodeData->m_node);
            }
        }
    }

    //! The solver for computing the execution order.
    class ExecutionOrderSolver
    {
    public:
        ExecutionOrderSolver(NodeDataStorage& nodeMap, NodeDataSet& nodes)
            : m_nodeMap(nodeMap), m_nodes(nodes)
        {
        }

        /*! Solves the execution order for an input node set `nodes` and
            outputs the ordered execution list `orderedNodes`.

            This algorithm is based on https://en.wikipedia.org/wiki/Topological_sorting#Depth-first_search.
            We visit each node once during this algorithm.
            From each node `a` in `nodes` we start a depth-first recursion (DFR), basically exploring the whole subtree `S`
            of each node `a`. While exploring, we set the NodeData::m_priority of each node in the subtree with root = `a`.

            For each DFR, we trace the current path explored by marking each node by the flag NodeData::OnCurrentDFRPath.
            This allows to detect cycles.

            When a node is visited its NodeData::Visited flag gets set.
            A DFR starts only from a node which has NodeData::Visited == false.
            During a DFR, a new child node is explored (added to the m_dfrStack) only if
            its priority is not greater than the parent.
            When the child node is added the flag NodeData::Visited is unset!
        */
        void solve(PrioritySet& prioritiesGlobal,
                   GroupExecutionList& prioritiesPerGroup,
                   bool checkResults = true)
        {
            prioritiesPerGroup.clear();
            prioritiesGlobal.clear();

            //! @todo Find a good stack size depending on tree height.
            m_dfrStack.reserve(10);

            // Remove all nodes up the stack which are visited
            // while doing so unmark the nodes.
            auto doBackTracking = [&]() {
                auto itStart = m_dfrStack.rbegin();
                auto itEnd   = m_dfrStack.rend();
                auto it      = itStart;
                while (it != itEnd)
                {
                    if (!(*it)->isFlagSet(NodeData::Visited))
                    {
                        // Not visited, backtracking finished!
                        // Remove the range [itStart,itLast) from the stack
                        break;
                    }
                    (*it)->unsetFlag(NodeData::OnCurrentDFRPath);  // Remove Mark
                    ++it;
                }
                EXEC_GRAPH_ASSERT(std::distance(itStart, it) <= static_cast<typename decltype(m_dfrStack)::difference_type>(m_dfrStack.size()),
                                  "Removing: " << std::distance(itStart, it) << " from " << m_dfrStack.size());
                // Convert backward iterator to forward
                m_dfrStack.erase(it.base(), itStart.base());
            };

            // Loop over all nodes and start a depth-first-search
            for (NodeData* nodeData : m_nodes)
            {
                // If the node is visited we know that the node was contained in a depth-first recursion
                // we know that the prioriteis below it are correct, so skip this one
                if (nodeData->isFlagSet(NodeData::Visited))
                {
                    EXEC_GRAPH_EXECTREE_SOLVER_LOG("DFS Start: Node id: " << nodeData->m_node->getId()
                                                                          << " already visited -> skip it."
                                                                          << std::endl;);
                    continue;
                }

                EXEC_GRAPH_EXECTREE_SOLVER_LOG("DFS Start: Node id: " << nodeData->m_node->getId() << std::endl;);

                // Start a depth-first recursion from this node (exploring its subtree)
                m_dfrStack.clear();
                // Insert root node
                m_dfrStack.push_back(nodeData);

                // Loop variables
                NodeData* currentNode;
                std::size_t currentSize;

                while (!m_dfrStack.empty())
                {
                    EXEC_GRAPH_EXECTREE_SOLVER_LOG("DFS Stack:" << getStackInfo() << std::endl);

                    currentNode = m_dfrStack.back();
                    currentSize = m_dfrStack.size();

                    // We are doing depth first search and try to visit a node which is already on the
                    // current DFR path.
                    EXEC_GRAPH_THROWEXCEPTION_IF(currentNode->isFlagSet(NodeData::OnCurrentDFRPath),
                                                 "Your execution logic graph contains a cycle! "
                                                 "Current traversal stack: "
                                                     << getTraversalInfo());

                    visit(*currentNode);  // Visits neighbors and add them to m_dfrStack

                    // If no nodes have been added, down traversal is finished, we do now backtracking
                    if (currentSize == m_dfrStack.size())
                    {
                        doBackTracking();  // Removing all visited nodes up the stack
                    }
                }
            }

            // Loop over all nodes and make priority sets:
            for (NodeData* nodeData : m_nodes)
            {
                nodeData->resetTraversalParameters();

                prioritiesGlobal[nodeData->m_priority].emplace_back(nodeData);
                // Put the nodes into PrioritySets for each Group
                for (auto& groupId : nodeData->m_groups)
                {
                    prioritiesPerGroup[groupId][nodeData->m_priority].emplace_back(nodeData);
                }
            }

            // Check execution order by checking all inputs of all nodes
            if (checkResults)
            {
                this->checkResults(m_nodes);
            }
        }

    private:
        //! Return current DFS stack, but only nodes which are on the current DFR path.
        std::string getTraversalInfo()
        {
            std::stringstream ss;
            auto it = m_dfrStack.begin();
            if (it != m_dfrStack.end() && (*it)->isFlagSet(NodeData::OnCurrentDFRPath))
            {
                ss << (*(it++))->m_node->getId();
            }
            for (; it != m_dfrStack.end(); ++it)
            {
                if ((*it)->isFlagSet(NodeData::OnCurrentDFRPath))
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
            ss << "[ ";
            for (auto* nodeData : m_dfrStack)
            {
                ss << nodeData->m_node->getId() << (nodeData->isFlagSet(NodeData::Visited) ? "*" : " ") << ", ";
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

            auto addParentsToStack = [&](auto* socket) {

                // Get NodeData of parentNode
                auto& parentNode = socket->getParent();
                auto itParent    = m_nodeMap.find(parentNode.getId());
                EXEC_GRAPH_THROWEXCEPTION_IF(itParent == m_nodeMap.end(),
                                             "Node with id: " << parentNode.getId() << " has not been added to the execution tree!");
                NodeData* parentNodeData = &itParent->second;

                if (parentNodeData->m_priority <= nodeData.m_priority)
                {
                    // Parent needs a other priority (because its computation becomes before nodeData)
                    parentNodeData->m_priority = nodeData.m_priority + 1;
                    parentNodeData->unsetFlag(NodeData::Visited);
                    m_dfrStack.push_back(parentNodeData);  // Add to stack and explore its subgraph
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
                for (auto* outputSocket : socket->getWritingSockets())
                {
                    addParentsToStack(outputSocket);
                }
            }

            // Mark this node as on the current depth-first recursion path
            nodeData.setFlag(NodeData::OnCurrentDFRPath);

            // Mark this node as visited
            nodeData.setFlag(NodeData::Visited);
        }

        void checkResults(const NodeDataSet& nodes)
        {
            auto check = [&](auto* socket, NodeData* nodeDataWithLowerPrio) {
                // Get NodeData of parentNode
                auto& parentNode = socket->getParent();
                auto itParent    = m_nodeMap.find(parentNode.getId());
                EXEC_GRAPH_THROWEXCEPTION_IF(itParent == m_nodeMap.end(),
                                             "Node with id: " << parentNode.getId() << " has not been added to the execution tree!");
                NodeData* parentNodeData = &itParent->second;

                EXEC_GRAPH_THROWEXCEPTION_IF(parentNodeData->m_priority <= nodeDataWithLowerPrio->m_priority,
                                             "Parent node id: " << parentNodeData->m_node->getId() << "[prio: " << parentNodeData->m_priority << " ]"
                                                                << "has not a higher priority as node id: "
                                                                << nodeDataWithLowerPrio->m_node->getId()
                                                                << " which is wrong!");
            };

            for (auto* nodeData : nodes)
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
                    for (auto* outputSocket : socket->getWritingSockets())
                    {
                        check(outputSocket, nodeData);
                    }
                }
            }
        }

        bool m_inputReachable     = false;
        NodeBaseType* m_reachNode = nullptr;
        NodeDataStorage& m_nodeMap;  //! All NodeDatas of the execution tree.
        NodeDataSet& m_nodes;        //! All NodeDatas of the execution tree.

        std::vector<NodeData*> m_dfrStack;  //! Depth-First-Search Stack
    };

    //! First computes a topological order and then assigns priorities.
    class ExecutionOrderSolver2
    {
    public:
        ExecutionOrderSolver2(NodeDataStorage& nodeMap, NodeDataSet& nodes)
            : m_nodeMap(nodeMap), m_nodes(nodes)
        {
        }

        void solve(PrioritySet& prioritiesGlobal,
                   GroupExecutionList& prioritiesPerGroup,
                   bool checkResults = true)
        {
            prioritiesPerGroup.clear();
            prioritiesGlobal.clear();

            //! @todo Find a good stack size depending on tree height.
            m_dfrStack.reserve(10);
            NodeDataList topoSortList;
            topoSortList.reserve(10);

            // Remove all nodes up the stack which are visited
            // while doing so unmark the nodes.
            auto doBackTracking = [&]() {
                auto itStart = m_dfrStack.rbegin();
                auto itEnd   = m_dfrStack.rend();
                auto it      = itStart;
                while (it != itEnd)
                {
                    if (!(*it)->isFlagSet(NodeData::Visited))
                    {
                        // Not visited, backtracking finished!
                        // Remove the range [itStart,itLast) from the stack
                        break;
                    }

                    // If the node is marked unmark and add it to the topoSortList
                    if ((*it)->isFlagSet(NodeData::OnCurrentDFRPath))
                    {
                        topoSortList.push_back(*it);
                        (*it)->unsetFlag(NodeData::OnCurrentDFRPath);
                    }

                    // Goto next node
                    ++it;
                }
                EXEC_GRAPH_ASSERT(std::distance(itStart, it) <= m_dfrStack.size(),
                                  "Removing: " << std::distance(itStart, it) << " from " << m_dfrStack.size());
                // Convert backward iterator to forward
                m_dfrStack.erase(it.base(), itStart.base());
            };

            // Loop over all nodes and start a depth-first-search
            for (NodeData* nodeData : m_nodes)
            {
                EXEC_GRAPH_EXECTREE_SOLVER_LOG("DFS Start: Node id: " << nodeData->m_node->getId() << std::endl;);
                // Skip visited nodes.
                if (nodeData->isFlagSet(NodeData::Visited))
                {
                    continue;
                }

                // Start a depth-first recursion from this node (exploring this subtree)
                m_dfrStack.clear();
                // Insert root node
                m_dfrStack.push_back(nodeData);

                NodeData* currentNode;
                std::size_t currentSize;

                while (!m_dfrStack.empty())
                {
                    EXEC_GRAPH_EXECTREE_SOLVER_LOG("DFS Stack:" << getStackInfo() << std::endl);

                    currentNode = m_dfrStack.back();

                    // We are doing depth first search and found another already visited node
                    // meaning we have a cycle.
                    EXEC_GRAPH_THROWEXCEPTION_IF(currentNode->isFlagSet(NodeData::OnCurrentDFRPath),
                                                 "Your execution logic graph contains a cycle! "
                                                     << "Current traversal stack: "
                                                     << getTraversalInfo());

                    currentNode->setFlag(NodeData::OnCurrentDFRPath);
                    currentSize = m_dfrStack.size();
                    visit(*currentNode);  // Visits neighbors and add them to m_dfrStack

                    // If no nodes have been added, down traversal is finished, we do now backtracking
                    if (currentSize == m_dfrStack.size())
                    {
                        doBackTracking();  // Removing all visited nodes up the stack
                    }
                }
            }

            // Topological sort finished
            // Traverse the sorted list from the back and assign the priorities

            auto rEndIt = topoSortList.rend();
            for (auto nodeDataIt = topoSortList.rbegin(); nodeDataIt != rEndIt; ++nodeDataIt)
            {
                NodeData* nodeData = *nodeDataIt;
                assignPrioritiesToChilds(*nodeData);
            }

            for (NodeData* nodeData : topoSortList)
            {
                prioritiesGlobal[nodeData->m_priority].emplace_back(nodeData);
                // Put the nodes into PrioritySets for each Group
                for (auto& groupId : nodeData->m_groups)
                {
                    prioritiesPerGroup[groupId][nodeData->m_priority].emplace_back(nodeData);
                }
            }

            // Check execution order by checking all inputs of all nodes
            if (checkResults)
            {
                this->checkResults(m_nodes);
            }
        }

    private:
        //! Return current DFS stack, but only nodes which are on the current DFR path.
        std::string getTraversalInfo()
        {
            std::stringstream ss;
            auto it = m_dfrStack.begin();
            if (it != m_dfrStack.end() && (*it)->isFlagSet(NodeData::OnCurrentDFRPath))
            {
                ss << (*(it++))->m_node->getId();
            }
            for (; it != m_dfrStack.end(); ++it)
            {
                if ((*it)->isFlagSet(NodeData::OnCurrentDFRPath))
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
            ss << "[ ";
            for (auto* nodeData : m_dfrStack)
            {
                ss << nodeData->m_node->getId() << (nodeData->isFlagSet(NodeData::Visited) ? "*" : " ") << ", ";
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

            auto addParentsToStack = [&](auto* socket) {
                // Get NodeData of parentNode
                auto& parentNode = socket->getParent();
                auto itParent    = m_nodeMap.find(parentNode.getId());
                EXEC_GRAPH_THROWEXCEPTION_IF(itParent == m_nodeMap.end(),
                                             "Node with id: " << parentNode.getId() << " has not been added to the execution tree!");
                NodeData* parentNodeData = &itParent->second;

                if (!parentNodeData->isFlagSet(NodeData::Visited))
                {
                    m_dfrStack.push_back(parentNodeData);  // Add to stack and explore its subgraph
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
                for (auto* outputSocket : socket->getWritingSockets())
                {
                    addParentsToStack(outputSocket);
                }
            }

            // Mark this node as visited
            nodeData.setFlag(NodeData::Visited);
        }

        void assignPrioritiesToChilds(NodeData& nodeData)
        {
            auto assignToChild = [&](auto* socket) {
                // Get NodeData of parentNode
                auto& parentNode = socket->getParent();
                auto itParent    = m_nodeMap.find(parentNode.getId());
                EXEC_GRAPH_THROWEXCEPTION_IF(itParent == m_nodeMap.end(),
                                             "Node with id: " << parentNode.getId() << " has not been added to the execution tree!");
                NodeData* parentNodeData = &itParent->second;

                if (parentNodeData->m_priority <= nodeData.m_priority)
                {
                    parentNodeData->m_priority = nodeData.m_priority + 1;
                }
            };

            // Follow all links
            auto& inSockets = nodeData.m_node->getInputs();
            for (auto& socket : inSockets)
            {
                // Try adding the get link to the stack
                if (socket->hasGetLink())
                {
                    assignToChild(socket->followGetLink());
                }
                // Try adding all writing links to the stack
                for (auto* outputSocket : socket->getWritingSockets())
                {
                    assignToChild(outputSocket);
                }
            }
        }

        void checkResults(const NodeDataSet& nodes)
        {
            auto check = [&](auto* socket, NodeData* nodeDataWithLowerPrio) {
                // Get NodeData of parentNode
                auto& parentNode = socket->getParent();
                auto itParent    = m_nodeMap.find(parentNode.getId());
                EXEC_GRAPH_THROWEXCEPTION_IF(itParent == m_nodeMap.end(),
                                             "Node with id: " << parentNode.getId() << " has not been added to the execution tree!");
                NodeData* parentNodeData = &itParent->second;

                EXEC_GRAPH_THROWEXCEPTION_IF(parentNodeData->m_priority <= nodeDataWithLowerPrio->m_priority,
                                             "Parent node id: " << parentNodeData->m_node->getId() << "[prio: "
                                                                << parentNodeData->m_priority
                                                                << " ]"
                                                                << "has not a higher priority as node id: "
                                                                << nodeDataWithLowerPrio->m_node->getId()
                                                                << " which is wrong!");
            };

            for (auto* nodeData : nodes)
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
                    for (auto* outputSocket : socket->getWritingSockets())
                    {
                        check(outputSocket, nodeData);
                    }
                }
            }
        }

        bool m_inputReachable     = false;
        NodeBaseType* m_reachNode = nullptr;
        NodeDataStorage& m_nodeMap;  //! All NodeDatas of the execution tree.
        NodeDataSet& m_nodes;        //! All NodeDatas of the execution tree.

        std::vector<NodeData*> m_dfrStack;  //! Depth-First-Search Stack
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
                    auto& parentNode   = outputSocket->getParent();
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

    std::set<NodeBaseType*> m_nodeClasses[m_nNodeClasses];  //!< The classification set for each node class.

    NodeDataStorage m_nodeMap;  //!< All nodes in the execution tree (main storage).
    NodeDataSet m_allNodes;     //!< All nodes in the execution tree.

    GroupNodeMap m_nodeGroups;           //!< The map of nodes in each group.
    PrioritySet m_execList;              //!< The global execution order.
    GroupExecutionList m_groupExecList;  //!< The execution order for each group.

    bool m_executionOrderUpToDate = false;  //!< Dirty flag which denotes that the execution order is not up to date!
};
}

#undef EXEC_GRAPH_EXECTREE_SOLVER_LOG
#endif  // ExecutionTreeInOut_hpp
