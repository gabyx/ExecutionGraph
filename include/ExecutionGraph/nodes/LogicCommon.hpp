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

using NodeId   = unsigned int;
using SocketId = NodeId;
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
        struct SocketDeclarationBase
        {
            using Id       = TId;    //! The integral identifier for this socket. (This is the converted enum defined by the user).
            using DataType = TData;  //! The data type of the socket.
        };

        template<typename TId, typename TData, typename TIndex>
        struct SocketDeclaration : SocketDeclarationBase<TId,TData>
        {
            using Index    = TIndex; //! The index corresponding to the storage of all input (or output) sockets in LogicNode.
        };

        //! Make the SocketDeclerationIndex list
        template<typename TEnum,
                 template<typename...> class TMPSocketDeclIn,
                 template<typename...> class TMPSocketDeclOut,
                 typename... TSocketDecl>
        struct SocketDeclarationList
        {
        public:
            //! The Enumaration type with which the sockets are accessed.
            using EnumType = TEnum;

            //! How many socket declaration we have
            static const auto nSockets = meta::size<meta::list<TSocketDecl...>>::value;

        private:
            // Filter out all not TMPSocketDeclIn<...> and compare length to TypeList
            template<typename T>
            using isCorrectType = isInstantiationOf<TMPSocketDeclIn, T>;
            static_assert(meta::size< meta::filter<meta::list<TSocketDecl...>,
                                                   meta::quote_trait< isCorrectType >>
                                    >::value == nSockets ,
                          "Not all types in TypeList are of type SocketTraits");


            //! Build the type list
            template<typename PairIndexAndSocketDecl>
            using makeSocketDecl = TMPSocketDeclOut<typename meta::at_c<PairIndexAndSocketDecl,1>::Id,
                                                     typename meta::at_c<PairIndexAndSocketDecl,1>::DataType,
                                                     typename meta::at_c<PairIndexAndSocketDecl,0>>;

            //! For generating the TypeList
            using IndexedTypeList = meta::zip<
                                                meta::list<
                                                    meta::as_list<meta::make_index_sequence<nSockets>> , // range(0,nSockets) -> becomes the SocketDeclaration::Index
                                                    meta::list<TSocketDecl...>                           // all SocketDeclerations
                                                >
                                            >;

        public:
            using TypeList = meta::transform<
                                                IndexedTypeList,
                                                meta::quote<makeSocketDecl>
                                            >; // a list of all SocketDeclaration 'meta::list<SocketDeclaration,...>'

        private:
        //! Assert that the IdList has unique ids
        template<typename T> using getId = typename T::Index;
        using IdList = meta::transform<TypeList, meta::quote<getId> >;          //! A list with all ids: meta::list<SocketDeclaration::Id,...>
        static_assert( meta::size<meta::unique<IdList>>::value == nSockets ,
                       "You have defined input (or output) enumeration with duplicated integral values"
                       ", e.g: enum class Inputs { Value1 = 1, Value2 = 1 } (integrals should not be assigned to the enumeration!!)" );



        private:

            template<EnumType searchId>
            struct GetSocketDeclImpl
            {
                template< typename TSocketDeclIdx, typename TIdSearch>
                struct hasId{
                    using type = meta::bool_<TSocketDeclIdx::Id::value == TIdSearch::value>;
                };
                using result1 = meta::find_if<TypeList, meta::bind_back< meta::quote_trait<hasId>,               // return true if Id matches searchId
                                                                         meta::size_t<enumToInt(searchId)> // searchId
                                                                       >
                                             >;
                static_assert(! std::is_same<result1, meta::list<>>::value, "The GetSocketDecl failed because the searchId was not found!");

                using result2 = meta::front<result1>; //! return the first element of the result (find_if returns a meta::list)
                using type  = result2 ; //TMPSocketDeclOut<typename result2::Id, typename result2::DataType>;
            };

        public:

            //! Get the SocketDecleration (TMPSocketDeclOut) of socket with id `id`.
            template<EnumType id>
            using Get = typename GetSocketDeclImpl<id>::type;
        };

        //! The user only defines Sockets with these templates.
        template<typename... Args>
        struct OutputSocketDeclarationBase : SocketDeclarationBase<Args...> {};
        template<typename... Args>
        struct InputSocketDeclarationBase  : SocketDeclarationBase<Args...> {};

        //! These socket definitions are stored inside and returned from SocketDeclarationList
        template<typename... Args>
        struct OutputSocketDeclaration : SocketDeclaration<Args...> {};
        template<typename... Args>
        struct InputSocketDeclaration  : SocketDeclaration<Args...> {};


        template<typename InputEnum, typename... TSocketDecl>
        struct InputSocketDeclarationList : SocketDeclarationList<InputEnum, InputSocketDeclarationBase, InputSocketDeclaration, TSocketDecl...>{};

        template<typename OutputEnum, typename... TSocketDecl>
        struct OutputSocketDeclarationList : SocketDeclarationList<OutputEnum, OutputSocketDeclarationBase, OutputSocketDeclaration, TSocketDecl...>{};

} // end details

//! Some handy macro to use when inheriting from LogicNode.
#define EXEC_GRAPH_DEFINE_SOCKET_TRAITS(InputEnum,OutputEnum)                                           \
    template<InputEnum id,  typename TData>                                                        \
    using InSocketDecl = executionGraph::details::InputSocketDeclarationBase< meta::size_t<enumToInt(id)>,TData>;                 \
    template<OutputEnum id,  typename TData>                                                        \
    using OutSocketDecl = executionGraph::details::OutputSocketDeclarationBase< meta::size_t<enumToInt(id)>,TData>; \
    template<typename... TSockets >                                                                     \
    using InSocketDeclList = executionGraph::details::InputSocketDeclarationList<InputEnum,TSockets...>;   \
    template<typename... TSockets >                                                                     \
    using OutSocketDeclList = executionGraph::details::OutputSocketDeclarationList<OutputEnum,TSockets...>;

}  // namespace

#endif
