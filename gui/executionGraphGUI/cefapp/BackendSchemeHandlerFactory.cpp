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

#include "BackendSchemeHandlerFactory.hpp"
#include <memory>
#include <foonathan/memory/heap_allocator.hpp>
#include <foonathan/memory/smart_ptr.hpp>
#include "executionGraphGUI/cefapp/BackendRequestDispatcher.hpp"
#include "executionGraphGUI/cefapp/BackendResourceHandler.hpp"
#include "executionGraphGUI/common/BufferPool.hpp"
#include "executionGraphGUI/common/Loggers.hpp"

using namespace foonathan::memory;

BackendSchemeHandlerFactory::BackendSchemeHandlerFactory(std::shared_ptr<BackendRequestDispatcher> dispatcher,
                                                         const std::path& pathPrefix)
    : m_pathPrefix(pathPrefix)
    , m_dispatcher(dispatcher)
    , m_bufferPool(std::make_shared<BufferPool>())
    , m_handlerPool(sizeof(BackendResourceHandler), sizeof(BackendResourceHandler) * 30)
{
}

CefRefPtr<CefResourceHandler> BackendSchemeHandlerFactory::Create(CefRefPtr<CefBrowser> browser,
                                                                  CefRefPtr<CefFrame> frame,
                                                                  const CefString& scheme_name,
                                                                  CefRefPtr<CefRequest> request)
{
    CEF_REQUIRE_IO_THREAD();

    // Handback a new Handler (use an efficient memory pool)
    using Type         = BackendResourceHandler;
    using RawAllocator = decltype(m_handlerPool);
    using RawPtr       = std::unique_ptr<Type, allocator_deallocator<Type, RawAllocator>>;

    // get the memory from the pool
    auto memory = m_handlerPool.allocate_node();
    // using RawPtr = std::unique_ptr<Type, allocator_deallocator<Type, RawAllocator>>;
    // raw_ptr deallocates memory in case of constructor exception
    RawPtr result(static_cast<BackendResourceHandler*>(memory), {m_handlerPool});
    // call constructor (placement new)
    ::new(memory) BackendResourceHandler(m_dispatcher, m_bufferPool, [this](auto* ptr) { allocator_deleter<Type, RawAllocator>{m_handlerPool}(ptr); });

    return result.release();
}
