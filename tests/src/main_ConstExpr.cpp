// =========================================================================================
//  ExecutionGraph
//  Copyright (C) 2014 by Gabriel Nützi <gnuetzi (at) gmail (døt) com>
//
//  @date Wed Jun 06 2018
//  @author Gabriel Nützi, gnuetzi (at) gmail (døt) com
//
//  This Source Code Form is subject to the terms of the Mozilla Public
//  License, v. 2.0. If a copy of the MPL was not distributed with this
//  file, You can obtain one at http://mozilla.org/MPL/2.0/.
// =========================================================================================

//#include <executionGraph/nodes/LogicNode.hpp>
#include <meta/meta.hpp>
#include <executionGraph/common/EnumFlags.hpp>
#include <executionGraph/common/MetaCommon.hpp>
#include <executionGraph/common/TupleUtil.hpp>
#include <executionGraph/common/TypeDefs.hpp>
#include <executionGraph/nodes/LogicCommon.hpp>
#include <executionGraph/nodes/LogicSocketFlags.hpp>
#include "TestFunctions.hpp"

using namespace executionGraph;

using EFlags = ELogicSocketFlags;
using Flags  = EnumFlags<ELogicSocketFlags>;

struct DescriptionBase
{
    constexpr DescriptionBase(SocketIndex index, EnumFlags<ELogicSocketFlags> flags)
        : m_index(index), m_flags(flags)
    {
    }

    constexpr auto index() const { return m_index; }
    constexpr auto flags() const { return m_flags; }

    SocketIndex m_index;
    Flags m_flags;
};

template<typename TData>
struct Description : public DescriptionBase
{
    using Base       = DescriptionBase;
    using Data       = TData;
    using SocketType = TData;

    constexpr Description(SocketIndex index,
                          EnumFlags<ELogicSocketFlags> flags)
        : Base(index, flags)
    {}
};

namespace details
{
    namespace sockets
    {
        template<typename A, typename B>
        using comp = meta::less<typename meta::at_c<A, 1>::Index,
                                typename meta::at_c<B, 1>::Index>;

        template<typename A>
        using getIdx = typename meta::at_c<A, 1>::Index;

        //! Returns the indices list denoting the sorted descriptions `descs`.
        template<auto& tuple,
                 EG_ENABLE_IF(meta::is<naked<decltype(tuple)>, std::tuple>)>
        constexpr auto sortDescriptions()
        {
            return tupleUtil::sortForward<tuple, [](auto&& descA, auto&& descB){
                descA.index() < descB.index();
            });
        }
    }  // namespace sockets
}  // namespace details

template<typename... Decs>
auto makeSockets(std::tuple<Decs...> descs)
{
    using namespace ::details::sockets;

    return tupleUtil::invoke(
        descs,
        [](auto&&... desc) {
            return std::make_tuple(
                typename naked<decltype(desc)>::SocketType(desc.index())...);
        });
}

template<typename T>
constexpr auto foo(T t)
{
    return 0;
}

MY_TEST(ConstExpr, Test)
{
    using namespace ::details::sockets;
    constexpr auto decl1 = Description<int>(0, {EFlags::Output});
    constexpr auto decl2 = Description<float>(1, {EFlags::Output, EFlags::Optional});
    static_assert(decl1.index() == 0 && decl2.index() == 1, "Wups");
    constexpr auto decls = std::make_tuple(decl1, decl2);

    {
        constexpr auto indices = sortDescriptions(decls);
    }

    // Constexpr evaluation
    {
        auto t      = makeSockets(decls);
        using Tuple = naked<decltype(t)>;
        static_assert(std::is_same_v<std::tuple_element_t<0, Tuple>, int> &&
                          std::is_same_v<std::tuple_element_t<1, Tuple>, float>,
                      "Ups");
    }

    // // Runtime expression evaluation
    // {
    //     auto decls  = std::make_tuple(decl1, decl2);
    //     auto t      = makeSockets(decls);
    //     using Tuple = naked<decltype(t)>;
    //     static_assert(std::is_same_v<std::tuple_element_t<0, Tuple>, int> &&
    //                       std::is_same_v<std::tuple_element_t<1, Tuple>, float>,
    //                   "Ups");
    // }
}

int main(int argc, char** argv)
{
    testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();
}
