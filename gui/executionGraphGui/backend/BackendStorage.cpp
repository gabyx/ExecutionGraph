#include "backend/BackendStorage.hpp"
#include <executionGraph/common/Exception.hpp>
#include <rttr/type>
#include "backend/Backend.hpp"
#include "backend/BackendMessageHandler.hpp"
#include "backend/BackendMessageHandlerFactory.hpp"

/*! 
    Register the backend `backend`:
    - Hold it as owner
    - Register/Unregister all its corresponding message handlers.
*/
void BackendStorage::RegisterBackend(const std::shared_ptr<Backend>& backend)
{
    m_backends.emplace(backend->getId(), backend);
}

//! Get the backend with id `id`.
std::shared_ptr<Backend> BackendStorage::GetBackend(const Id& id) const
{
    auto it = m_backends.find(id);
    return (it != m_backends.end()) ? it->second : nullptr;
}

void BackendStorage::RegisterHandlersAtRouter(CefRefPtr<CefMessageRouterBrowserSide> router)
{
    EXECGRAPH_THROW_EXCEPTION_IF(!router, "No router!");

    // Register all message handlers for the backends
    for(auto& p : m_backends)
    {
        auto& id         = p.first;
        Backend& backend = *p.second;

        auto type            = rttr::type::get(backend);
        auto messageHandlers = BackendMessageHandlerFactory::Create(type);
        if(messageHandlers.size())
        {
            auto result = m_backendHandlers.emplace(id, messageHandlers);
            EXECGRAPH_THROW_EXCEPTION_IF(!result.second, "already added!");  // exception if no insertion!

            // Register all created handlers
            for(std::shared_ptr<BackendMessageHandler> handler : result.first->second)
            {
                router->AddHandler(handler.get(), true);
            }
        }
    }
}

void BackendStorage::UnregisterHandlersFromRouter(CefRefPtr<CefMessageRouterBrowserSide> router)
{
    EXECGRAPH_THROW_EXCEPTION_IF(!router, "No router!");

    // Uninstall all message handlers for all backends
    for(auto& p : m_backends)
    {
        auto& id = p.first;
        // Unregister all created handlers
        for(std::shared_ptr<BackendMessageHandler> handler : m_backendHandlers[id])
        {
            router->RemoveHandler(static_cast<CefMessageRouterBrowserSide::Handler*>(handler.get()));
        }
    }
}