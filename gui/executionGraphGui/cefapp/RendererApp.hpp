#ifndef RENDERER_APP_H_
#define RENDERER_APP_H_

#include <cef_app.h>

#include <cef_render_process_handler.h>
#include <wrapper/cef_message_router.h>

class RendererApp : public CefApp, public CefRenderProcessHandler
{
public:

    // CefApp methods:
    virtual CefRefPtr<CefRenderProcessHandler> GetRenderProcessHandler() override {
        return this;
    }

    // CefRenderProcessHandler methods:
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
private:
    CefRefPtr<CefMessageRouterRendererSide> router;

private:
    // Include the default reference counting implementation.
    IMPLEMENT_REFCOUNTING(RendererApp);
};

#endif //RENDERER_APP_H_
