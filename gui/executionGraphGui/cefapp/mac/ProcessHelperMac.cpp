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

#include <cef_app.h>
#include "cefapp/mac/RendererApp.hpp"

namespace
{
    enum ProcessType
    {
        PROCESS_TYPE_BROWSER,
        PROCESS_TYPE_RENDERER,
        PROCESS_TYPE_OTHER,
    };

    const char kProcessType[]     = "type";
    const char kRendererProcess[] = "renderer";
#if defined(OS_LINUX)
    const char kZygoteProcess[] = "zygote";
#endif

    CefRefPtr<CefCommandLine> CreateCommandLine(const CefMainArgs& main_args)
    {
        CefRefPtr<CefCommandLine> command_line = CefCommandLine::CreateCommandLine();
        command_line->InitFromArgv(main_args.argc, main_args.argv);
        return command_line;
    }

    ProcessType GetProcessType(const CefRefPtr<CefCommandLine>& command_line)
    {
        // The command-line flag won't be specified for the browser process.
        if(!command_line->HasSwitch(kProcessType))
            return PROCESS_TYPE_BROWSER;

        const std::string& process_type = command_line->GetSwitchValue(kProcessType);
        if(process_type == kRendererProcess)
            return PROCESS_TYPE_RENDERER;

#if defined(OS_LINUX)
        // On Linux the zygote process is used to spawn other process types. Since we
        // don't know what type of process it will be we give it the renderer app.
        if(process_type == kZygoteProcess)
            return PROCESS_TYPE_RENDERER;
#endif

        return PROCESS_TYPE_OTHER;
    }
}

// Entry point function for sub-processes.
int main(int argc, char* argv[])
{
    // Provide CEF with command-line arguments.
    CefMainArgs main_args(argc, argv);

    // Create a temporary CommandLine object.
    CefRefPtr<CefCommandLine> command_line = CreateCommandLine(main_args);

    // Create a CefApp of the correct process type. The browser process is handled
    // by main_mac.mm.
    CefRefPtr<CefApp> app;
    switch(GetProcessType(command_line))
    {
        case PROCESS_TYPE_RENDERER:
            app = new RendererApp();
            break;
        // case PROCESS_TYPE_OTHER:
        //     app = CreateOtherProcessApp();
        //     break;
        default:
            break;
    }

    // Execute the sub-process.
    return CefExecuteProcess(main_args, app, NULL);
}
