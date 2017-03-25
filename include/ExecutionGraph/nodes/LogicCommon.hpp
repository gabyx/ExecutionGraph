// ========================================================================================
//  ExecutionGraph
//  Copyright (C) 2014 by Gabriel Nützi <gnuetzi (at) gmail (døt) com>
//
//  This Source Code Form is subject to the terms of the Mozilla Public
//  License, v. 2.0. If a copy of the MPL was not distributed with this
//  file, You can obtain one at http://mozilla.org/MPL/2.0/.
// ========================================================================================

#ifndef ExecutionGraph_nodes_LogicCommon_hpp
#define ExecutionGraph_nodes_LogicCommon_hpp

#include <vector>

#include "ExecutionGraph/common/TypeDefs.hpp"
#include "ExecutionGraph/common/SfinaeMacros.hpp"
#include "ExecutionGraph/common/EnumClassHelper.hpp"
#include "ExecutionGraph/nodes/LogicSocketDefaultTypes.hpp"

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

using NodeIdType   = unsigned int;
using SocketIdType = NodeIdType;
using IndexType    = unsigned int;

template<typename TSocketTypes = SocketDefaultTypes>
struct GeneralConfig
{
    using SocketTypes  = TSocketTypes;
    using NodeBaseType = LogicNode<GeneralConfig>;

    using SocketInputBaseType  = LogicSocketInputBase<GeneralConfig>;
    using SocketOutputBaseType = LogicSocketOutputBase<GeneralConfig>;

    template<typename T>
    using SocketInputType = LogicSocketInput<T, GeneralConfig>;
    template<typename T>
    using SocketOutputType = LogicSocketOutput<T, GeneralConfig>;
};

#define EXEC_GRAPH_TYPEDEF_CONFIG(__CONFIG__)                              \
    using Config               = __CONFIG__;                              \
    using SocketTypes          = typename Config::SocketTypes;            \
    using NodeBaseType         = typename Config::NodeBaseType;           \
    using SocketInputBaseType  = typename Config::SocketInputBaseType;    \
    using SocketOutputBaseType = typename Config::SocketOutputBaseType;   \
    template<typename T>                                                  \
    using SocketInputType = typename Config::template SocketInputType<T>; \
    template<typename T>                                                  \
    using SocketOutputType = typename Config::template SocketOutputType<T>





//! Some type traits for input/output socket definitions in derived classes from LogicNode
namespace details
{
        template <template <typename...> class X, typename T>
        struct isInstantiationOf : meta::bool_<false> {};

        template <template <typename...> class X, typename... Y>
        struct isInstantiationOf<X, X<Y...>> : meta::bool_<true> {};

        template<typename TId, typename TData>
        struct SocketDeclaration
        {
            using Id       = TId;
            using DataType = TData;
        };

        template<typename TId, typename TData, typename TIndex>
        struct SocketDeclarationIdx : SocketDeclaration<TId,TData>
        {
            using Index    = TIndex;
        };

        //! Make the SocketDeclerationIndex list


        template<template<typename...> class TMPSocketDecl, typename... TSocketDecl>
        struct SocketDeclarationList
        {
            //! How many socket declaration we have
            static const auto nSockets = meta::size<meta::list<TSocketDecl...>>::value;

            // Filter out all not TMPSocketDecl<...> and compare length to TypeList
            template<typename T>
            using isCorrectType = isInstantiationOf<TMPSocketDecl, T>;
            static_assert(meta::size< meta::filter<meta::list<TSocketDecl...>,
                                                   meta::quote_trait< isCorrectType >>
                                    >::value == nSockets ,
                          "Not all types in TypeList are of type SocketTraits");


            //! Build the type list
            template<typename PairIndexAndSocketDecl>
            using makeSocketDecl = SocketDeclarationIdx<typename meta::at_c<PairIndexAndSocketDecl,1>::Id,
                                                        typename meta::at_c<PairIndexAndSocketDecl,1>::DataType,
                                                        typename meta::at_c<PairIndexAndSocketDecl,0>>;

            using EnumeratedTypeList = meta::zip<
                                                meta::list<
                                                    meta::as_list<meta::make_index_sequence<nSockets>> ,
                                                    meta::list<TSocketDecl...>
                                                >
                                            >;
            using TypeList = meta::transform<
                                                EnumeratedTypeList,
                                                meta::quote<makeSocketDecl>
                                            >;

            template<std::size_t Index>
            using DataType = typename meta::at_c<TypeList,Index>::DataType;
        };

        template<typename... Args>
        struct OutputSocketDeclaration : SocketDeclaration<Args...> {};
        template<typename... Args>
        struct InputSocketDeclaration  : SocketDeclaration<Args...> {};

        template<typename... TSocketDecl>
        struct InputSocketDeclarationList : SocketDeclarationList<InputSocketDeclaration, TSocketDecl...>{};

        template<typename... TSocketDecl>
        struct OutputSocketDeclarationList : SocketDeclarationList<OutputSocketDeclaration, TSocketDecl...>{};

} // end details

//! Some handy macro to use when inheriting from LogicNode.
#define EXEC_GRAPH_DEFINE_SOCKET_TRAITS(InputEnum,OutputEnum)                                           \
    template<InputEnum id,  typename TData>                                                        \
    using InSocketDecl = executionGraph::details::InputSocketDeclaration< meta::size_t<enumToInt(id)>,TData>;                 \
    template<OutputEnum id,  typename TData>                                                        \
    using OutSocketDecl = executionGraph::details::OutputSocketDeclaration< meta::size_t<enumToInt(id)>,TData>; \
    template<typename... TSockets >                                                                     \
    using InSocketDeclList = executionGraph::details::InputSocketDeclarationList<TSockets...>;   \
    template<typename... TSockets >                                                                     \
    using OutSocketDeclList = executionGraph::details::OutputSocketDeclarationList<TSockets...>;

}  // namespace

#endif
