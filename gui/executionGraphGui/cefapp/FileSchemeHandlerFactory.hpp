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

#ifndef FILE_SCHEME_HANDLER_FACTORY_H
#define FILE_SCHEME_HANDLER_FACTORY_H

#include <cef_scheme.h>
#include <string>
#include "executionGraph/common/FileSystem.hpp"

// Implementation of the factory for creating client request handlers.
class FileSchemeHandlerFactory : public CefSchemeHandlerFactory
{
public:
    FileSchemeHandlerFactory(std::path folderPath, std::path urlPrefix)
        : m_folderPath(folderPath)
        , m_urlPrefix(urlPrefix)
    {
    }

    virtual CefRefPtr<CefResourceHandler> Create(CefRefPtr<CefBrowser> browser,
                                                 CefRefPtr<CefFrame> frame,
                                                 const CefString& scheme_name,
                                                 CefRefPtr<CefRequest> request) override;

private:
    const std::path m_folderPath;
    const std::path m_urlPrefix;

    IMPLEMENT_REFCOUNTING(FileSchemeHandlerFactory);
};

#endif
