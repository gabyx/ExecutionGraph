
// ========================================================================================
// Execution Graph
//  Copyright (C) 2014 by Gabriel Nützi <gnuetzi (at) gmail (døt) com>
//
//  This Source Code Form is subject to the terms of the Mozilla Public
//  License, v. 2.0. If a copy of the MPL was not distributed with this
//  file, You can obtain one at http://mozilla.org/MPL/2.0/.
// ========================================================================================

#pragma once

#include <type_traits>

/* ---------------------------------------------------------------------------------------*/
/*!
    SFINAE macros:

    usage:

    @code
    template<typename T,
              EXECGRAPH_SFINAE_ENABLE_IF( T::value == 2 ),
              EXECGRAPH_SFINAE_ENABLE_IF( T::size == 5 ),
    >
    void foo(){}
    @endcode

    @code
    template<typename T, typename = void> struct A;
    template<typename T> 
    struct A<T, EXECGRAPH_SFINAE_ENABLE_IF_CLASS(T::a ==3)>
    {
        // code
    }
    template<typename T> 
    struct A<T, EXECGRAPH_SFINAE_ENABLE_IF_CLASS(T::a ==4)>
    {
        // code
    }
    @endcode


    @date Thu Feb 22 2018
    @author Gabriel Nützi, gnuetzi (at) gmail (døt) com
 */
/* ---------------------------------------------------------------------------------------*/

#define EXECGRAPH_SFINAE_ENABLE_IF_IMPL(__meta__) typename std::enable_if<(__meta__)>::type*
#define EXECGRAPH_SFINAE_ENABLE_IF(__meta__) EXECGRAPH_SFINAE_ENABLE_IF_IMPL(__meta__) = nullptr

#define EXECGRAPH_SFINAE_ENABLE_IF_CLASS(__meta__) typename std::enable_if<(__meta__)>::type
