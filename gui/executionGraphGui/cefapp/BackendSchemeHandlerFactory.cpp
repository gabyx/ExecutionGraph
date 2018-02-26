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
#include <executionGraph/common/Assert.hpp>
#include <executionGraph/common/Exception.hpp>
#include <foonathan/memory/heap_allocator.hpp>
#include <foonathan/memory/smart_ptr.hpp>
#include <memory>
#include <random>
#include "cefapp/Loggers.hpp"
#include "cefapp/SchemeHandlerHelper.hpp"

using namespace foonathan::memory;

CefRefPtr<CefResourceHandler> BackendSchemeHandlerFactory::Create(CefRefPtr<CefBrowser> browser,
                                                                  CefRefPtr<CefFrame> frame,
                                                                  const CefString& scheme_name,
                                                                  CefRefPtr<CefRequest> request)
{
    CEF_REQUIRE_IO_THREAD();

    // {
    //     using Type         = BackendResourceHandler;
    //     using RawAllocator = memory_pool<>;
    //     using Ptr = std::unique_ptr<Type, allocator_deleter<Type, RawAllocator>>;
    //     using RawPtr = std::unique_ptr<Type, allocator_deallocator<Type, RawAllocator>>;
    //     std::vector<CefRefPtr<CefResourceHandler>> vec;

    //     std::vector<Ptr> vec2;
    //     for(auto i = 0; i < 3; i++)
    //     {

    //         // get the memory from the pool
    //         auto memory = m_handlerPool.allocate_node(/*sizeof(Type), alignof(Type)*/);

    //         using RawPtr = std::unique_ptr<Type, allocator_deallocator<Type, RawAllocator>>;
    //         // raw_ptr deallocates memory in case of constructor exception
    //         RawPtr result(static_cast<Type*>(memory), {m_handlerPool});
    //         // call constructor (placement new)
    //         ::new(memory) Type(m_bufferPool, [this](auto* ptr) { allocator_deleter<Type, RawAllocator>{m_handlerPool}(ptr); });
    //         // pass ownership to return value CefRefPtr which will use the internal BackendResourceHandler::m_deleter
    //         EXECGRAPHGUI_APPLOG_INFO("Allocated node: {0}", i);
    //         vec.emplace_back(result.release());
    //     }
    //     std::shuffle(vec.begin(), vec.end(), std::mt19937{});
    // }

    return new BackendResourceHandler{m_bufferPool, [](auto* p) { delete p; }};
}
