//! ========================================================================================
//!  ExecutionGraph
//!  Copyright (C) 2014 by Gabriel Nützi <gnuetzi (at) gmail (døt) com>
//!
//!  @date Sun Jan 21 2018
//!  @author Gabriel Nützi, gnuetzi (at) gmail (døt) com
//!
//!  This Source Code Form is subject to the terms of the Mozilla Public
//!  License, v. 2.0. If a copy of the MPL was not distributed with this
//!  file, You can obtain one at http://mozilla.org/MPL/2.0/.
//! ========================================================================================

#ifndef execitionGraph_common_CommonTraits_hpp
#define execitionGraph_common_CommonTraits_hpp

#include <meta/meta.hpp>

namespace executionGraph
{
    namespace traits
    {
        //! Trait which tests if T is a template X.
        template<template<typename...> class X, typename T>
        struct isInstantiationOf : meta::bool_<false>
        {
        };
        //! Trait which tests if T is a template X.
        template<template<typename...> class X, typename... Y>
        struct isInstantiationOf<X, X<Y...>> : meta::bool_<true>
        {
        };
    }  // namespace traits
}  // namespace executionGraph

#endif
