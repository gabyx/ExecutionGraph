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

#include "AppHandler.hpp"
#include <cef_app.h>
#include <sstream>
#include <string>
#include <base/cef_bind.h>
#include <views/cef_browser_view.h>
#include <views/cef_window.h>
#include <wrapper/cef_closure_task.h>
#include <wrapper/cef_helpers.h>
#include "executionGraphGUI/cefapp/PlatformTitleChanger.hpp"
#include "executionGraphGUI/common/DevFlags.hpp"
#include "executionGraphGUI/common/Loggers.hpp"

namespace
{
    class DevToolsClient : public CefClient
    {
        IMPLEMENT_REFCOUNTING(DevToolsClient)
    };

    //! Show the Developer Tools
    bool showDevTools(CefRefPtr<CefBrowser> browser)
    {
        CefWindowInfo windowInfo;
        CefBrowserSettings settings;
#if defined(OS_WIN)
        windowInfo.SetAsPopup(browser->GetHost()->GetWindowHandle(), "DevTools");
#endif
        CefRefPtr<CefClient> client(std::make_unique<DevToolsClient>().release());
        browser->GetHost()->ShowDevTools(windowInfo, client, settings, CefPoint{});
        return true;
    }

}  // namespace

AppHandler::AppHandler(std::shared_ptr<CefMessageRouterBrowserSide::Handler> requestDispatcher, bool useViews)
    : m_requestDispatcher(requestDispatcher), m_useViews(useViews), m_isClosing(false)
{
}

void AppHandler::OnTitleChange(CefRefPtr<CefBrowser> browser,
                               const CefString& title)
{
    CEF_REQUIRE_UI_THREAD();

    if(m_useViews)
    {
        // Set the title of the window using the Views framework.
        CefRefPtr<CefBrowserView> browser_view =
            CefBrowserView::GetForBrowser(browser);
        if(browser_view)
        {
            CefRefPtr<CefWindow> window = browser_view->GetWindow();
            if(window)
                window->SetTitle(title);
        }
    }
    else
    {
        // Set the title of the window using platform APIs.
        PlatformTitleChanger::OnTitleChange(browser, title);
    }
}

void AppHandler::OnAfterCreated(CefRefPtr<CefBrowser> browser)
{
    CEF_REQUIRE_UI_THREAD();

    // Add to the list of existing browsers.
    m_browserList.push_back(browser);

    if(!m_router)
    {
        CefMessageRouterConfig config;
        m_router = CefMessageRouterBrowserSide::Create(config);
        m_router->AddHandler(m_requestDispatcher.get(), true);
    }

    if(devFlags::showDevTools)
    {
        showDevTools(browser);
    }
}

bool AppHandler::OnProcessMessageReceived(CefRefPtr<CefBrowser> browser,
                                          CefProcessId source_process,
                                          CefRefPtr<CefProcessMessage> message)
{
    CEF_REQUIRE_UI_THREAD();
    DCHECK(m_router);
    return m_router->OnProcessMessageReceived(browser, source_process, message);
}

bool AppHandler::DoClose(CefRefPtr<CefBrowser> browser)
{
    CEF_REQUIRE_UI_THREAD();

    // Closing the main window requires special handling. See the DoClose()
    // documentation in the CEF header for a detailed destription of this
    // process.
    if(m_browserList.size() == 1)
    {
        // Set a flag to indicate that the window close should be allowed.
        m_isClosing = true;
    }

    // Allow the close. For windowed browsers this will result in the OS close
    // event being sent.
    return false;
}

void AppHandler::OnBeforeClose(CefRefPtr<CefBrowser> browser)
{
    CEF_REQUIRE_UI_THREAD();

    // Remove from the list of existing browsers.
    auto bIt = m_browserList.begin();
    for(; bIt != m_browserList.end(); ++bIt)
    {
        if((*bIt)->IsSame(browser))
        {
            bIt = m_browserList.erase(bIt);
            break;
        }
    }

    if(m_router)
    {
        m_router->RemoveHandler(m_requestDispatcher.get());
    }

    if(m_browserList.empty())
    {
        // All browser windows have closed. Quit the application message loop.
        CefQuitMessageLoop();
    }
}

void AppHandler::OnLoadError(CefRefPtr<CefBrowser> browser,
                             CefRefPtr<CefFrame> frame,
                             ErrorCode errorCode,
                             const CefString& errorText,
                             const CefString& failedUrl)
{
    CEF_REQUIRE_UI_THREAD();

    // Don't display an error for downloaded files.
    if(errorCode == ERR_ABORTED)
        return;

    // Display a load error message.
    std::stringstream ss;
    ss << "<html><body bgcolor=\"white\">"
          "<h2>Failed to load URL "
       << std::string(failedUrl) << " with error " << std::string(errorText)
       << " (" << errorCode << ").</h2></body></html>";
    frame->LoadString(ss.str(), failedUrl);
}

//! Request that all existing browser windows close.
void AppHandler::CloseAllBrowsers(bool forceClose)
{
    if(!CefCurrentlyOn(TID_UI))
    {
        // Execute on the UI thread.
        CefPostTask(TID_UI, base::Bind(&AppHandler::CloseAllBrowsers, this, forceClose));
        return;
    }

    if(m_browserList.empty())
        return;

    BrowserList::const_iterator it = m_browserList.begin();
    for(; it != m_browserList.end(); ++it)
        (*it)->GetHost()->CloseBrowser(forceClose);
}

//! Show Developer Tools for every browser
void AppHandler::ShowDeveloperTools()
{
    for(auto browser : m_browserList)
    {
        showDevTools(browser);
    }
}
