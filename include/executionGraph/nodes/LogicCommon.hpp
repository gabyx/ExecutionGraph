//! ========================================================================================
//!  ExecutionGraph
//!  Copyright (C) 2014 by Gabriel Nützi <gnuetzi (at) gmail (døt) com>
//!
//!  @date Mon Jan 08 2018
//!  @author Gabriel Nützi, <gnuetzi (at) gmail (døt) com>
//!
//!  This Source Code Form is subject to the terms of the Mozilla Public
//!  License, v. 2.0. If a copy of the MPL was not distributed with this
//!  file, You can obtain one at http://mozilla.org/MPL/2.0/.
//! ========================================================================================

#pragma once

#include <array>
#include <memory>
#include <string>
#include "executionGraph/common/EnumClassHelper.hpp"
#include "executionGraph/common/SfinaeMacros.hpp"
#include "executionGraph/common/TypeDefs.hpp"
#include "executionGraph/nodes/LogicSocketDefaultTypes.hpp"

#define EXECGRAPH_DEFINE_TYPES(__CONFIG__)                                   \
    using Config                  = __CONFIG__;                              \
    using NodeId                  = executionGraph::NodeId;                  \
    using IndexType               = executionGraph::IndexType;               \
    using SocketIndex             = executionGraph::SocketIndex;             \
    using SocketTypes             = typename Config::SocketTypes;            \
    using NodeBaseType            = typename Config::NodeBaseType;           \
    using SocketInputBaseType     = typename Config::SocketInputBaseType;    \
    using SocketOutputBaseType    = typename Config::SocketOutputBaseType;   \
    using SocketInputBasePointer  = typename Config::SocketInputBasePointer; \
    using SocketOutputBasePointer = typename Config::SocketOutputBasePointer

#define EXECGRAPH_DEFINE_CONFIG(__CONFIG__)                               \
    EXECGRAPH_DEFINE_TYPES(__CONFIG__);                                   \
    template<typename T>                                                  \
    using SocketInputType = typename Config::template SocketInputType<T>; \
    template<typename T>                                                  \
    using SocketOutputType = typename Config::template SocketOutputType<T>

//! Some handy macro to use when inheriting from LogicNode.
#define EXECGRAPH_DEFINE_SOCKET_TRAITS(InputEnum, OutputEnum)                                                                       \
    template<InputEnum id, typename TData>                                                                                          \
    using InSocketDecl = executionGraph::details::InputSocketDeclarationBase<meta::size_t<executionGraph::enumToInt(id)>, TData>;   \
    template<OutputEnum id, typename TData>                                                                                         \
    using OutSocketDecl = executionGraph::details::OutputSocketDeclarationBase<meta::size_t<executionGraph::enumToInt(id)>, TData>; \
    template<typename... TSockets>                                                                                                  \
    using InSocketDeclList = executionGraph::details::InputSocketDeclarationList<InputEnum, TSockets...>;                           \
    template<typename... TSockets>                                                                                                  \
    using OutSocketDeclList = executionGraph::details::OutputSocketDeclarationList<OutputEnum, TSockets...>;

namespace executionGraph
{
    // Forward declarations
    template<typename TConfig>
    class LogicNode;
    template<typename TConfig>
    class LogicSocketBase;
    template<typename TConfig>
    class LogicSocketInputBase;
    template<typename TConfig>
    class LogicSocketOutputBase;
    template<typename T, typename TConfig>
    class LogicSocketInput;
    template<typename T, typename TConfig>
    class LogicSocketOutput;

    template<typename T>
    using SocketPointer = std::unique_ptr<T, void (*)(T*)>;  //! The general socket pointer type.

    using IndexType   = uint64_t;   //! A general index type.
    using NodeId      = uint64_t;   //! Node Id type.
    using SocketIndex = IndexType;  //! The socket index type.

    //! The general config which is used to build nodes and execution trees.
    //! It defaults to the standart socket types in SocketDefaultTypes
    template<typename TSocketTypes = SocketDefaultTypes>
    struct GeneralConfig
    {
        using SocketTypes  = TSocketTypes;
        using NodeBaseType = LogicNode<GeneralConfig>;

        using SocketInputBaseType  = LogicSocketInputBase<GeneralConfig>;   //! This class is used as the base for input sockets.
        using SocketOutputBaseType = LogicSocketOutputBase<GeneralConfig>;  //! This class is used as the base for output sockets.

        using SocketInputBasePointer  = SocketPointer<SocketInputBaseType>;
        using SocketOutputBasePointer = SocketPointer<SocketOutputBaseType>;

        template<typename T>
        using SocketInputType = LogicSocketInput<T, GeneralConfig>;  //! This is the class template (T needs to be in TSocketTypes) for input sockets.
        template<typename T>
        using SocketOutputType = LogicSocketOutput<T, GeneralConfig>;  //! This is the class template (T needs to be in TSocketTypes) for output sockets.
    };

    //! Some type traits for input/output socket definitions in derived classes from GeneralConfig::NodeBaseType
    //! These metaprogramming tricks here, allow to smoothly define input/output sockets.
    //! See the examples.
    namespace details
    {
        //! The basis class for every socket declaration.
        template<typename TId, typename TData>
        struct SocketDeclarationBase
        {
            using Id       = TId;    //! The integral identifier for this socket. (This is the converted enum defined by the user).
            using DataType = TData;  //! The data type of the socket.
        };

        template<typename TId, typename TData, typename TIndex>
        struct SocketDeclaration : SocketDeclarationBase<TId, TData>
        {
            using Index = TIndex;  //! The index corresponding to the storage of all input (or output) sockets in LogicNode.
        };

        //! The actual socket declaration list, the order of the socket declarations in this list
        //! corresponds to the sequence in which they are stored in LogicNode<Config>.
        template<typename TEnum,
                 template<typename...>
                 class TMPSocketDeclIn,
                 template<typename...>
                 class TMPSocketDeclOut,
                 typename... TSocketDecl>
        struct SocketDeclarationList
        {
        public:
            //! The Enumaration type with which the sockets are accessed.
            using EnumType = TEnum;

            //! How many socket declaration we have
            static const auto nSockets = meta::size<meta::list<TSocketDecl...>>::value;

        private:
            //! Filter out all not TMPSocketDeclIn<...> and compare length to TypeList
            template<typename T>
            using isCorrectType = meta::is<T, TMPSocketDeclIn>;
            static_assert(meta::size<meta::filter<meta::list<TSocketDecl...>,
                                                  meta::quote<isCorrectType>>>::value == nSockets,
                          "Not all types in TypeList are of type SocketTraits");

            //! Build the type list
            template<typename PairIndexAndSocketDecl>
            using makeSocketDecl = TMPSocketDeclOut<typename meta::at_c<PairIndexAndSocketDecl, 1>::Id,
                                                    typename meta::at_c<PairIndexAndSocketDecl, 1>::DataType,
                                                    typename meta::at_c<PairIndexAndSocketDecl, 0>>;

            //! For generating the TypeList
            using IndexedTypeList = meta::zip<
                meta::list<
                    meta::as_list<meta::make_index_sequence<nSockets>>,  // range(0,nSockets) -> becomes the SocketDeclaration::Index
                    meta::list<TSocketDecl...>                           // all SocketDeclerations
                    >>;

        public:
            using TypeList = meta::transform<
                IndexedTypeList,
                meta::quote<makeSocketDecl>>;  //! a list of all SocketDeclaration 'meta::list<SocketDeclaration,...>'

        private:
            //! Assert that the IdList has unique ids
            template<typename T>
            using getId  = typename T::Index;
            using IdList = meta::transform<TypeList, meta::quote<getId>>;  //! A list with all ids: meta::list<SocketDeclaration::Id,...>
            static_assert(meta::size<meta::unique<IdList>>::value == nSockets,
                          "You have defined input (or output) enumeration with duplicated integral values"
                          ", e.g: enum class Inputs { Value1 = 1, Value2 = 1 } "
                          "(integrals should not be assigned to the enumeration!!)");

        private:
            template<EnumType searchId>
            struct GetSocketDeclImpl
            {
                template<typename TSocketDeclIdx, typename TIdSearch>
                struct hasId
                {
                    using type = meta::bool_<TSocketDeclIdx::Id::value == TIdSearch::value>;
                };
                using result1 = meta::find_if<TypeList, meta::bind_back<meta::quote_trait<hasId>,          // return true if Id matches searchId
                                                                        meta::size_t<enumToInt(searchId)>  // searchId
                                                                        >>;
                static_assert(!std::is_same<result1, meta::list<>>::value,
                              "The GetSocketDecl failed because the searchId was not found!");

                using result2 = meta::front<result1>;  //! return the first element of the result (find_if returns a meta::list)
                using type    = result2;               //TMPSocketDeclOut<typename result2::Id, typename result2::DataType>;
            };

        public:
            //! Get the SocketDecleration (TMPSocketDeclOut) of socket with id `id`.
            template<EnumType id>
            using Get = typename GetSocketDeclImpl<id>::type;
        };

        //! The user only defines Sockets with these templates.
        template<typename... Args>
        struct OutputSocketDeclarationBase : SocketDeclarationBase<Args...>
        {
        };
        template<typename... Args>
        struct InputSocketDeclarationBase : SocketDeclarationBase<Args...>
        {
        };

        //! These socket definitions are stored inside and returned from SocketDeclarationList
        template<typename... Args>
        struct OutputSocketDeclaration : SocketDeclaration<Args...>
        {
        };
        template<typename... Args>
        struct InputSocketDeclaration : SocketDeclaration<Args...>
        {
        };

        template<typename InputEnum, typename... TSocketDecl>
        struct InputSocketDeclarationList : SocketDeclarationList<InputEnum,
                                                                  InputSocketDeclarationBase,
                                                                  InputSocketDeclaration,
                                                                  TSocketDecl...>
        {
        };

        template<typename OutputEnum, typename... TSocketDecl>
        struct OutputSocketDeclarationList : SocketDeclarationList<OutputEnum,
                                                                   OutputSocketDeclarationBase,
                                                                   OutputSocketDeclaration,
                                                                   TSocketDecl...>
        {
        };

    }  // namespace details

}  // namespace executionGraph
