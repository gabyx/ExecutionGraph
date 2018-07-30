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

#import <Cocoa/Cocoa.h>
#include <cef_application_mac.h>
#include <spdlog/spdlog.h>
#include <wrapper/cef_helpers.h>
#include "cefapp/App.hpp"
#include "cefapp/AppCLArgs.hpp"
#include "cefapp/AppHandler.hpp"
#include "common/Loggers.hpp"

namespace
{
    //! The global application instance.
    CefRefPtr<App> application;

    //! Get the associated application handler for `application`.
    CefRefPtr<AppHandler> GetAppHandler()
    {
        return application ? application->GetAppHandler() : nullptr;
    }
}  // namespace

// Receives notifications from the application.
@interface ClientAppDelegate : NSObject<NSApplicationDelegate>
- (void)createApplication:(id)object;
- (void)tryToTerminateApplication:(NSApplication*)app;

- (IBAction)showDeveloperTools:(id)sender;
@end

// Provide the CefAppProtocol implementation required by CEF.
@interface ClientApplication : NSApplication<CefAppProtocol>
{
@private
    BOOL handlingSendEvent_;
}
@end

@implementation ClientApplication
- (BOOL)isHandlingSendEvent
{
    return handlingSendEvent_;
}

- (void)setHandlingSendEvent:(BOOL)handlingSendEvent
{
    handlingSendEvent_ = handlingSendEvent;
}

- (void)sendEvent:(NSEvent*)event
{
    CefScopedSendingEvent sendingEventScoper;
    [super sendEvent:event];
}

// |-terminate:| is the entry point for orderly "quit" operations in Cocoa. This
// includes the application menu's quit menu item and keyboard equivalent, the
// application's dock icon menu's quit menu item, "quit" (not "force quit") in
// the Activity Monitor, and quits triggered by user logout and system restart
// and shutdown.
//
// The default |-terminate:| implementation ends the process by calling exit(),
// and thus never leaves the main run loop. This is unsuitable for Chromium
// since Chromium depends on leaving the main run loop to perform an orderly
// shutdown. We support the normal |-terminate:| interface by overriding the
// default implementation. Our implementation, which is very specific to the
// needs of Chromium, works by asking the application delegate to terminate
// using its |-tryToTerminateApplication:| method.
//
// |-tryToTerminateApplication:| differs from the standard
// |-applicationShouldTerminate:| in that no special event loop is run in the
// case that immediate termination is not possible (e.g., if dialog boxes
// allowing the user to cancel have to be shown). Instead, this method tries to
// close all browsers by calling CloseBrowser(false) via
// ClientHandler::CloseAllBrowsers. Calling CloseBrowser will result in a call
// to ClientHandler::DoClose and execution of |-performClose:| on the NSWindow.
// DoClose sets a flag that is used to differentiate between new close events
// (e.g., user clicked the window close button) and in-progress close events
// (e.g., user approved the close window dialog). The NSWindowDelegate
// |-windowShouldClose:| method checks this flag and either calls
// CloseBrowser(false) in the case of a new close event or destructs the
// NSWindow in the case of an in-progress close event.
// ClientHandler::OnBeforeClose will be called after the CEF NSView hosted in
// the NSWindow is dealloc'ed.
//
// After the final browser window has closed ClientHandler::OnBeforeClose will
// begin actual tear-down of the application by calling CefQuitMessageLoop.
// This ends the NSApplication event loop and execution then returns to the
// main() function for cleanup before application termination.
//
// The standard |-applicationShouldTerminate:| is not supported, and code paths
// leading to it must be redirected.
- (void)terminate:(id)sender
{
    ClientAppDelegate* delegate =
        static_cast<ClientAppDelegate*>([NSApp delegate]);
    [delegate tryToTerminateApplication:self];
    // Return, don't exit. The application is responsible for exiting on its own.
}
@end

@implementation ClientAppDelegate
// Create the application on the UI thread.
- (void)createApplication:(id)object
{
    NSApplication* application = [NSApplication sharedApplication];

    // The top menu is configured using Interface Builder (IB). To modify the menu
    // start by loading MainMenu.xib in IB.
    //
    // To associate MainMenu.xib with ClientAppDelegate:
    // 1. Select "File's Owner" from the "Placeholders" section in the left side
    //    pane.
    // 2. Load the "Identity inspector" tab in the top-right side pane.
    // 3. In the "Custom Class" section set the "Class" value to
    //    "ClientAppDelegate".
    // 4. Pass an instance of ClientAppDelegate as the |owner| parameter to
    //    loadNibNamed:.
    //
    // To create a new top menu:
    // 1. Load the "Object library" tab in the bottom-right side pane.
    // 2. Drag a "Submenu Menu Item" widget from the Object library to the desired
    //    location in the menu bar shown in the center pane.
    // 3. Select the newly created top menu by left clicking on it.
    // 4. Load the "Attributes inspector" tab in the top-right side pane.
    // 5. Under the "Menu Item" section set the "Tag" value to a unique integer.
    //    This is necessary for the GetMenuBarMenuWithTag function to work
    //    properly.
    //
    // To create a new menu item in a top menu:
    // 1. Add a new receiver method in ClientAppDelegate (e.g. menuTestsDoStuff:).
    // 2. Load the "Object library" tab in the bottom-right side pane.
    // 3. Drag a "Menu Item" widget from the Object library to the desired
    //    location in the menu bar shown in the center pane.
    // 4. Double-click on the new menu item to set the label.
    // 5. Right click on the new menu item to show the "Get Source" dialog.
    // 6. In the "Sent Actions" section drag from the circle icon and drop on the
    //    new receiver method in the ClientAppDelegate source code file.
    //
    // Load the top menu from MainMenu.xib.
    [[NSBundle mainBundle] loadNibNamed:@"MainMenu"
                                  owner:self
                        topLevelObjects:nil];

    // Set the delegate for application events.
    [application setDelegate:self];
}

- (void)tryToTerminateApplication:(NSApplication*)app
{
    auto appHandler = GetAppHandler();
    if(appHandler && !appHandler->IsClosing())
    {
        appHandler->CloseAllBrowsers(false);
    }
}

- (NSApplicationTerminateReply)applicationShouldTerminate:(NSApplication*)sender
{
    return NSTerminateNow;
}

//! Show the developer tools.
- (IBAction)showDeveloperTools:(id)sender
{
    auto appHandler = GetAppHandler();
    if(appHandler)
    {
        appHandler->ShowDeveloperTools();
    }
}
@end

// Entry point function for the browser process.
int main(int argc, char* argv[])
{
    // Parse command line arguments
    EXECGRAPH_INSTANCIATE_SINGLETON_CTOR(AppCLArgs, appCLArgs, (argc, argv));
    auto cefArgs = AppCLArgs::getInstance().getCEFArgs();

    // Provide CEF with command-line arguments.
    CefMainArgs mainArgs(cefArgs.size(), cefArgs.data());

    // Make all application loggers
    EXECGRAPH_INSTANCIATE_SINGLETON_CTOR(Loggers, loggers, (AppCLArgs::getInstance().getLogPath()));

    // Initialize the AutoRelease pool.
    NSAutoreleasePool* autopool = [[NSAutoreleasePool alloc] init];

    // Initialize the ClientApplication instance.
    [ClientApplication sharedApplication];

    // Specify CEF global settings here.
    CefSettings settings;
    settings.remote_debugging_port = 8088;

    // App implements application-level callbacks for the browser process.
    // It will create the first browser instance in OnContextInitialized() after
    // CEF has initialized.
    application = std::make_unique<App>(appCLArgs->getClientSourcePath()).release();

    // Initialize CEF for the browser process.
    CefInitialize(mainArgs, settings, application, nullptr);

    // Create the application delegate.
    NSObject* delegate = [[ClientAppDelegate alloc] init];
    [delegate performSelectorOnMainThread:@selector(createApplication:)
                               withObject:nil
                            waitUntilDone:NO];

    // Run the CEF message loop. This will block until CefQuitMessageLoop() is
    // called.
    CefRunMessageLoop();

    // Shut down CEF.
    CefShutdown();

    // Release the delegate.
    [delegate release];

    // Release the AutoRelease pool.
    [autopool release];

    return 0;
}
