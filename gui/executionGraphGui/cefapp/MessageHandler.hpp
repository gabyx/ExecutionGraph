#ifndef MESSAGE_HANDLER_H_
#define MESSAGE_HANDLER_H_

#include <wrapper/cef_message_router.h>

// Handle messages in the browser process.
class MessageHandler : public CefMessageRouterBrowserSide::Handler
{
public:
    explicit MessageHandler()
    {
    }

    // Called due to cefQuery execution in message_router.html.
    virtual bool OnQuery(CefRefPtr<CefBrowser> browser,
                         CefRefPtr<CefFrame> frame,
                         int64 query_id,
                         const CefString& request,
                         bool persistent,
                         CefRefPtr<Callback> callback) override;

private:
    DISALLOW_COPY_AND_ASSIGN(MessageHandler);
};

#endif  // MESSAGE_HANDLER_H_
