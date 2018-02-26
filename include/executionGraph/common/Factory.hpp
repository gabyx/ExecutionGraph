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
#include <optional>
#include <rttr/type>
#include <type_traits>
#include <unordered_map>
#include "executionGraph/common/Assert.hpp"
#include "executionGraph/config/Config.hpp"

#ifdef __clang__
#    pragma clang diagnostic push
#    pragma clang diagnostic ignored "-Wglobal-constructors"
#    pragma clang diagnostic ignored "-Wexit-time-destructors"
#endif

namespace executionGraph
{
    namespace detailsStaticFactory
    {
        template<typename TMap, typename T>
        struct StaticMapStorage;
        template<typename TMap, typename... Creator>
        struct StaticMapStorage<TMap, meta::list<Creator...>>
        {
            using Map = TMap;
            static Map map;  //!< Map with CreatorFunctions for the Factory.
        };
        //! Static initialization of the factory map with all key,function pointer pairs.
        template<typename TMap, typename... Creator>
        TMap StaticMapStorage<TMap, meta::list<Creator...>>::map = {{rttr::type::get<typename Creator::Key>(), &Creator::create}...};

        template<typename TCreatorList>
        struct Traits
        {
            using CreatorList = TCreatorList;
            static_assert(meta::is<CreatorList, meta::list>::value,
                          "CreatorList needs to be meta::list!");

            using CreatorFunction = decltype(&meta::front<CreatorList>::create);
            template<typename T>
            struct functionTraits;
            template<typename R, typename... Args>
            struct functionTraits<R (*)(Args...)>
            {
                using returnType = R;
                using arguments  = meta::list<Args...>;
            };
            using CreatorType   = typename functionTraits<CreatorFunction>::returnType;
            using DynamicMap    = std::unordered_map<rttr::type, CreatorFunction>;
            using StaticStorage = StaticMapStorage<DynamicMap, CreatorList>;

            // extract all Key types and check if unique -> otherwise assert!
            template<typename T>
            using extractKey = typename T::Key;
            using allKeys    = meta::transform<CreatorList, meta::quote<extractKey>>;
            using uniqueKeys = meta::unique<allKeys>;
            static_assert(meta::size<uniqueKeys>::value == meta::size<allKeys>::value,
                          "Your input CreatorList contains duplicated `Key` types!");

            template<typename Creator, typename SearchKey>
            using isCreator = std::is_same<typename Creator::Key, SearchKey>;

            struct Undefined
            {
            };
            template<typename T>
            using isUndefined = std::is_same<T, Undefined>;

            template<typename SearchKey>
            struct getCreatorTypeImpl
            {
                using list = meta::find_if<CreatorList,
                                           meta::bind_back<meta::quote<isCreator>, SearchKey>>;
                using type = meta::if_<meta::empty<list>, Undefined, meta::front<list>>;
            };

            template<typename SearchKey>
            using getCreatorType = meta::_t<getCreatorTypeImpl<SearchKey>>;
        };

    }  // namespace detailsStaticFactory

    /* ---------------------------------------------------------------------------------------*/
    /*!
        A factory class which a statically fixed list of creators which create types `CreatorType`.

        @date Sun Feb 18 2018
        @author Gabriel Nützi, gnuetzi (at) gmail (døt) com
    */
    /* ---------------------------------------------------------------------------------------*/
    template<typename CreatorList>
    class StaticFactory
    {
    public:
        using Traits        = detailsStaticFactory::Traits<CreatorList>;
        using StaticStorage = typename Traits::StaticStorage;
        using DynamicMap    = typename Traits::DynamicMap;

    public:
        using CreatorType = typename Traits::CreatorType;

        //! Create the type
        template<typename Key, typename... Args>
        static CreatorType create(Args&&... args)
        {
            using Creator = typename Traits::template getCreatorType<Key>;
            static_assert(!Traits::template isUndefined<Creator>::value, "Your Key is not found in the factory!");
            return Creator::create(std::forward<Args>(args)...);
        }

        //! Create the type registered with Key `key`.
        template<typename... Args>
        static std::optional<CreatorType> create(const rttr::type& key, Args&&... args)
        {
            auto it = StaticStorage::map.find(key);
            if(it == StaticStorage::map.end())
            {
                return {};
            }
            return it->second(std::forward<Args>(args)...);  // Will move automatically into the return
        }
    };

}  // namespace executionGraph

#    ifdef __clang__
#        pragma clang diagnostic pop
#    endif

#endif
