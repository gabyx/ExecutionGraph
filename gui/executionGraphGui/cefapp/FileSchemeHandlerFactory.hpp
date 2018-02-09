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

#ifndef backend_File_Scheme_Handler_Factory_h
#define backend_File_Scheme_Handler_Factory_h

#include <cef_scheme.h>
#include <string>
#include "executionGraph/common/FileSystem.hpp"

//! Factory for creating client request handlers.
class FileSchemeHandlerFactory final : public CefSchemeHandlerFactory
{
public:
    FileSchemeHandlerFactory(std::path folderPath, std::path pathPrefix)
        : m_folderPath(folderPath)
        , m_pathPrefix(pathPrefix)
    {
    }
    virtual ~FileSchemeHandlerFactory() = default;

    virtual CefRefPtr<CefResourceHandler> Create(CefRefPtr<CefBrowser> browser,
                                                 CefRefPtr<CefFrame> frame,
                                                 const CefString& scheme_name,
                                                 CefRefPtr<CefRequest> request) override;

private:
    const std::path m_folderPath;  //! Where the files this handler serves are located.
    const std::path m_pathPrefix;  //! Prefix of the URL Path part.

    IMPLEMENT_REFCOUNTING(FileSchemeHandlerFactory)
};

#endif
