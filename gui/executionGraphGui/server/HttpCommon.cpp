// =========================================================================================
//  ExecutionGraph
//  Copyright (C) 2014 by Gabriel Nützi <gnuetzi (at) gmail (døt) com>
//
//  @date Thu Dec 13 2018
//  @author Gabriel Nützi, gnuetzi (at) gmail (døt) com
//
//  This Source Code Form is subject to the terms of the Mozilla Public
//  License, v. 2.0. If a copy of the MPL was not distributed with this
//  file, You can obtain one at http://mozilla.org/MPL/2.0/.
// =========================================================================================

#include "executionGraphGui/server/HttpCommon.hpp"

namespace executionGraphGui
{
    // Append an HTTP rel-path to a local filesystem path.
    // The returned path is normalized for the platform.
    std::string path_cat(boost::beast::string_view base,
                         boost::beast::string_view path)
    {
        if(base.empty())
            return path.to_string();
        std::string result = base.to_string();

        char constexpr path_separator = '/';
        if(result.back() == path_separator)
            result.resize(result.size() - 1);
        result.append(path.data(), path.size());

        return result;
    }

}  // namespace executionGraphGui