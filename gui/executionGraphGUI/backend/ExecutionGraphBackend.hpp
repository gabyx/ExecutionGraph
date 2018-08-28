//! ========================================================================================
//!  ExecutionGraph
//!  Copyright (C) 2014 by Gabriel Nützi <gnuetzi (at) gmail (døt) com>
//!
//!  @date Sun Jan 14 2018
//!  @author Gabriel Nützi, gnuetzi (at) gmail (døt) com
//!
//!  This Source Code Form is subject to the terms of the Mozilla Public
//!  License, v. 2.0. If a copy of the MPL was not distributed with this
//!  file, You can obtain one at http://mozilla.org/MPL/2.0/.
//! ========================================================================================

#ifndef executionGraphGUI_backend_ExecutionGraphBackend_hpp
#define executionGraphGUI_backend_ExecutionGraphBackend_hpp

#include <array>
#include <chrono>
#include <string>
#include <variant>
#include <vector>
#include <rttr/type>
#include <executionGraph/common/Deferred.hpp>
#include <executionGraph/common/Identifier.hpp>
#include <executionGraph/common/Synchronized.hpp>
#include <executionGraph/graphs/ExecutionTreeInOut.hpp>
#include <executionGraph/serialization/GraphTypeDescription.hpp>
#include "executionGraphGUI/backend/Backend.hpp"
#include "executionGraphGUI/backend/ExecutionGraphBackendDefs.hpp"
#include "executionGraphGUI/common/RequestError.hpp"

/* ---------------------------------------------------------------------------------------*/
/*!
    The execution graph backend.

    @date Sun Feb 18 2018
    @author Gabriel Nützi, gnuetzi (at) gmail (døt) com
 */
/* ---------------------------------------------------------------------------------------*/
class ExecutionGraphBackend final : public Backend
{
    RTTR_ENABLE()

public:
    using DefaultGraph = executionGraph::ExecutionTreeInOut<executionGraph::GeneralConfig<>>;

    using Id                   = executionGraph::Id;
    using IdNamed              = executionGraph::IdNamed;
    using GraphTypeDescription = executionGraph::GraphTypeDescription;
    using NodeId               = executionGraph::NodeId;
    template<typename K, typename T>
    using SyncedUMap = Synchronized<std::unordered_map<K, T>>;

public:
    ExecutionGraphBackend()
        : Backend(IdNamed("ExecutionGraphBackend"))
    {
    }
    ~ExecutionGraphBackend() override = default;

    //! Adding/removing graphs.
    //@{
    Id addGraph(const Id& graphType);
    void removeGraph(const Id& graphId);
    void removeGraphs();
    //@}

    //! Adding/removing nodes.
    //@{
    template<typename ResponseCreator>
    void addNode(const Id& graphId,
                 const std::string& type,
                 const std::string& nodeName,
                 ResponseCreator&& responseCreator);

    void removeNode(const Id& graphId,
                    NodeId id);
    //@}

    //! Information about graphs.
    //@{
public:
private:
    static const std::array<IdNamed, 1> m_graphTypeDescriptionIds;                     //!< All IDs used for the graph description.
    static const std::unordered_map<Id, GraphTypeDescription> m_graphTypeDescription;  //!< All graph descriptions.

public:
    //! Get all graph descriptions identified by its id.
    const Systd::unordered_map<Id, GraphTypeDescription>& getGraphTypeDescriptions() const { return m_graphTypeDescription; }
    //@}

private:
    [[nodiscard]] Deferred initRequest(Id graphId);

private:
    class GraphStatus
    {
    private:
        using Mutex             = std::mutex;
        using Lock              = std::lock_guard<Mutex>;
        using ConditionVariable = std::condition_variable;

    public:
        void isRequestHandlingEnabled() const { return m_requestHandlingEnabled; }
        void setRequestHandlingEnabled(bool enabled) { m_requestHandlingEnabled = enabled; }

    private:
        std::atomic<bool> m_requestHandlingEnabled = true;

    public:
        std::size_t getRequestCount() const
        {
            Lock lock(m_requestCountMutex);
            return m_requestCount;
        }

        void incrementRequestCount()
        {
            Lock lock(m_requestCountMutex);
            return ++m_requestCount;
        };
        void decrementRequestCount()
        {
            bool singleRequest = false;
            {  // Locking start
                Lock lock(m_requestCountMutex);
                if(m_requestCount)
                {
                    --m_requestCount;
                }
                singleRequest = m_requestCount == 1;
            }  // Locking end

            if(singleRequest)
            {
                m_onlySingleRequest.notify_all();  // Notify all waiting threads, that this graph has one single request
            }
        };

        //! Wait until the request count is zero, or timeout.
        //! @param Return the lock, such that no one can change the request count and
        //! the bool indicating if the request count is zero!
        template<typename Duration>
        std::pair<std::unique_lock<std::mutex>, bool>
        waitUntilOnlySingleRequests(const Duration& timeout = std::chrono::seconds(10))
        {
            std::unique_lock<std::mutex> lock(m_requestCountMutex);
            bool singleRequest = m_onlySingleRequest.wait_for(lock,
                                                              timeout,
                                                              [&]() { return m_requestCount == 1; });
            return {std::move(lock), singleRequest};
        }

    private:
        ConditionVariable m_onlySingleRequest;  //!< Condition variable indicating: request count == 1
        Mutex m_requestCountMutex;              //!< The mutex for the request count
        std::size_t m_requestCount = 0;         //!< The number of simultanously handling requests.
    };

    using GraphVariant = std::variant<std::shared_ptr<Synchronized<DefaultGraph>>>;

private:
    SyncedUMap<Id, GraphVariant> m_graphs;                  //! Graphs identified by its id.
    SyncedUMap<Id, std::shared_ptr<GraphStatus>> m_status;  //! Graph status for each graph id.
    //SyncedUMap<Id, std::shared_ptr<IGraphIExecutor> > m_executor; //!< Graph executors for each graph id.
};

//! Add a node with type `type` to the graph with id `graphId`.
template<typename ResponseCreator>
void ExecutionGraphBackend::addNode(const Id& graphId,
                                    const std::string& type,
                                    const std::string& nodeName,
                                    ResponseCreator&& responseCreator)
{
    abortIfHandlingDisabled(graphId);

    GraphVariant graphVar;

    m_graphs.withlock([&](auto& graphs) {
        auto graphIt = graphs.find(graphId);
        EXECGRAPHGUI_THROW_BAD_REQUEST_IF(graphIt == graphs.cend(),
                                          "Graph id: '{0}' does not exist!",
                                          graphId.toString());
        graphVar = graphIt->second;
    });

    // Remark: Here somebody could potentially call `removeGraph` (other thread)
    // which would remove this GraphVariant... and we carry on here adding a node
    // its not optimal but still safe.

    // Make a visitor to dispatch the "add" over the variant...
    auto add = [&](auto& graph) {
        using GraphType    = std::remove_cv_t<std::remove_reference_t<typename decltype(*graph)>>;
        using Config       = typename GraphType::Config;
        using NodeBaseType = typename Config::NodeBaseType;

        // Construct the node with the serializer
        typename ExecutionGraphBackendDefs<Config>::NodeSerializer serializer;
        NodeId id          = graph.generateNodeId();
        NodeBaseType* node = nullptr;

        // Locking start
        auto graphLock = graph->wlock();
        auto& graph    = *graphL;
        try
        {
            auto n = serializer.read(type, id, nodeName);
            node   = graphL->addNode(std::move(n));
        }
        catch(executionGraph::Exception& e)
        {
            EXECGRAPHGUI_THROW_BAD_REQUEST("Construction of node '{0}' with type: '{1}' for graph id '{2}' failed: '{3}'",
                                           nodeName,
                                           type,
                                           graphId.toString(),
                                           e.what());
        }

        EXECGRAPH_ASSERT(node != nullptr, "Node is nullptr!!?");

        // Create the response
        responseCreator(graph, *node);

        // Locking end
    };

    std::visit(add, graphVar);
}

#endif
