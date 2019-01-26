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

#pragma once

#include <cef_client.h>
#include <memory>
#include <wrapper/cef_message_router.h>

/* ---------------------------------------------------------------------------------------*/
/*!
    The CEF application handler.

    @todo Use composition instead of stupid virtual inheritance and putting all handling
    shit into this CefClient.

    @date Sun Feb 18 2018
    @author Gabriel Nützi, gnuetzi (at) gmail (døt) com
 */
/* ---------------------------------------------------------------------------------------*/
class AppHandler final : public CefClient,
                         public CefDisplayHandler,
                         public CefLifeSpanHandler,
                         public CefLoadHandler,
                         public CefContextMenuHandler
{
    IMPLEMENT_REFCOUNTING(AppHandler)

public:
    explicit AppHandler(std::shared_ptr<CefMessageRouterBrowserSide::Handler> requestDispatcher, bool useViews);
    ~AppHandler() = default;

    //! CefClient methods.
    //@{
    virtual CefRefPtr<CefDisplayHandler> GetDisplayHandler() override { return this; }
    virtual CefRefPtr<CefLifeSpanHandler> GetLifeSpanHandler() override { return this; }
    virtual CefRefPtr<CefLoadHandler> GetLoadHandler() override { return this; }
    virtual CefRefPtr<CefContextMenuHandler> GetContextMenuHandler() override { return this; }
    //@}

    virtual bool OnProcessMessageReceived(CefRefPtr<CefBrowser> browser,
                                          CefProcessId source_process,
                                          CefRefPtr<CefProcessMessage> message) override;
    //! CefDisplayHandler methods.
    //@{
    virtual void OnTitleChange(CefRefPtr<CefBrowser> browser,
                               const CefString& title) override;
    //@}

    //! CefLifeSpanHandler methods.
    //@{
    virtual void OnAfterCreated(CefRefPtr<CefBrowser> browser) override;
    virtual bool DoClose(CefRefPtr<CefBrowser> browser) override;
    virtual void OnBeforeClose(CefRefPtr<CefBrowser> browser) override;

    //! CefLoadHandler methods.
    //@{
    virtual void OnLoadError(CefRefPtr<CefBrowser> browser,
                             CefRefPtr<CefFrame> frame,
                             ErrorCode errorCode,
                             const CefString& errorText,
                             const CefString& failedUrl) override;
    //@}

    void CloseAllBrowsers(bool forceClose);
    void ShowDeveloperTools();
    void Reload();

private:
    // List of existing browser windows. Only accessed on the CEF UI thread.
    using BrowserList = std::vector<CefRefPtr<CefBrowser>>;
    BrowserList m_browserList;

private:
    CefRefPtr<CefMessageRouterBrowserSide> m_router;

private:
    std::shared_ptr<CefMessageRouterBrowserSide::Handler> m_requestDispatcher;  //! Dispatcher which is installed in the `m_router`.

private:
    //! True if the application is using the Views framework.
    const bool m_useViews;

public:
    bool IsClosing() const { return m_isClosing; }

private:
    bool m_isClosing;
};
