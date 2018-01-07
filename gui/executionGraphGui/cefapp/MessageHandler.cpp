#include "MessageHandler.hpp"

bool MessageHandler::OnQuery(CefRefPtr<CefBrowser> browser,
                             CefRefPtr<CefFrame> frame,
                             int64 query_id,
                             const CefString& request,
                             bool persistent,
                             CefRefPtr<Callback> callback)
{
    std::string result = request;
    std::reverse(result.begin(), result.end());
    callback->Success(result);
    return true;
}
