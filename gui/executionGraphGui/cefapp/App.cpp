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
#include <cef_origin_whitelist.h>
#include <string>
#include <views/cef_browser_view.h>
#include <views/cef_window.h>
#include <wrapper/cef_helpers.h>
#include "executionGraphGui/backend/BackendFactory.hpp"
#include "executionGraphGui/backend/ExecutionGraphBackend.hpp"
#include "executionGraphGui/cefapp/AppHandler.hpp"
#include "executionGraphGui/cefapp/BackendRequestDispatcher.hpp"
#include "executionGraphGui/cefapp/BackendRequestDispatcherAdapterCef.hpp"
#include "executionGraphGui/cefapp/BackendResourceHandler.hpp"
#include "executionGraphGui/cefapp/BackendSchemeHandlerFactory.hpp"
#include "executionGraphGui/cefapp/ClientSchemeHandlerFactory.hpp"
#include "executionGraphGui/cefapp/SchemeHandlerHelper.hpp"
#include "executionGraphGui/common/Exception.hpp"

namespace
{
    // When using the Views framework this object provides the delegate
    // implementation for the CefWindow that hosts the Views-based browser.
    class SimpleWindowDelegate : public CefWindowDelegate
    {
    public:
        explicit SimpleWindowDelegate(CefRefPtr<CefBrowserView> browserView)
            : browser_view_(browserView) {}

        void OnWindowCreated(CefRefPtr<CefWindow> window) override
        {
            // Add the browser view and show the window.
            window->AddChildView(browser_view_);
            window->Show();

            // Give keyboard focus to the browser view.
            browser_view_->RequestFocus();
        }

        void OnWindowDestroyed(CefRefPtr<CefWindow> window) override
        {
            browser_view_ = nullptr;
        }

        bool CanClose(CefRefPtr<CefWindow> window) override
        {
            // Allow the window to close if the browser says it's OK.
            CefRefPtr<CefBrowser> browser = browser_view_->GetBrowser();
            if(browser)
                return browser->GetHost()->TryCloseBrowser();
            return true;
        }

    private:
        CefRefPtr<CefBrowserView> browser_view_;

        IMPLEMENT_REFCOUNTING(SimpleWindowDelegate)
        DISALLOW_COPY_AND_ASSIGN(SimpleWindowDelegate);
    };

    //! Install the client
    void setupClient(const std::string& clientSourcePath)
    {
        // Register the URL Scheme handler for the client (angular application)
        CefRegisterSchemeHandlerFactory("client",
                                        "executiongraph",
                                        new ClientSchemeHandlerFactory(clientSourcePath));
    }

    //! Install various backends and setup all of them.
    template<typename Dispatcher>
    void setupBackends(std::shared_ptr<Dispatcher> requestDispatcher)
    {
        // Install the URL RequestHandler for the backend
        CefRefPtr<BackendSchemeHandlerFactory> backendSchemeHandlerFactory(new BackendSchemeHandlerFactory(requestDispatcher));
        // A custom scheme receiving post data does not work
        // CefRegisterSchemeHandlerFactory("backend",
        //                                 "executiongraph-backend",
        //                                 backendSchemeHandlerFactory);
        CefRegisterSchemeHandlerFactory("http",
                                        "executiongraph-backend",
                                        backendSchemeHandlerFactory);
        // So far an own scheme does not work:
        // WebKit does not pass POST data to the request for synchronous XHRs executed on non-HTTP schemes.
        // See the m\_url.protocolInHTTPFamily() https://bitbucket.org/chromiumembedded/cef/issues/404
        // however we only uses asynchronous XHR requests... ?
        // CefAddCrossOriginWhitelistEntry("client://executiongraph", "backend", "", true);
        // only needed if we use the scheme "backend://" to allow CORS

        // Install the executionGraph backend
        BackendFactory::BackendData messageHandlers = BackendFactory::Create<ExecutionGraphBackend>();

        for(auto& backendHandler : messageHandlers.second)
        {
            requestDispatcher->addHandler(backendHandler);
        }
    }

    //! Setup tht CEF Browser
    template<typename Dispatcher>
    CefRefPtr<AppHandler> setupBrowser(std::shared_ptr<Dispatcher> requestDispatcher,
                                       const std::string& clientLoadUrl)
    {
        CefRefPtr<CefCommandLine> command_line = CefCommandLine::GetGlobalCommandLine();

#if defined(OS_WIN) || defined(OS_LINUX)
        // Create the browser using the Views framework if "--use-views" is specified
        // via the command-line. Otherwise, create the browser using the native
        // platform framework. The Views framework is currently only supported on
        // Windows and Linux.
        const bool useViews = command_line->HasSwitch("use-views");
#else
        const bool useViews = false;
#endif

        // AppHandler implements browser-level callbacks.
        auto dispatcherAdapter = std::make_shared<BackendRequestDispatcherAdapterCef>(requestDispatcher);
        CefRefPtr<AppHandler> appHandler(std::make_unique<AppHandler>(dispatcherAdapter, useViews).release());

        // Specify CEF browser settings here.
        CefBrowserSettings browserSettings;
        // Disable security, such that http:// XHRequests do not trigger a CORS Preflight Request (if special headers are used)
        browserSettings.web_security = cef_state_t::STATE_DISABLED;
        CefString url                = clientLoadUrl;

        if(useViews)
        {
            // Create the BrowserView.
            CefRefPtr<CefBrowserView> browserView = CefBrowserView::CreateBrowserView(appHandler, url, browserSettings, nullptr, nullptr);

            // Create the Window. It will show itself after creation.
            CefWindow::CreateTopLevelWindow(new SimpleWindowDelegate(browserView));
        }
        else
        {
            // Information used when creating the native window.
            CefWindowInfo windowInfo;

#if defined(OS_WIN)
            // On Windows we need to specify certain flags that will be passed to
            // CreateWindowEx().
            windowInfo.SetAsPopup(nullptr, "executionGraphGui");
#endif

            // Create the first browser window.
            CefBrowserHost::CreateBrowser(windowInfo, appHandler, url, browserSettings, nullptr);
        }

        return appHandler;
    }

}  // namespace

void App::OnContextInitialized()
{
    CEF_REQUIRE_UI_THREAD();

    // Setup the client
    setupClient(m_clientSourcePath);

    // Make a global message dispatcher
    auto requestDispatcher = std::make_shared<BackendRequestDispatcher>();
    requestDispatcher->start();  // start the dispatcher thread

    // Setup the backends
    setupBackends(requestDispatcher);

    // Setup the browser
    m_appHandler = setupBrowser(requestDispatcher, m_clientLoadUrl);
}

void App::OnRegisterCustomSchemes(CefRawPtr<CefSchemeRegistrar> registrar)
{
    schemeHandlerHelper::registerCustomSchemes(registrar);
}
