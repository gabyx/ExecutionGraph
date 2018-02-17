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

#ifndef APP_H
#define APP_H

#include <cef_app.h>
#include <memory>
#include <wrapper/cef_message_router.h>
#include "executionGraph/common/FileSystem.hpp"
class AppHandler;

// Implement application-level callbacks for the browser process.
class App : public CefApp,
            public CefBrowserProcessHandler
{
    IMPLEMENT_REFCOUNTING(App)

public:
    App(const std::path& clientSourcePath)
        : m_clientSourcePath(clientSourcePath)
    {
    }

    //! @name CefApp Methods
    //@{
    virtual CefRefPtr<CefBrowserProcessHandler> GetBrowserProcessHandler() override { return this; }
    virtual void OnRegisterCustomSchemes(CefRawPtr<CefSchemeRegistrar> registrar) override;
    //@}

    //! @name CefBrowserProcessHandler Methods
    //@{
    virtual void OnContextInitialized() override;
    //@}

private:
    void setupBrowser();
    CefRefPtr<AppHandler> m_appHandler;

private:
    std::shared_ptr<CefMessageRouterBrowserSide::Handler> m_messageDispatcher;

private:
    std::path m_clientSourcePath;  //!< Client source path.
};

#endif
