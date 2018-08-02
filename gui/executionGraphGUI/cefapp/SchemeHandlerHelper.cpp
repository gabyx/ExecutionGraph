//! ========================================================================================
//!  ExecutionGraph
//!  Copyright (C) 2014 by Gabriel Nützi <gnuetzi (at) gmail (døt) com>
//!
//!  @date Fri Feb 09 2018
//!  @author Gabriel Nützi, gnuetzi (at) gmail (døt) com
//!
//!  This Source Code Form is subject to the terms of the Mozilla Public
//!  License, v. 2.0. If a copy of the MPL was not distributed with this
//!  file, You can obtain one at http://mozilla.org/MPL/2.0/.
//! ========================================================================================

#include "executionGraphGUI/cefapp/SchemeHandlerHelper.hpp"
#include <executionGraph/common/FileSystem.hpp>
#include "executionGraphGUI/common/Exception.hpp"

namespace schemeHandlerHelper
{
    //! Register all custom schemes for the application as standard and secure.
    //! Must be the same implementation in all processes.
    void registerCustomSchemes(CefRawPtr<CefSchemeRegistrar> registrar)
    {
        for(auto& scheme : getCustomSchemes())
        {
            bool result = registrar->AddCustomScheme(scheme,
                                                     true /* is standart*/,
                                                     false /* is local */,
                                                     false /* is display_isolated */,
                                                     true /* is secure */,
                                                     false /* is cors enabled*/,
                                                     false /* is_csp_bypassing*/);
            EXECGRAPHGUI_THROW_EXCEPTION_IF(!result, "Scheme not registered!");
        }
    }

}  // namespace schemeHandlerHelper