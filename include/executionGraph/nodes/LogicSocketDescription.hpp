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
#include "executionGraph/common/EnumClassHelper.hpp"
#include "executionGraph/common/EnumFlags.hpp"
#include "executionGraph/common/MetaCommon.hpp"
#include "executionGraph/common/MetaIndices.hpp"
#include "executionGraph/common/MetaInvoke.hpp"
#include "executionGraph/common/TupleUtil.hpp"
#include "executionGraph/common/TypeDefs.hpp"
#include "executionGraph/nodes/LogicCommon.hpp"
#include "executionGraph/nodes/LogicSocketFlags.hpp"

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
             typename TFlags = LogicSocketFlagsList<>,
             typename TNode  = void>
    class LogicSocketDescription
    {
    public:
        using Data   = TData;
        using Node   = TNode;
        using Flags  = TFlags;
        using EFlags = ELogicSocketFlags;

    private:
        static constexpr auto m_flags = convertSocketFlags(Flags{});

    public:
        constexpr LogicSocketDescription(SocketIndex index, std::string_view name)
            : m_index(index), m_name(name)
        {}

        constexpr SocketIndex index() const { return m_index; }

        static constexpr const auto& flags() { return m_flags; };
        static constexpr bool isOutput() { return flags().isSet(EFlags::Output); }
        static constexpr bool isInput() { return !isOutput(); }

        constexpr const auto& name() const { return m_name; }

        using SocketType = meta::if_<meta::bool_<isInput()>,
                                     LogicSocketInput<Data>,
                                     LogicSocketOutput<Data>>;

    public:
        template<typename T, bool strict = true>
        static constexpr bool belongsToNode(const T* = nullptr)
        {
            // A `Node == void` belongs to every node if non-strict.
            return (!strict && std::is_same_v<Node, void>) || std::is_same_v<T, Node>;
        }

    private:
        SocketIndex m_index;
        //! @todo This needs c++20 constexpr std::string ...
        std::string_view m_name;
    };

    template<typename Data, typename Flags = LogicSocketFlagsList<>, typename Node = void>
    using InputDescription = LogicSocketDescription<Data,
                                                    Flags,
                                                    Node>;

    template<typename Data, typename Flags = LogicSocketFlagsList<>, typename Node = void>
    using OutputDescription = LogicSocketDescription<Data,
                                                     Flags,
                                                     Node>;

    namespace details
    {
        template<auto&... socketDescs>
        constexpr auto sortSocketDescriptions()
        {
            return tupleUtil::sort<std::forward_as_tuple(socketDescs...),
                                   [](auto&& descA, auto&& descB) {
                                       return descA.index() < descB.index();
                                   }>();
        }
    }  // namespace details

    //! Make socket
    template<typename CallableDesc,
             typename Data,
             typename FlagsList = LogicSocketFlagsList<>,
             typename Node      = void>
    constexpr auto makeSocketDescription(SocketIndex index, std::string_view name)
    {
        return meta::invoke<CallableDesc,
                            Data,
                            FlagsList,
                            Node>{index, name};
    }

    template<typename Data, typename FlagsList = LogicSocketFlagsList<>, typename Node = void>
    constexpr auto makeInputDescription(SocketIndex index, std::string_view name)
    {
        return makeSocketDescription<meta::quote<InputDescription>,
                                     Data,
                                     FlagsList,
                                     Node>(index, name);
    }

    template<typename Data, typename FlagsList = LogicSocketFlagsList<>, typename Node = void>
    constexpr auto makeOutputDescription(SocketIndex index, std::string_view name)
    {
        using EFlags = ELogicSocketFlags;
        return makeSocketDescription<meta::quote<OutputDescription>,
                                     Data,
                                     meta::join<meta::list<FlagsList,
                                                           LogicSocketFlagsList<EFlags::Output>>>,
                                     Node>(index, name);
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

#define EG_DEFINE_INPUT_DESC(descName, TData, index, name) \
    static constexpr auto descName = makeInputDescription<TData, LogicSocketFlagsList<>, Node>(index, name)

#define EG_DEFINE_OUTPUT_DESC(descName, TData, index, name) \
    static constexpr auto descName = makeOutputDescription<TData, LogicSocketFlagsList<>, Node>(index, name)

#define EG_DEFINE_DESCS(descName, descName1, ...)                                               \
    static constexpr auto descName = details::sortSocketDescriptions<descName1, __VA_ARGS__>(); \
    static_assert(tupleUtil::invoke(descName, [](auto&&... desc) {                              \
                      return belongSocketDescriptionsToSameNodes<decltype(desc)...> &&          \
                             (isInputDescriptions<decltype(desc)...> ||                         \
                              isOutputDescriptions<decltype(desc)...>);                         \
                  }),                                                                           \
                  "All descriptions must be input or ouput and on the same node!")
}  // namespace executionGraph