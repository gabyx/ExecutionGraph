//! ========================================================================================
//!  ExecutionGraph
//!  Copyright (C) 2014 by Gabriel Nützi <gnuetzi (at) gmail (døt) com>
//!
//!  @date Mon Jan 15 2018
//!  @author Gabriel Nützi, gnuetzi (at) gmail (døt) com
//!
//!  This Source Code Form is subject to the terms of the Mozilla Public
//!  License, v. 2.0. If a copy of the MPL was not distributed with this
//!  file, You can obtain one at http://mozilla.org/MPL/2.0/.
//! ========================================================================================

#ifndef executionGraph_common_Factory_hpp
#define executionGraph_common_Factory_hpp

#include <meta/meta.hpp>
#include "executionGraph/config/Config.hpp"

namespace executionGraph
{
    //! A factory class which creates types `CreatorType`.
    template<typename Key, typename CreatorList>
    class Factory
    {
    public:
        using CreatorFunction = CreatorType (*)();

        //! Register the creation of a type `CreatorType` by a Key `key`
        //! executed by functor `Creator::create`.
        template<typename Creator>
        static void register(const Key& key)
        {
            // If this does not compile your type `Creator` does not have a static
            // create function of type `CreatorFunction`.
            creatorMap.emplace(type, static_cast<CreatorFunction>(&Creator::create));
        }

        //! Create the type registered with Key `key`.
        static CreatorType create(const Key& key)
        {
            auto it = creatorMap.find(key);

            if(it != creatorMap.end())
            {
                return it->second();  // Will move automatically
            }

            return CreatorType{};
        }

    private:
        static std::unordered_map<Key, CreatorFunction> creatorMap;  //!< Map with CreatorFunctions for the Factory.
    };

}  // namespace executionGraph
