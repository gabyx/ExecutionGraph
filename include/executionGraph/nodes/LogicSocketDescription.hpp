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
#include "executionGraph/common/CharSequence.hpp"
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
             typename TNode = void>
    class LogicSocketDescription
    {
    public:
        using Data   = TData;
        using Node   = TNode;
        using EFlags = typename LogicSocketEnumFlags::Enum;

    public:
        template<typename Char, Char... Cs>
        constexpr LogicSocketDescription(SocketIndex index,
                                         CharSequence<Char, Cs...> name,
                                         LogicSocketEnumFlags flags = {})
            : m_index(index), m_name(name), m_flags(flags)
        {}

        constexpr SocketIndex index() const { return m_index; }
        constexpr const auto& flags() const { return m_flags; };
        constexpr bool isOutput() const { return flags().isSet(EFlags::Output); }
        constexpr bool isInput() const { return !isOutput(); }
        constexpr const auto& name() const { return m_name; }

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
        LogicSocketEnumFlags m_flags;
    };

    //! The input socket description type.
    template<typename Data, typename Node = void>
    using InputDescription = LogicSocketDescription<Data, Node>;

    //! The output socket descritption type.
    template<typename Data, typename Node = void>
    using OutputDescription = LogicSocketDescription<Data, Node>;

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

    //! Make a input socket description.
    template<typename Data, typename Node = void, typename Char, Char... Cs>
    constexpr auto makeInputDescription(SocketIndex index,
                                        CharSequence<Char, Cs...> name,
                                        LogicSocketEnumFlags flags = {})
    {
        return InputDescription<Data, Node>(index, name, flags);
    }

    //! Make a output socket description.
    template<typename Data, typename Node = void, typename Char, Char... Cs>
    constexpr auto makeOutputDescription(SocketIndex index,
                                         CharSequence<Char, Cs...> name,
                                         LogicSocketEnumFlags flags = {})
    {
        using EFlags = ELogicSocketFlags;
        return OutputDescription<Data, Node>(index, name, {flags, EFlags::Output});
    }

    //! Test if all description are input descriptions.
    template<typename... SocketDescs>
    constexpr bool isInputDescriptions(SocketDescs&&... socketDescs)
    {
        return (... && (meta::is_v<naked<decltype(socketDescs)>, LogicSocketDescription> &&
                        socketDescs.isInput()));
    }
    //! Test if all description are output descriptions.
    template<typename... SocketDescs>
    constexpr bool isOutputDescriptions(SocketDescs&&... socketDescs)
    {
        return (... && (meta::is_v<naked<decltype(socketDescs)>, LogicSocketDescription> &&
                        socketDescs.isOutput()));
    }

    //! Test if all description have the same node type.
    template<typename SocketDescA, typename... SocketDescB>
    constexpr bool belongSocketDescriptionsToSameNodes(SocketDescA&&, SocketDescB&&...)
    {
        return (... && naked<SocketDescB>::template belongsToNode<typename naked<SocketDescA>::Node>());
    }

//! Define a input socket description.
#define EG_DEFINE_INPUT_DESC(descName, TData, index, ...) \
    static constexpr auto descName = makeInputDescription<TData, Node>(index, __VA_ARGS__)

//! Define a output socket description.
#define EG_DEFINE_OUTPUT_DESC(descName, TData, index, ...) \
    static constexpr auto descName = makeOutputDescription<TData, Node>(index, __VA_ARGS__)

//! Define a `std::tuple<SocketDesc&...>` of either input or output socket description
//! sorted by socket index.
#define EG_DEFINE_DESCS(descName, descName1, ...)                                               \
    static constexpr auto descName = details::sortSocketDescriptions<descName1, __VA_ARGS__>(); \
    static_assert(tupleUtil::invoke(descName, [](auto&&... descs) {                             \
                      return belongSocketDescriptionsToSameNodes(descs...) &&                   \
                             (isInputDescriptions(descs...) ||                                  \
                              isOutputDescriptions(descs...));                                  \
                  }),                                                                           \
                  "All descriptions must be input or ouput and on the same node!")
}  // namespace executionGraph