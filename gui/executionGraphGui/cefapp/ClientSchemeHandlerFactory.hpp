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

#ifndef executionGraphGui_cefapp_ClientSchemeHandlerFactory_hpp
#define executionGraphGui_cefapp_ClientSchemeHandlerFactory_hpp

#include <cef_scheme.h>
#include <string>
#include <executionGraph/common/FileSystem.hpp>

/* ---------------------------------------------------------------------------------------*/
/*!
    Factory for creating client request handlers.

    @date Sun Feb 18 2018
    @author Gabriel Nützi, gnuetzi (at) gmail (døt) com
 */
/* ---------------------------------------------------------------------------------------*/
class ClientSchemeHandlerFactory final : public CefSchemeHandlerFactory
{
    IMPLEMENT_REFCOUNTING(ClientSchemeHandlerFactory)
public:
    ClientSchemeHandlerFactory(std::path folderPath, std::path pathPrefix = "")
        : m_folderPath(folderPath)
        , m_pathPrefix(pathPrefix)
    {
    }
    virtual ~ClientSchemeHandlerFactory() = default;

    virtual CefRefPtr<CefResourceHandler> Create(CefRefPtr<CefBrowser> browser,
                                                 CefRefPtr<CefFrame> frame,
                                                 const CefString& scheme_name,
                                                 CefRefPtr<CefRequest> request) override;

private:
    const std::path m_folderPath;  //! Where the files this handler serves are located.
    const std::path m_pathPrefix;  //! Prefix of the URL Path part.
};

#endif
