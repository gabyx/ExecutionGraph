// ========================================================================================
//  ExecutionGraph
//  Copyright (C) 2014 by Gabriel Nützi <gnuetzi (at) gmail (døt) com>
//
//  This Source Code Form is subject to the terms of the Mozilla Public
//  License, v. 2.0. If a copy of the MPL was not distributed with this
//  file, You can obtain one at http://mozilla.org/MPL/2.0/.
// ========================================================================================

#ifndef RENDERER_APP_H_
#define RENDERER_APP_H_

#include <cef_app.h>

#include <cef_render_process_handler.h>
#include <wrapper/cef_message_router.h>

class RendererApp : public CefApp, public CefRenderProcessHandler
{
    IMPLEMENT_REFCOUNTING(RendererApp);

public:
    //! @name CefApp Methods
    //@{
    virtual CefRefPtr<CefRenderProcessHandler> GetRenderProcessHandler() override
    {
        return this;
    }
    //@}

    ///! @name CefRenderProcessHandler Methods
    //@{
    virtual void OnWebKitInitialized() override;

    virtual void OnContextCreated(CefRefPtr<CefBrowser> browser,
                                  CefRefPtr<CefFrame> frame,
                                  CefRefPtr<CefV8Context> context) override;

    virtual void OnContextReleased(CefRefPtr<CefBrowser> browser,
                                   CefRefPtr<CefFrame> frame,
                                   CefRefPtr<CefV8Context> context) override;

    virtual bool OnProcessMessageReceived(CefRefPtr<CefBrowser> browser,
                                          CefProcessId source_process,
                                          CefRefPtr<CefProcessMessage> message) override;
    //@}

private:
    CefRefPtr<CefMessageRouterRendererSide> m_router;
};

#endif
