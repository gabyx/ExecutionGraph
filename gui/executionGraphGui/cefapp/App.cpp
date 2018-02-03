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

#include "App.hpp"

#include <cef_browser.h>
#include <cef_command_line.h>
#include <string>
#include <views/cef_browser_view.h>
#include <views/cef_window.h>
#include <wrapper/cef_helpers.h>
#include "backend/ExecutionGraphBackend.hpp"
#include "cefapp/AppHandler.hpp"
#include "cefapp/FileSchemeHandlerFactory.hpp"

namespace
{
    // When using the Views framework this object provides the delegate
    // implementation for the CefWindow that hosts the Views-based browser.
    class SimpleWindowDelegate : public CefWindowDelegate
    {
    public:
        explicit SimpleWindowDelegate(CefRefPtr<CefBrowserView> browser_view)
            : browser_view_(browser_view) {}

        void OnWindowCreated(CefRefPtr<CefWindow> window) OVERRIDE
        {
            // Add the browser view and show the window.
            window->AddChildView(browser_view_);
            window->Show();

            // Give keyboard focus to the browser view.
            browser_view_->RequestFocus();
        }

        void OnWindowDestroyed(CefRefPtr<CefWindow> window) OVERRIDE
        {
            browser_view_ = NULL;
        }

        bool CanClose(CefRefPtr<CefWindow> window) OVERRIDE
        {
            // Allow the window to close if the browser says it's OK.
            CefRefPtr<CefBrowser> browser = browser_view_->GetBrowser();
            if(browser)
                return browser->GetHost()->TryCloseBrowser();
            return true;
        }

    private:
        CefRefPtr<CefBrowserView> browser_view_;

        IMPLEMENT_REFCOUNTING(SimpleWindowDelegate);
        DISALLOW_COPY_AND_ASSIGN(SimpleWindowDelegate);
    };

}  // namespace

void App::OnContextInitialized()
{
    CEF_REQUIRE_UI_THREAD();

    CefRefPtr<CefCommandLine> command_line = CefCommandLine::GetGlobalCommandLine();

#if defined(OS_WIN) || defined(OS_LINUX)
    // Create the browser using the Views framework if "--use-views" is specified
    // via the command-line. Otherwise, create the browser using the native
    // platform framework. The Views framework is currently only supported on
    // Windows and Linux.
    const bool use_views = command_line->HasSwitch("use-views");
#else
    const bool use_views = false;
#endif

    CefRegisterSchemeHandlerFactory("client",
                                    "executionGraph",
                                    new FileSchemeHandlerFactory(m_clientSourcePath, "executionGraph"));

    // AppHandler implements browser-level callbacks.
    m_appHandler = CefRefPtr<AppHandler>(new AppHandler(use_views));

    // Specify CEF browser settings here.
    CefBrowserSettings browser_settings;
    CefString url = "client://executionGraph/index.html";

    if(use_views)
    {
        // Create the BrowserView.
        CefRefPtr<CefBrowserView> browser_view = CefBrowserView::CreateBrowserView(m_appHandler, url, browser_settings, NULL, NULL);

        // Create the Window. It will show itself after creation.
        CefWindow::CreateTopLevelWindow(new SimpleWindowDelegate(browser_view));
    }
    else
    {
        // Information used when creating the native window.
        CefWindowInfo window_info;

#if defined(OS_WIN)
        // On Windows we need to specify certain flags that will be passed to
        // CreateWindowEx().
        window_info.SetAsPopup(NULL, "executionGraphGui");
#endif

        // Create the first browser window.
        CefBrowserHost::CreateBrowser(window_info, m_appHandler, url, browser_settings, NULL);
    }
}
