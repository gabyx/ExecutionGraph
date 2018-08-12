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

#include <functional>
#include <string>
#include <crossguid/guid.hpp>
#include "executionGraph/config/Config.hpp"

namespace executionGraph
{
    /* ---------------------------------------------------------------------------------------*/
    /*!
        Class for representing a unique identifier.

        @date Sun Feb 18 2018
        @author Gabriel Nützi, gnuetzi (at) gmail (døt) com
    */
    /* ---------------------------------------------------------------------------------------*/
    class Identifier
    {
        template<typename T>
        friend struct std::hash;

    public:
        Identifier()
            : m_guid(xg::newGuid())
        {}

        Identifier(const Identifier&) = default;
        Identifier(Identifier&&)      = default;

        Identifier& operator=(const Identifier&) = default;
        Identifier& operator=(Identifier&&) = default;

        template<typename T,
                 typename SFINAE = std::enable_if_t<!std::is_base_of_v<Identifier, std::decay_t<T>>>>
        explicit Identifier(T&& guid)
            : m_guid(std::forward<T>(guid))
        {
        }

        //! Get the name of this identifier.
        std::string toString() const { return m_guid; }

        //! Comparison operators.
        bool operator==(const Identifier& id) const { return m_guid == id.m_guid; }
        bool operator!=(const Identifier& id) const { return !(m_guid == id.m_guid); }

    protected:
        //! Get the unique identifier.
        const xg::Guid& getId() { return m_guid; }

    private:
        xg::Guid m_guid;  //!< The unique identifier.
    };

    /* ---------------------------------------------------------------------------------------*/
    /*!
        Named Identifier Mixin.

        @date Tue Jul 31 2018
        @author Gabriel Nützi, gnuetzi (at) gmail (døt) com
    */
    /* ---------------------------------------------------------------------------------------*/
    template<typename TIdentifier>
    class MixinNamedIdentifier final : public TIdentifier
    {
    public:
        MixinNamedIdentifier()
            : TIdentifier(), m_name(TIdentifier::toString())
        {}

        MixinNamedIdentifier(const MixinNamedIdentifier&) = default;
        MixinNamedIdentifier(MixinNamedIdentifier&&)      = default;

        template<typename T, typename... Args>
        explicit MixinNamedIdentifier(const T& name, Args&&... args)
            : TIdentifier(std::forward<Args>(args)...), m_name(name)
        {}

        //! Get the name of this identifier.
        std::string getName() const { return m_name + "-" + TIdentifier::toString(); }

        //! Get the short name of this identifier.
        const std::string& getShortName() const { return m_name; }

    public:
        std::string m_name;  //!< The short name of this identifier.
    };

    using Id      = Identifier;                        //! Abreviation
    using IdNamed = MixinNamedIdentifier<Identifier>;  //! Abreviation
}  // namespace executionGraph

//! Specialize std::hash for executionGraph::Id
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
