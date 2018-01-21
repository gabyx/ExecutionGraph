//! ========================================================================================
//!  ExecutionGraph
//!  Copyright (C) 2014 by Gabriel Nützi <gnuetzi (at) gmail (døt) com>
//!
//!  @date Mon Jan 08 2018
//!  @author Gabriel Nützi, <gnuetzi (at) gmail (døt) com>
//!
//!  This Source Code Form is subject to the terms of the Mozilla Public
//!  License, v. 2.0. If a copy of the MPL was not distributed with this
//!  file, You can obtain one at http://mozilla.org/MPL/2.0/.
//! ========================================================================================

#ifndef executionGraph_graphs_ExecutionTreeInOut_hpp
#define executionGraph_graphs_ExecutionTreeInOut_hpp

#include <algorithm>
#include <deque>
#include <set>
#include <unordered_set>

#include "executionGraph/common/Asserts.hpp"
#include "executionGraph/common/DemangleTypes.hpp"
#include "executionGraph/common/StringFormat.hpp"
#include "executionGraph/nodes/LogicCommon.hpp"
#include "executionGraph/nodes/LogicNode.hpp"
#include "executionGraph/nodes/LogicNodeDefaultPool.hpp"
#include "executionGraph/nodes/LogicSocket.hpp"

#define EXECGRAPH_EXECTREE_SOLVER_LOG(message) EXECGRAPH_DEBUG_ONLY(EXECGRAPH_LOG_TRACE(message));

namespace executionGraph
{
    template<typename TConfig>
    class ExecutionTreeInOut
    {
    public:
        EXECGRAPH_TYPEDEF_CONFIG(TConfig);

        enum NodeClassification : unsigned char
        {
            NormalNode   = 0,
            InputNode    = 1,
            OutputNode   = 2,
            ConstantNode = 3
            //! If you changes this -> adjust m_nNodeClasses!
        };

        using GroupId = unsigned int;

    private:
        static const std::underlying_type_t<NodeClassification> m_nNodeClasses = 4;

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

        using ConstantNodeStorage = std::unordered_map<NodeId, NodePointer>;
        using NodeDataStorage     = std::unordered_map<NodeId, NodeData>;  // Rehashing does not invalidate pointers or references to elements.

        using NodeDataList = std::vector<NodeData*>;
        using NodeDataSet  = std::unordered_set<NodeData*>;
        using GroupNodeMap = std::unordered_map<GroupId, NodeDataSet>;

        using PrioritySet        = std::map<IndexType, NodeDataList, std::greater<IndexType>>;
        using GroupExecutionList = std::unordered_map<GroupId, PrioritySet>;

        using LogicNodeDefaultOutputs = LogicNodeDefaultPool<TConfig>;

    public:
        ExecutionTreeInOut()
        {
            // Make a default pool of output sockets.
            auto p                  = std::make_unique<LogicNodeDefaultOutputs>(std::numeric_limits<NodeId>::max(), "DefaultOutputPool");
            m_nodeDefaultOutputPool = p.get();
            addNode(std::move(p), NodeClassification::ConstantNode);
        }
        virtual ~ExecutionTreeInOut() = default;

        //! Set the node class of a specific node id \p nodeId.
        //! Invalidates the execution order.
        void setNodeClass(NodeId nodeId, NodeClassification newType)
        {
            m_executionOrderUpToDate = false;

            auto it = m_nodeMap.find(nodeId);
            EXECGRAPH_THROW_EXCEPTION_IF(it == m_nodeMap.end(), "Node with id: " << nodeId << " does not exist in tree!");

            NodeClassification& currType = it->second.m_class;
            if(currType == newType)
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
            if(it == m_nodeMap.end())
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

        //! Get the pool of default output sockets.
        //! All not connected input sockets will be hooked up to these default output sockets!
        LogicNodeDefaultOutputs& getDefaultOuputPool() { return *m_nodeDefaultOutputPool; }

        //! Get all nodes classified as \p type.
        const NodeDataSet& getNodes(NodeClassification type) const { return m_nodeClasses[type]; }

        //! Get all nodes in the group with id \p groupId.
        const NodeDataSet& getNodes(GroupId groupId) const
        {
            auto it = m_nodeGroups.find(groupId);
            EXECGRAPH_THROW_EXCEPTION_IF(it == m_nodeGroups.end(),
                                         "Group with id: " << groupId << " is not part of the tree!");
            return it->second;
        }

        //! Adds a node to the execution tree and classifies it as \p type.
        virtual NodeBaseType* addNode(NodePointer node,
                                      NodeClassification type = NodeClassification::NormalNode,
                                      GroupId groupId         = 0)
        {
            if(type != NodeClassification::ConstantNode)
            {
                m_executionOrderUpToDate = false;
            }
            EXECGRAPH_THROW_EXCEPTION_IF(node == nullptr, "Nullptr added!");

            auto id             = node->getId();
            NodeBaseType* pNode = nullptr;

            if(type == NodeClassification::ConstantNode)
            {
                // Constant node
                if(m_constantNodes.find(id) != m_constantNodes.end())
                {
                    EXECGRAPH_WARNINGMSG(0, "Constant node id: " << node->getId() << " already added in tree!");
                    return nullptr;
                }
                pNode = node.get();
                m_constantNodes.emplace(id, std::move(node));
                // Add to classes
                m_nodeClasses[type].emplace(pNode);
            }
            else
            {
                // Any other node
                if(m_nodeMap.find(id) != m_nodeMap.end())
                {
                    EXECGRAPH_WARNINGMSG(0, "Node id: " << node->getId() << " already added in tree!");
                    return nullptr;
                }

                // Add to storage map
                pNode          = node.get();
                const auto& p  = m_nodeMap.emplace(id, NodeData{std::move(node), type});
                auto* nodeData = &(p.first->second);
                m_allNodes.emplace(nodeData);
                // Add to classes
                m_nodeClasses[type].emplace(pNode);
                // Add node to group
                addNodeToGroup(id, groupId);
            }
            return pNode;
        }

        //! Add the node with id \p nodeId to the group with id \p groupId.
        virtual void addNodeToGroup(NodeId nodeId, GroupId groupId)
        {
            m_executionOrderUpToDate = false;

            auto it = m_nodeMap.find(nodeId);
            EXECGRAPH_THROW_EXCEPTION_IF(it == m_nodeMap.end(), "Node with id: " << nodeId << " does not exist in tree!");
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
            EXECGRAPH_THROW_EXCEPTION_IF(outNit == m_nodeMap.end() || inNit == m_nodeMap.end(),
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
            if(outNit == m_nodeMap.end() || inNit == m_nodeMap.end())
            {
                EXECGRAPH_THROW_EXCEPTION("Node: " << outN << " or " << inN << " does not exist!");
            }
            NodeBaseType::addWriteLink(*outNit->second.m_node, outS, *inNit->second.m_node, inS);
        }

        //! Reset all nodes in group with id: \p groupId.
        virtual void reset(unsigned int groupId)
        {
            EXECGRAPH_THROW_EXCEPTION_IF(!m_executionOrderUpToDate,
                                         "ExecutionTree's execution order is not up to date!");
            // Execute in determined order!
            auto it = m_groupExecList.find(groupId);
            EXECGRAPH_THROW_EXCEPTION_IF(it == m_groupExecList.end(),
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
            EXECGRAPH_THROW_EXCEPTION_IF(!m_executionOrderUpToDate,
                                         "ExecutionTree's execution order is not up to date!");
            // Execute in determined order!
            auto it = m_groupExecList.find(groupId);
            EXECGRAPH_THROW_EXCEPTION_IF(it == m_groupExecList.end(),
                                         "ExecutionTree does not contain a group with id: " << groupId);
            executePrioritySet(it->second, [](NodeBaseType& node) { node.compute(); });
        }

        //! Execute the whole graph.
        virtual void execute()
        {
            EXECGRAPH_THROW_EXCEPTION_IF(!m_executionOrderUpToDate,
                                         "ExecutionTree's execution order is not up to date!")

            executePrioritySet(m_execList, [](NodeBaseType& node) { node.compute(); });
        }

        //! Setups the execution tree by building its execution order.
        virtual void setup(bool connectAllDanglingInputs = true, bool checkResults = false)
        {
            // Allways check results in Debug mode.
            EXECGRAPH_DEBUG_ONLY(checkResults = true;)

            if(m_nodeClasses[NodeClassification::OutputNode].size() == 0)
            {
                EXECGRAPH_THROW_EXCEPTION("No output nodes specified!");
            }

            // Solve execution order globally over all groups!
            // Each group has its own execution order based on the the global computed one!
            ExecutionOrderSolver solver(m_nodeMap,
                                        m_allNodes,
                                        m_constantNodes,
                                        (connectAllDanglingInputs) ? m_nodeDefaultOutputPool : nullptr);
            solver.solve(m_execList, m_groupExecList);

            // Check if each output node reaches at least one input, if not print warning!
            ReachNodeCheck c;
            for(auto* outNode : m_nodeClasses[NodeClassification::OutputNode])
            {
                bool outputReachedInput = false;

                for(auto* inNode : m_nodeClasses[NodeClassification::InputNode])
                {
                    if(c.check(outNode, inNode))
                    {
                        outputReachedInput = true;
                        break;
                    }
                }
                EXECGRAPH_WARNINGMSG(outputReachedInput, "WARNING: Output id: " << outNode->getId() << " did not reach any input!")
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
            for(auto& g : m_groupExecList)
            {
                s << "Execution order for group id: " << g.first << std::endl;
                s << suffix << stringFormat(fmtH, "Name", "NodeId", "Priority", "NodeType") << std::endl;
                s << suffix << stringFormat(fmtH, "---------------", "------", "--------", "--------") << std::endl;
                for(auto& p : g.second)
                {
                    for(NodeData* nodeData : p.second)
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
            for(auto& p : prioritySet)
            {
                // Execute all nodes with this priority
                for(NodeData* nodeData : p.second)
                {
                    func(*nodeData->m_node);
                }
            }
        }

        class ExecutionSolverBase
        {
        public:
            ExecutionSolverBase(ConstantNodeStorage& constantNodes, LogicNodeDefaultOutputs* defaultOutputSockets = nullptr)
                : m_constantNodes(constantNodes), m_defaultOutputSockets(defaultOutputSockets) {}

            //! Connects all dangling input sockets to the default output socket.
            void connectAllDanglingInputs(NodeData& nodeData)
            {
                // All input sockets need to be connected!
                for(SocketInputBasePointer& inSocket : nodeData.m_node->getInputs())
                {
                    if(m_defaultOutputSockets)
                    {
                        m_defaultOutputSockets->connectIfDangling(*inSocket);
                    }
                    else
                    {
                        EXECGRAPH_THROW_EXCEPTION_TYPE_IF(inSocket->getConnectionCount() == 0,
                                                          "Input socket index: " << inSocket->getIndex()
                                                                                 << "of node: " << nodeData.m_node->getId() << " is not connected!",
                                                          NodeConnectionException);
                    }
                }
            }

            void checkResults(const NodeDataStorage& nodes)
            {
                auto check = [&](auto* socket, const NodeData& nodeDataWithLowerPrio) {
                    // Get NodeData of parentNode
                    auto& parentNode = socket->getParent();

                    // Check first if it is a constant node.
                    auto itConstant = m_constantNodes.find(parentNode.getId());
                    if(itConstant != m_constantNodes.end())
                    {
                        // nothing to check for constant nodes
                        return;
                    }

                    auto itParent = nodes.find(parentNode.getId());
                    EXECGRAPH_THROW_EXCEPTION_IF(itParent == nodes.end(),
                                                 "Node with id: " << parentNode.getId() << " has not been added to the execution tree!");
                    const NodeData& parentNodeData = itParent->second;

                    EXECGRAPH_THROW_EXCEPTION_IF(parentNodeData.m_priority <= nodeDataWithLowerPrio.m_priority,
                                                 "Parent node id: " << parentNodeData.m_node->getId() << "[prio: " << parentNodeData.m_priority << " ]"
                                                                    << "has not a higher priority as node id: "
                                                                    << nodeDataWithLowerPrio.m_node->getId()
                                                                    << " which is wrong!");
                };

                for(auto& pair : nodes)
                {
                    const NodeData& nodeData = pair.second;
                    // Follow all links
                    auto& inSockets = nodeData.m_node->getInputs();
                    for(auto& socket : inSockets)
                    {
                        // Try adding the get link to the stack
                        if(socket->hasGetLink())
                        {
                            check(socket->followGetLink(), nodeData);
                        }
                        // Try adding all writing links to the stack
                        for(auto* outputSocket : socket->getWritingSockets())
                        {
                            check(outputSocket, nodeData);
                        }
                    }
                }
            }

        private:
            LogicNodeDefaultOutputs* m_defaultOutputSockets;  //!< Default output sockets which are used for all dangling input sockets.
            ConstantNodeStorage& m_constantNodes;             //!< Constant nodes which do not need evalualtion.
        };

        //! The solver for computing the execution order.
        class ExecutionOrderSolver : public ExecutionSolverBase
        {
        public:
            ExecutionOrderSolver(NodeDataStorage& nodeMap,
                                 NodeDataSet& nodes,
                                 ConstantNodeStorage& constantNodes,
                                 LogicNodeDefaultOutputs* defaultOutputSockets = nullptr)
                : ExecutionSolverBase(constantNodes, defaultOutputSockets), m_nodeMap(nodeMap), m_nodes(nodes)
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
                    while(it != itEnd)
                    {
                        if(!(*it)->isFlagSet(NodeData::Visited))
                        {
                            // Not visited, backtracking finished!
                            // Remove the range [itStart,itLast) from the stack
                            break;
                        }
                        (*it)->unsetFlag(NodeData::OnCurrentDFRPath);  // Remove Mark
                        ++it;
                    }
                    EXECGRAPH_ASSERT(std::distance(itStart, it) <= static_cast<typename decltype(m_dfrStack)::difference_type>(m_dfrStack.size()),
                                     "Removing: " << std::distance(itStart, it) << " from " << m_dfrStack.size());
                    // Convert backward iterator to forward
                    m_dfrStack.erase(it.base(), itStart.base());
                };

                // Loop over all nodes and start a depth-first-search
                for(NodeData* nodeData : m_nodes)
                {
                    // If the node is visited we know that the node was contained in a depth-first recursion
                    // we know that the prioriteis below it are correct, so skip this one
                    if(nodeData->isFlagSet(NodeData::Visited))
                    {
                        EXECGRAPH_EXECTREE_SOLVER_LOG("DFS Start: Node id: " << nodeData->m_node->getId()
                                                                             << " already visited -> skip it."
                                                                             << std::endl;);
                        continue;
                    }

                    EXECGRAPH_EXECTREE_SOLVER_LOG("DFS Start: Node id: " << nodeData->m_node->getId());

                    // Start a depth-first recursion from this node (exploring its subtree)
                    m_dfrStack.clear();
                    // Insert root node
                    m_dfrStack.push_back(nodeData);

                    // Loop variables
                    NodeData* currentNode;
                    std::size_t currentSize;

                    while(!m_dfrStack.empty())
                    {
                        EXECGRAPH_EXECTREE_SOLVER_LOG("DFS Stack:" << getStackInfo() << std::endl);

                        currentNode = m_dfrStack.back();
                        currentSize = m_dfrStack.size();

                        // We are doing depth first search and try to visit a node which is already on the
                        // current DFR path.
                        EXECGRAPH_THROW_EXCEPTION_TYPE_IF(currentNode->isFlagSet(NodeData::OnCurrentDFRPath),
                                                          "Your execution logic graph contains a cycle! "
                                                          "Current traversal stack: "
                                                              << getTraversalInfo(),
                                                          ExecutionGraphCycleException);

                        visit(*currentNode);  // Visits neighbors and add them to m_dfrStack

                        // If no nodes have been added, down traversal is finished, we do now backtracking
                        if(currentSize == m_dfrStack.size())
                        {
                            doBackTracking();  // Removing all visited nodes up the stack
                        }
                    }
                }

                // Loop over all nodes and make priority sets:
                for(NodeData* nodeData : m_nodes)
                {
                    nodeData->resetTraversalParameters();

                    prioritiesGlobal[nodeData->m_priority].emplace_back(nodeData);
                    // Put the nodes into PrioritySets for each Group
                    for(auto& groupId : nodeData->m_groups)
                    {
                        prioritiesPerGroup[groupId][nodeData->m_priority].emplace_back(nodeData);
                    }

                    // Connect all dangling input sockets.
                    ExecutionSolverBase::connectAllDanglingInputs(*nodeData);
                }

                // Check execution order by checking all inputs of all nodes
                if(checkResults)
                {
                    this->checkResults(m_nodeMap);
                }
            }

        private:
            //! Return current DFS stack, but only nodes which are on the current DFR path.
            std::string getTraversalInfo()
            {
                std::stringstream ss;
                auto it = m_dfrStack.begin();
                if(it != m_dfrStack.end() && (*it)->isFlagSet(NodeData::OnCurrentDFRPath))
                {
                    ss << (*(it++))->m_node->getId();
                }
                for(; it != m_dfrStack.end(); ++it)
                {
                    if((*it)->isFlagSet(NodeData::OnCurrentDFRPath))
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
                for(auto* nodeData : m_dfrStack)
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
                EXECGRAPH_EXECTREE_SOLVER_LOG("visit: " << nodeData.m_node->getId());

                auto addParentsToStack = [&](auto* socket) {
                    // Get NodeData of parentNode
                    auto& parentNode = socket->getParent();
                    auto itParent    = m_nodeMap.find(parentNode.getId());
                    EXECGRAPH_THROW_EXCEPTION_IF(itParent == m_nodeMap.end(),
                                                 "Node with id: " << parentNode.getId() << " has not been added to the execution tree!");
                    NodeData* parentNodeData = &itParent->second;

                    if(parentNodeData->m_priority <= nodeData.m_priority)
                    {
                        // Parent needs a other priority (because its computation becomes before nodeData)
                        parentNodeData->m_priority = nodeData.m_priority + 1;
                        parentNodeData->unsetFlag(NodeData::Visited);
                        m_dfrStack.push_back(parentNodeData);  // Add to stack and explore its subgraph
                    }
                };

                // Follow all links
                auto& inSockets = nodeData.m_node->getInputs();
                for(auto& socket : inSockets)
                {
                    // Try adding the get link to the stack
                    if(socket->hasGetLink())
                    {
                        addParentsToStack(socket->followGetLink());
                    }
                    // Try adding all writing links to the stack
                    for(auto* outputSocket : socket->getWritingSockets())
                    {
                        addParentsToStack(outputSocket);
                    }
                }

                // Mark this node as on the current depth-first recursion path
                nodeData.setFlag(NodeData::OnCurrentDFRPath);

                // Mark this node as visited
                nodeData.setFlag(NodeData::Visited);
            }

            bool m_inputReachable     = false;
            NodeBaseType* m_reachNode = nullptr;
            NodeDataStorage& m_nodeMap;  //!< All NodeDatas of the execution tree.
            NodeDataSet& m_nodes;        //!< All NodeDatas of the execution tree.

            std::vector<NodeData*> m_dfrStack;  //!< Depth-First-Search Stack
        };

        //! First computes a topological order and then assigns priorities.
        class ExecutionOrderSolver2 : public ExecutionSolverBase
        {
        public:
            ExecutionOrderSolver2(NodeDataStorage& nodeMap,
                                  NodeDataSet& nodes,
                                  ConstantNodeStorage& constantNodes,
                                  LogicNodeDefaultOutputs* defaultOutputSockets = nullptr)
                : ExecutionSolverBase(constantNodes, defaultOutputSockets), m_nodeMap(nodeMap), m_nodes(nodes)
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
                    while(it != itEnd)
                    {
                        if(!(*it)->isFlagSet(NodeData::Visited))
                        {
                            // Not visited, backtracking finished!
                            // Remove the range [itStart,itLast) from the stack
                            break;
                        }

                        // If the node is marked unmark and add it to the topoSortList
                        if((*it)->isFlagSet(NodeData::OnCurrentDFRPath))
                        {
                            topoSortList.push_back(*it);
                            (*it)->unsetFlag(NodeData::OnCurrentDFRPath);
                        }

                        // Goto next node
                        ++it;
                    }
                    EXECGRAPH_ASSERT(std::distance(itStart, it) <= m_dfrStack.size(),
                                     "Removing: " << std::distance(itStart, it) << " from " << m_dfrStack.size());
                    // Convert backward iterator to forward
                    m_dfrStack.erase(it.base(), itStart.base());
                };

                // Loop over all nodes and start a depth-first-search
                for(NodeData* nodeData : m_nodes)
                {
                    EXECGRAPH_EXECTREE_SOLVER_LOG("DFS Start: Node id: " << nodeData->m_node->getId());
                    // Skip visited nodes.
                    if(nodeData->isFlagSet(NodeData::Visited))
                    {
                        continue;
                    }

                    // Start a depth-first recursion from this node (exploring this subtree)
                    m_dfrStack.clear();
                    // Insert root node
                    m_dfrStack.push_back(nodeData);

                    NodeData* currentNode;
                    std::size_t currentSize;

                    while(!m_dfrStack.empty())
                    {
                        EXECGRAPH_EXECTREE_SOLVER_LOG("DFS Stack:" << getStackInfo() << std::endl);

                        currentNode = m_dfrStack.back();

                        // We are doing depth first search and found another already visited node
                        // meaning we have a cycle.
                        EXECGRAPH_THROW_EXCEPTION_TYPE_IF(currentNode->isFlagSet(NodeData::OnCurrentDFRPath),
                                                          "Your execution logic graph contains a cycle! "
                                                              << "Current traversal stack: "
                                                              << getTraversalInfo(),
                                                          ExecutionGraphCycleException);

                        currentNode->setFlag(NodeData::OnCurrentDFRPath);
                        currentSize = m_dfrStack.size();
                        visit(*currentNode);  // Visits neighbors and add them to m_dfrStack

                        // If no nodes have been added, down traversal is finished, we do now backtracking
                        if(currentSize == m_dfrStack.size())
                        {
                            doBackTracking();  // Removing all visited nodes up the stack
                        }
                    }
                }

                // Topological sort finished
                // Traverse the sorted list from the back and assign the priorities

                auto rEndIt = topoSortList.rend();
                for(auto nodeDataIt = topoSortList.rbegin(); nodeDataIt != rEndIt; ++nodeDataIt)
                {
                    NodeData* nodeData = *nodeDataIt;
                    assignPrioritiesToChilds(*nodeData);
                    ExecutionSolverBase::connectAllDanglingInputs(*nodeData);
                }

                for(NodeData* nodeData : topoSortList)
                {
                    prioritiesGlobal[nodeData->m_priority].emplace_back(nodeData);
                    // Put the nodes into PrioritySets for each Group
                    for(auto& groupId : nodeData->m_groups)
                    {
                        prioritiesPerGroup[groupId][nodeData->m_priority].emplace_back(nodeData);
                    }
                }

                // Check execution order by checking all inputs of all nodes
                if(checkResults)
                {
                    this->checkResults(m_nodeMap);
                }
            }

        private:
            //! Return current DFS stack, but only nodes which are on the current DFR path.
            std::string getTraversalInfo()
            {
                std::stringstream ss;
                auto it = m_dfrStack.begin();
                if(it != m_dfrStack.end() && (*it)->isFlagSet(NodeData::OnCurrentDFRPath))
                {
                    ss << (*(it++))->m_node->getId();
                }
                for(; it != m_dfrStack.end(); ++it)
                {
                    if((*it)->isFlagSet(NodeData::OnCurrentDFRPath))
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
                for(auto* nodeData : m_dfrStack)
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
                EXECGRAPH_EXECTREE_SOLVER_LOG("visit: " << nodeData.m_node->getId());

                auto addParentsToStack = [&](auto* socket) {
                    // Get NodeData of parentNode
                    auto& parentNode = socket->getParent();
                    auto itParent    = m_nodeMap.find(parentNode.getId());
                    EXECGRAPH_THROW_EXCEPTION_IF(itParent == m_nodeMap.end(),
                                                 "Node with id: " << parentNode.getId() << " has not been added to the execution tree!");
                    NodeData* parentNodeData = &itParent->second;

                    if(!parentNodeData->isFlagSet(NodeData::Visited))
                    {
                        m_dfrStack.push_back(parentNodeData);  // Add to stack and explore its subgraph
                    }
                };

                // Follow all links
                auto& inSockets = nodeData.m_node->getInputs();
                for(auto& socket : inSockets)
                {
                    // Try adding the get link to the stack
                    if(socket->hasGetLink())
                    {
                        addParentsToStack(socket->followGetLink());
                    }
                    // Try adding all writing links to the stack
                    for(auto* outputSocket : socket->getWritingSockets())
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
                    EXECGRAPH_THROW_EXCEPTION_IF(itParent == m_nodeMap.end(),
                                                 "Node with id: " << parentNode.getId() << " has not been added to the execution tree!");
                    NodeData* parentNodeData = &itParent->second;

                    if(parentNodeData->m_priority <= nodeData.m_priority)
                    {
                        parentNodeData->m_priority = nodeData.m_priority + 1;
                    }
                };

                // Follow all links
                auto& inSockets = nodeData.m_node->getInputs();
                for(auto& socket : inSockets)
                {
                    // Try adding the get link to the stack
                    if(socket->hasGetLink())
                    {
                        assignToChild(socket->followGetLink());
                    }
                    // Try adding all writing links to the stack
                    for(auto* outputSocket : socket->getWritingSockets())
                    {
                        assignToChild(outputSocket);
                    }
                }
            }

            bool m_inputReachable     = false;
            NodeBaseType* m_reachNode = nullptr;
            NodeDataStorage& m_nodeMap;  //!< All NodeDatas of the execution tree.
            NodeDataSet& m_nodes;        //!< All NodeDatas of the execution tree.

            std::vector<NodeData*> m_dfrStack;  //!< Depth-First-Search Stack
        };

        //! Only for directed graphs, does not detect cycles -> endless loop!
        class ReachNodeCheck
        {
        public:
            // From end to start node
            bool check(NodeBaseType* endNode, NodeBaseType* startNode)
            {
                if(endNode == startNode)
                {
                    return true;
                }

                m_start   = startNode;
                m_reached = false;

                std::deque<NodeBaseType*> currentNodes;  // Breath first search
                currentNodes.push_back(endNode);

                // visit current front node, as long as currentNode list is not empty or
                // start has not yet been found!
                while(currentNodes.size() != 0 && m_reached == false)
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
                for(auto& socket : inSockets)
                {
                    if(socket->hasGetLink())
                    {
                        auto* outputSocket = socket->followGetLink();
                        auto& parentNode   = outputSocket->getParent();
                        // If we reached the start node, return!
                        if(m_start == &parentNode)
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

        NodeDataStorage m_nodeMap;            //!< All nodes in the execution tree (main storage) except for constant nodes.
        NodeDataSet m_allNodes;               //!< All nodes in the execution tree.
        ConstantNodeStorage m_constantNodes;  //!< All constant nodes which do not need evaluation and are also not part of the execution order.

        GroupNodeMap m_nodeGroups;           //!< The map of nodes in each group.
        PrioritySet m_execList;              //!< The global execution order.
        GroupExecutionList m_groupExecList;  //!< The execution order for each group.

        bool m_executionOrderUpToDate = false;  //!< Dirty flag which denotes that the execution order is not up to date!

        LogicNodeDefaultOutputs* m_nodeDefaultOutputPool;  //!< Default Pool with output sockets, to which all not connected input sockets are connected!
    };
}  // namespace executionGraph

#undef EXECGRAPH_EXECTREE_SOLVER_LOG
#endif  // ExecutionTreeInOut_hpp
