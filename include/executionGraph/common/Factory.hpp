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
#include <rttr/type.h>
#include <type_traits>

#include "executionGraph/config/Config.hpp"

namespace executionGraph
{
    namespace detailsStaticFactory
    {
        template<typename CreatorList>
        struct Types
        {
            using FirstCreator    = meta::at_c<CreatorList, 0>;
            using Key             = typename FirstCreator::Key;
            using CreatorFunction = decltype(&FirstCreator::create);
            using CreatorType     = typename std::result_of<CreatorFunction>::type;
            using DynamicMap      = typename std::unordered_map<rttr::type, CreatorFunction>;

            template<typename Creator, typename SearchKey>
            using isCreator = std::is_same<typename Creator::Key, SearchKey>> ;

            // extract all Key types and check if unique -> otherwise warning
            template<typename T>
            using extractKey = typename T::Key;
            using allKeys    = meta::transform<CreatorList, meta::quote<extractKey>>;
            using uniqueKeys = meta::unique<allKeys>;
            static_assert(meta::size<uniqueKeys>::value == meta::size<allKeys>::value, "Your input CreatorList contains duplicated `Key` types!") template<typename SearchKey>
            using getCreator = meta::at_c<meta::find<CreatorList,
                                                     meta::bind_back<meta::quote<isCreator>, SearchKey>>,
                                          0>;
        };

        template<typename Map, typename T>
        struct MapStorage;
        template<typename Map, typename... Creator>
        struct MapStorage<Map, std::meta::list<Creator...>>
        {
            using Map static Map map;  //!< Map with CreatorFunctions for the Factory.
        };
        //! Static initialization of the factory map with all key,function pointer pairs.
        template<typename Map, typename... Creator>
        typename Map MapStorage<Map, std::meta::list<Creator...>>::map = {{rttr::type::get<Creator::Key>, &Creator::create}...};
    }  // namespace detailsStaticFactory

    //! A factory class which creates types `CreatorType`.
    template<typename CreatorList>
    class StaticFactory
    {
    public:
        using Key         = typename details::StaticFactoryDetails<CreatorList>::Key;
        using CreatorType = typename details::StaticFactoryDetails<CreatorList>::CreatorType;
        using DynamicMap  = details::StaticFactoryDetails<CreatorList>::DynamicMap;

        //! Create the type
        template<typename Key>
        static CreatorType create()
        {
            using Creator = getCreator<Key>;
            return Creator::create();
        }

        //! Create the type registered with Key `key`.
        static CreatorType create(const rttr::type& key)
        {
            auto it = MapStorage<DynamicMap, CreatorList>::map.find(key);

            if(it == creatorMap.end())
            {
                EXEC_GRAPH_THROWEXCEPTION("No such type in Factory!");
            }
            return it->second();  // Will move automatically
        }
    };

}  // namespace executionGraph

#endif