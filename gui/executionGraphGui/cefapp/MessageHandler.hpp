// ========================================================================================
//  ExecutionGraph
//  Copyright (C) 2014 by Gabriel Nützi <gnuetzi (at) gmail (døt) com>
//
//  This Source Code Form is subject to the terms of the Mozilla Public
//  License, v. 2.0. If a copy of the MPL was not distributed with this
//  file, You can obtain one at http://mozilla.org/MPL/2.0/.
// ========================================================================================

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
