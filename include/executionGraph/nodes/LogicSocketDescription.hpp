// =========================================================================================
//  ExecutionGraph
//  Copyright (C) 2014 by Gabriel Nützi <gnuetzi (at) gmail (døt) com>
//
//  @date Fri Dec 27 2019
//  @author Gabriel Nützi, gnuetzi (at) gmail (døt) com
//
//  This Source Code Form is subject to the terms of the Mozilla Public
//  License, v. 2.0. If a copy of the MPL was not distributed with this
//  file, You can obtain one at http://mozilla.org/MPL/2.0/.
// =========================================================================================

#pragma once

#include <meta/meta.hpp>
#include "executionGraph/common/MetaCommon.hpp"
#include "executionGraph/common/MetaIndices.hpp"
#include "executionGraph/common/MetaInvoke.hpp"
#include "executionGraph/common/TypeDefs.hpp"
#include "executionGraph/nodes/LogicCommon.hpp"

namespace executionGraph
{
    /* ---------------------------------------------------------------------------------------*/
    /*!
        Describing input and output sockets. 
        This type can be used as `constexpr`.

        @note The string `name` provided in the constructor is referenced! 
        (c++20 constexpr strings make this type correct)

        @date Fri Dec 27 2019
        @author Gabriel Nützi, gnuetzi (at) gmail (døt) com
    */
    /* ---------------------------------------------------------------------------------------*/
    template<typename TData,
             typename TIndex,
             typename IsInput,
             typename TNode = void>
    class LogicSocketDescription
    {
    public:
        using Data  = TData;
        using Index = TIndex;
        using Node  = TNode;

        using SocketType = meta::if_<IsInput,
                                     LogicSocketInput<Data>,
                                     LogicSocketOutput<Data>>;

        constexpr LogicSocketDescription(std::string_view name)
            : m_name(name) {}

        static constexpr const auto index()
        {
            return static_cast<IndexType>(Index::value);
        }
        constexpr auto& name() const { return m_name; }

        static constexpr bool isInput() { return IsInput::value; }
        static constexpr bool isOutput() { return !isInput(); }

        template<typename T, bool strict = true>
        static constexpr bool belongsToNode(const T* = nullptr)
        {
            // A `Node == void` belongs to every node if non-strict.
            return (!strict && std::is_same_v<Node, void>) || std::is_same_v<T, Node>;
        }

    private:
        //! @todo This needs c++20 constexpr std::string ...
        const std::string_view m_name;
    };

    template<typename TData, typename TIndex, typename TNode = void>
    using InputDescription = LogicSocketDescription<TData,
                                                    TIndex,
                                                    meta::bool_<true>,
                                                    TNode>;

    template<typename TData, typename TIndex, typename TNode = void>
    using OutputDescription = LogicSocketDescription<TData,
                                                     TIndex,
                                                     meta::bool_<false>,
                                                     TNode>;

    template<typename CallableDesc,
             typename Data,
             IndexType Idx,
             typename TNode = void>
    constexpr auto makeSocketDescription(std::string_view name)
    {
        return meta::invoke<CallableDesc,
                            Data,
                            meta::size_t<Idx>,
                            TNode>{std::move(name)};
    }

    template<typename TData, IndexType Idx, typename TNode = void>
    constexpr auto makeInputDescription(std::string_view name)
    {
        return makeSocketDescription<meta::quote<InputDescription>,
                                     TData,
                                     Idx,
                                     TNode>(std::move(name));
    }

    template<typename TData, IndexType Idx, typename TNode = void>
    constexpr auto makeOutputDescription(std::string_view name)
    {
        return makeSocketDescription<meta::quote<OutputDescription>,
                                     TData,
                                     Idx,
                                     TNode>(std::move(name));
    }

    //! Test if all description are input descriptions.
    template<typename... SocketDesc>
    constexpr bool isInputDescriptions = (... && (meta::is_v<naked<SocketDesc>, LogicSocketDescription> &&
                                                  naked<SocketDesc>::isInput()));
    //! Test if all description are output descriptions.
    template<typename... SocketDesc>
    constexpr bool isOutputDescriptions = (... && (meta::is_v<naked<SocketDesc>, LogicSocketDescription> &&
                                                   naked<SocketDesc>::isOutput()));

    //! Test if all description have the same node type.
    template<typename SocketDescA, typename... SocketDescB>
    constexpr bool belongSocketDescriptionsToSameNodes = (... && naked<SocketDescB>::template belongsToNode<
                                                                     typename naked<SocketDescA>::Node>());

#define EG_DEFINE_INPUT_DESC(descName, TData, Idx, name) \
    static constexpr auto descName = makeInputDescription<TData, Idx, Node>(name)

#define EG_DEFINE_OUTPUT_DESC(descName, TData, Idx, name) \
    static constexpr auto descName = makeOutputDescription<TData, Idx, Node>(name)

#define EG_DEFINE_DESCS(descName, descName1, ...)                               \
    static constexpr auto descName = std::forward_as_tuple(descName1, __VA_ARGS__);    \
    static_assert(tupleUtil::invoke(descName, [](auto&&... desc) {                     \
                      return belongSocketDescriptionsToSameNodes<decltype(desc)...> && \
                             (isInputDescriptions<decltype(desc)...> ||                \
                              isOutputDescriptions<decltype(desc)...>);                \
                  }),                                                                  \
                  "All input or ouput sockets and on same node!");

}  // namespace executionGraph