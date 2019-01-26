// =========================================================================================
//  ExecutionGraph
//  Copyright (C) 2014 by Gabriel Nützi <gnuetzi (at) gmail (døt) com>
//
//  @date Fri Jul 06 2018
//  @author Gabriel Nützi, gnuetzi (at) gmail (døt) com
//
//  This Source Code Form is subject to the terms of the Mozilla Public
//  License, v. 2.0. If a copy of the MPL was not distributed with this
//  file, You can obtain one at http://mozilla.org/MPL/2.0/.
// =========================================================================================

#pragma once

#include <string>
#include <meta/meta.hpp>
#include <rttr/type>

namespace executionGraph
{
    //! A simple socket descriptio, describing a LogicSocket.
    struct SocketTypeDescription
    {
        std::string m_type;           //!< The unique RTTI name of the socket
        std::string m_name = m_type;  //!< The readable name of the socket
    };

    //! Get all socket descriptions for this config `TConfig`.
    template<typename TConfig>
    static const std::vector<SocketTypeDescription>& getSocketDescriptions()
    {
        using WrappedNodes = meta::transform<typename TConfig::SocketTypes, meta::quote<meta::id>>;

        auto init = []() {
            std::vector<SocketTypeDescription> desc;
            auto create = [&desc](auto idType) {
                using SocketType = typename decltype(idType)::type;
                desc.emplace_back(SocketTypeDescription{rttr::type::get<SocketType>().get_name().to_string()});
            };

            meta::for_each(WrappedNodes{}, create);
            return desc;
        };

        //! The static socket description for this default configuration
        static const std::vector<SocketTypeDescription> socketTypeDescriptions = init();

        return socketTypeDescriptions;
    }

}  // namespace executionGraph
