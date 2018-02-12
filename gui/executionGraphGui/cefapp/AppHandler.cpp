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
#include <base/cef_bind.h>
#include <cef_app.h>
#include <sstream>
#include <string>
#include <views/cef_browser_view.h>
#include <views/cef_window.h>
#include <wrapper/cef_closure_task.h>
#include <wrapper/cef_helpers.h>
#include "cefapp/PlatformTitleChanger.hpp"

namespace
{
    AppHandler* g_instance = nullptr;
}

AppHandler::AppHandler(std::shared_ptr<CefMessageRouterBrowserSide::Handler> messageDispatcher, bool useViews)
    : m_messageDispatcher(messageDispatcher), m_useViews(useViews), m_isClosing(false)
{
    DCHECK(!g_instance);
    g_instance = this;
}

AppHandler::~AppHandler()
{
    g_instance = nullptr;
}

// static
AppHandler* AppHandler::GetInstance()
{
    return g_instance;
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
        m_router->AddHandler(m_messageDispatcher.get(), true);
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
    BrowserList::iterator bit = m_browserList.begin();
    for(; bit != m_browserList.end(); ++bit)
    {
        if((*bit)->IsSame(browser))
        {
            m_browserList.erase(bit);
            break;
        }
    }

    if(m_router)
    {
        m_router->RemoveHandler(m_messageDispatcher.get());
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

void AppHandler::CloseAllBrowsers(bool force_close)
{
    if(!CefCurrentlyOn(TID_UI))
    {
        // Execute on the UI thread.
        CefPostTask(TID_UI, base::Bind(&AppHandler::CloseAllBrowsers, this, force_close));
        return;
    }

    if(m_browserList.empty())
        return;

    BrowserList::const_iterator it = m_browserList.begin();
    for(; it != m_browserList.end(); ++it)
        (*it)->GetHost()->CloseBrowser(force_close);
}
