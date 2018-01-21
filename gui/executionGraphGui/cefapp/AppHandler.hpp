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

#ifndef executionGraph_cefapp_AppHandler_H
#define executionGraph_cefapp_AppHandler_H

#include <cef_client.h>
#include <memory>
#include <unordered_map>
#include <vector>
#include <wrapper/cef_message_router.h>
#include "backend/Backend.hpp"
#include "cefapp/MessageHandler.hpp"

class AppHandler : public CefClient,
                   public CefDisplayHandler,
                   public CefLifeSpanHandler,
                   public CefLoadHandler
{
    IMPLEMENT_REFCOUNTING(AppHandler);

public:
    explicit AppHandler(bool use_views);
    ~AppHandler();

    // Provide access to the single global instance of this object.
    static AppHandler* GetInstance();

    // CefClient methods:
    virtual CefRefPtr<CefDisplayHandler> GetDisplayHandler() OVERRIDE
    {
        return this;
    }
    virtual CefRefPtr<CefLifeSpanHandler> GetLifeSpanHandler() OVERRIDE
    {
        return this;
    }
    virtual CefRefPtr<CefLoadHandler> GetLoadHandler() OVERRIDE { return this; }

    virtual bool OnProcessMessageReceived(CefRefPtr<CefBrowser> browser,
                                          CefProcessId source_process,
                                          CefRefPtr<CefProcessMessage> message) override;
    // CefDisplayHandler methods:
    virtual void OnTitleChange(CefRefPtr<CefBrowser> browser,
                               const CefString& title) OVERRIDE;

    // CefLifeSpanHandler methods:
    virtual void OnAfterCreated(CefRefPtr<CefBrowser> browser) OVERRIDE;
    virtual bool DoClose(CefRefPtr<CefBrowser> browser) OVERRIDE;
    virtual void OnBeforeClose(CefRefPtr<CefBrowser> browser) OVERRIDE;

    // CefLoadHandler methods:
    virtual void OnLoadError(CefRefPtr<CefBrowser> browser,
                             CefRefPtr<CefFrame> frame,
                             ErrorCode errorCode,
                             const CefString& errorText,
                             const CefString& failedUrl) OVERRIDE;

    // Request that all existing browser windows close.
    void CloseAllBrowsers(bool force_close);

    bool IsClosing() const { return m_isClosing; }

private:
    // Platform-specific implementation.
    void PlatformTitleChange(CefRefPtr<CefBrowser> browser,
                             const CefString& title);

    // True if the application is using the Views framework.
    const bool m_useViews;
    bool m_isClosing;

    // List of existing browser windows. Only accessed on the CEF UI thread.
    using BrowserList = std::vector<CefRefPtr<CefBrowser>>;
    BrowserList m_browserList;

    CefRefPtr<CefMessageRouterBrowserSide> m_router;

    //! Registering for Backends
    //@{
public:
    void RegisterBackend(const std::shared_ptr<Backend>& backend);
    std::shared_ptr<Backend> GetBackend(const Backend::Id& id) const;

private:
    std::unordered_map<Backend::Id, std::shared_ptr<Backend>> m_backends;
    //@}
};

#endif  // CEF_TESTS_CEFSIMPLE_SIMPLE_HANDLER_H_
