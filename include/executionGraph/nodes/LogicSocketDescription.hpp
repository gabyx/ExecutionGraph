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
#include "executionGraph/common/TypeDefs.hpp"
#include "executionGraph/nodes/LogicCommon.hpp"

namespace executionGraph
{
    enum class ELogicSocketFlags : std::size_t
    {
        NoFlags = 0,
        //! If the socket is a output socket, otherwise its a input socket.
        Output = 1,
        //! If the socket is optional meaning it needs no data node connection.
        Optional = 2
    };

    template<ELogicSocketFlags... E>
    using LogicSocketFlags = meta::list<decltype(enumToIntC(E))...>;

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
             typename Flags = LogicSocketFlags<>,
             typename TNode = void>
    class LogicSocketDescription
    {
    public:
        using Data   = TData;
        using Index  = TIndex;
        using Node   = TNode;
        using EFlags = ELogicSocketFlags;

        static_assert(std::is_same_v<SocketIndex, Index::value_type>,
                      "Wrong index type");

    private:
        static constexpr auto m_flags = EnumFlags<EFlags>{
            meta::accumulate<Flags,
                             meta::size_t<0>,
                             meta::quote<meta::bit_or>>::value};

    public:
        constexpr LogicSocketDescription(std::string_view name)
            : m_name(name) {}

        static constexpr SocketIndex index() { return Index::value; }

        static constexpr const auto& flags() { return flags; };
        static constexpr bool isInput() { return !isOutput(); }
        static constexpr bool isOutput() { return flags.isSet(EFlags::Output); }

        constexpr const auto& name() const { return m_name; }

        using SocketType = meta::if_<meta::bool_<flags().isSet()>,
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
        //! @todo This needs c++20 constexpr std::string ...
        const std::string_view m_name;
    };

    template<typename Data, typename Index, typename Flags = LogicSocketFlags<>, typename Node = void>
    using InputDescription = LogicSocketDescription<Data,
                                                    Index,
                                                    Flags,
                                                    Node>;

    template<typename Data, typename Index, typename Flags = LogicSocketFlags<>, typename Node = void>
    using OutputDescription = LogicSocketDescription<Data,
                                                     Index,
                                                     Flags,
                                                     Node>;

    template<typename CallableDesc,
             typename Data,
             IndexType Idx,
             typename FlagsList = LogicSocketFlags<>,
             typename Node      = void>
    constexpr auto makeSocketDescription(std::string_view name)
    {
        return meta::invoke<CallableDesc,
                            Data,
                            meta::size_t<Idx>,
                            FlagsList,
                            Node>{std::move(name)};
    }

    template<typename Data, IndexType Idx, typename FlagsList = LogicSocketFlags<>, typename Node = void>
    constexpr auto makeInputDescription(std::string_view name)
    {
        return makeSocketDescription<meta::quote<InputDescription>,
                                     Data,
                                     Idx,
                                     FlagsList,
                                     Node>(std::move(name));
    }

    template<typename Data, IndexType Idx, typename FlagsList = LogicSocketFlags<>, typename Node = void>
    constexpr auto makeOutputDescription(std::string_view name)
    {
        return makeSocketDescription<meta::quote<OutputDescription>,
                                     Data,
                                     Idx,
                                     meta::push_back<FlagsList, decltype(enumToIntC(ELogicSocketFlags::Output))>,
                                     Node>(std::move(name));
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
    static constexpr auto descName = makeInputDescription<TData, Idx, LogicSocketFlags<>, Node>(name)

#define EG_DEFINE_OUTPUT_DESC(descName, TData, Idx, name) \
    static constexpr auto descName = makeOutputDescription<TData, Idx, LogicSocketFlags<>, Node>(name)

#define EG_DEFINE_DESCS(descName, descName1, ...)                                      \
    static constexpr auto descName = std::forward_as_tuple(descName1, __VA_ARGS__);    \
    static_assert(tupleUtil::invoke(descName, [](auto&&... desc) {                     \
                      return belongSocketDescriptionsToSameNodes<decltype(desc)...> && \
                             (isInputDescriptions<decltype(desc)...> ||                \
                              isOutputDescriptions<decltype(desc)...>);                \
                  }),                                                                  \
                  "All input or ouput sockets and on same node!");

}  // namespace executionGraph