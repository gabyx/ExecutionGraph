// =========================================================================================
//  ExecutionGraph
//  Copyright (C) 2014 by Gabriel Nützi <gnuetzi (at) gmail (døt) com>
//
//  @date Sun Mar 11 2018
//  @author Gabriel Nützi, gnuetzi (at) gmail (døt) com
//
//  This Source Code Form is subject to the terms of the Mozilla Public
//  License, v. 2.0. If a copy of the MPL was not distributed with this
//  file, You can obtain one at http://mozilla.org/MPL/2.0/.
// =========================================================================================

#ifndef cefapp_RequestDispatcherAdapterCef_h
#define cefapp_RequestDispatcherAdapterCef_h

#include <cef_base.h>
#include <memory>
#include <wrapper/cef_message_router.h>
#include <executionGraph/common/Assert.hpp>
class BackendRequestDispatcher;

/* ---------------------------------------------------------------------------------------*/
/*!
    A copy-movable adapter for a CEF `MessageRouterBrowserSide::Handler` to forward 
    the message a `RequestDispatcher`.

    @date Sat Mar 10 2018
    @author Gabriel Nützi, gnuetzi (at) gmail (døt) com
 */
/* ---------------------------------------------------------------------------------------*/
class BackendRequestDispatcherAdapterCef final : public CefMessageRouterBrowserSide::Handler
{
public:
    explicit BackendRequestDispatcherAdapterCef(std::shared_ptr<BackendRequestDispatcher> dispatcher)
        : m_dispatcher(dispatcher)
    {
        EXECGRAPHGUI_ASSERT(m_dispatcher, "nullptr!");
    }

    virtual ~BackendRequestDispatcherAdapterCef() = default;

public:
    bool OnQuery(CefRefPtr<CefBrowser> browser,
                 CefRefPtr<CefFrame> frame,
                 int64 queryId,
                 const CefString& request,
                 bool persistent,
                 CefRefPtr<Callback> callback) override
    {
        CEF_REQUIRE_UI_THREAD();
        //todo wrap here
        return true;
    }

private:
    std::shared_ptr<BackendRequestDispatcher> m_dispatcher;  //!< The dispatcher to forward OnQuery requests.
};

#endif