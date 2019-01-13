// =========================================================================================
//  ExecutionGraph
//  Copyright (C) 2014 by Gabriel Nützi <gnuetzi (at) gmail (døt) com>
//
//  @date Sat Jul 07 2018
//  @author Gabriel Nützi, gnuetzi (at) gmail (døt) com
//
//  This Source Code Form is subject to the terms of the Mozilla Public
//  License, v. 2.0. If a copy of the MPL was not distributed with this
//  file, You can obtain one at http://mozilla.org/MPL/2.0/.
// =========================================================================================

#ifndef executionGraphGui_common_FunctionMap_hpp
#define executionGraphGui_common_FunctionMap_hpp

#include <functional>
#include <string>
#include <unordered_map>
#include <unordered_set>

/* ---------------------------------------------------------------------------------------*/
/*!
    A simple function mapping, for the request handlers internal dispatch.

    @date Sat Jul 07 2018
    @author Gabriel Nützi, gnuetzi (at) gmail (døt) com
 */
/* ---------------------------------------------------------------------------------------*/
template<typename TFunction, typename TKey = std::string>
class FunctionMap final
{
public:
    using Function = TFunction;
    using Map      = std::unordered_map<TKey, Function>;
    using Entry    = typename Map::value_type;

    template<typename Container>
    FunctionMap(Container&& entries)
    {
        for(auto& entry : entries)
        {
            m_keys.emplace(entry.first);
            m_map.emplace(entry);
        }
    }

    const auto& map() const { return m_map; }
    const auto& keys() const { return m_keys; }

    //! Dispatch over the function map.
    template<typename Key, typename... Args>
    bool dispatch(Key&& key, Args&&... args) const
    {
        auto it = m_map.find(key);
        if(it != m_map.end())
        {
            it->second(std::forward<Args>(args)...);
            return true;
        }
        return false;
    }

private:
    Map m_map;                        //! Key to function mapping.
    std::unordered_set<TKey> m_keys;  //! All keys.
};
#endif