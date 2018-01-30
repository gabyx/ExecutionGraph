//! ========================================================================================
//!  ExecutionGraph
//!  Copyright (C) 2014 by Gabriel Nützi <gnuetzi (at) gmail (døt) com>
//!
//!  @date Tue Jan 09 2018
//!  @author Gabriel Nützi, gnuetzi (at) gmail (døt) com
//!
//!  This Source Code Form is subject to the terms of the Mozilla Public
//!  License, v. 2.0. If a copy of the MPL was not distributed with this
//!  file, You can obtain one at http://mozilla.org/MPL/2.0/.
//! ========================================================================================

#ifndef executionGraph_common_Identifier_hpp
#define executionGraph_common_Identifier_hpp

#include <Guid.hpp>
#include <functional>
#include <string>
#include "executionGraph/config/Config.hpp"

namespace executionGraph
{
    class Identifier final
    {
        template<typename T>
        friend struct std::hash;

    public:
        Identifier(const char* name)
            : m_name(name)
        {
        }
        Identifier(const std::string& name)
            : m_name(name)
        {
        }
        Identifier(const std::string& name, const xg::Guid& guid)
            : m_name(name), m_guid(guid)
        {
        }

        //! Get the name of this identifier
        const std::string& getName();

        //! Get full name of this identifier: "<name>-<guid>".
        std::string getFullName() { return m_name + "-" + std::string(m_guid); }

        //! Comparison operators.
        bool operator==(const Identifier& id) const { return m_guid == id.m_guid; }
        bool operator!=(const Identifier& id) const { return !(m_guid == id.m_guid); }

    private:
        std::string m_name;  //!< The nickname of this identifier.
        xg::Guid m_guid;     //!< The unique guid of this identifier.
    };

    using Id = Identifier;

}  // namespace executionGraph

//! Sepcialize std::hash
namespace std
{
    template<>
    class hash<executionGraph::Id>
    {
    public:
        size_t operator()(const executionGraph::Id& id) const
        {
            // Hash all bytes...
            const std::array<unsigned char, 16>& bytes = id.m_guid.bytes();
            size_t h                                   = *reinterpret_cast<const uint64_t*>(&bytes[0]);  // 8 bytes
            return h ^ (*reinterpret_cast<const uint64_t*>(&bytes[8]) << 1);
        }
    };

}  // namespace std

#endif
