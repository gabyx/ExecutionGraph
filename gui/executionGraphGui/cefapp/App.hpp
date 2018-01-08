// ========================================================================================
//  ExecutionGraph
//  Copyright (C) 2014 by Gabriel Nützi <gnuetzi (at) gmail (døt) com>
//
//  Created by Gabriel Nützi, Mon Jan 08 2018
//
//  This Source Code Form is subject to the terms of the Mozilla Public
//  License, v. 2.0. If a copy of the MPL was not distributed with this
//  file, You can obtain one at http://mozilla.org/MPL/2.0/.
// ========================================================================================

#ifndef APP_H
#define APP_H

#include <cef_app.h>

// Implement application-level callbacks for the browser process.
class App : public CefApp,
            public CefBrowserProcessHandler
{
    IMPLEMENT_REFCOUNTING(App);

public:
    App() {}

    //! @name CefApp Methods
    //@{
    virtual CefRefPtr<CefBrowserProcessHandler> GetBrowserProcessHandler() OVERRIDE
    {
        return this;
    }
    //@}

    //! @name CefBrowserProcessHandler Methods
    //@{
    virtual void OnContextInitialized() OVERRIDE;
    //@}
};

#endif
