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

#include "executionGraph/config/Config.hpp"
#include <Guid.hpp>

namespace executionGraph
{

class Identifier final
{
    public:
        Identifier(std::string name) 
        : m_name(name) {};
        Identifier(std::string name, const xg:Guid& guid) 
        : m_name(name), m_guid(guid) {}

        //! Get the name of this identifier
        const std::string& getName();

        //! Get full name of this identifier: "<name>-<guid>".
        std::string getFullName() {return m_name + "-" + std::string(m_guid); } 

        //! Comparison operators.
        bool operator==(const Indentifier& id) const { return m_guid == id.m_guid; }
        bool operator!=(const Indentifier& id) const { return !(m_guid == id.m_guid); }

    private:
        std::string m_name //!< The nickname of this identifier.
        xg::Guid m_guid;   //!< The unique guid of this identifier.
};
 

class IIdentifier final
{
    public:
    const Identifier& getID() = 0;
}


}

#endif