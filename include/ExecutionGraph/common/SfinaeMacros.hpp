
// ========================================================================================
//  executionGraph
//  Copyright (C) 2014 by Gabriel Nützi <gnuetzi (at) gmail (døt) com>
//
//  This Source Code Form is subject to the terms of the Mozilla Public
//  License, v. 2.0. If a copy of the MPL was not distributed with this
//  file, You can obtain one at http://mozilla.org/MPL/2.0/.
// ========================================================================================

#ifndef ExecutionGraph_Common_SfinaeMacros_hpp
#define ExecutionGraph_Common_SfinaeMacros_hpp

#include <type_traits>

/** use this macro in sfinae selection in templated functions
 *
 *   template<typename TopoType,
 *            typename
 * std::enable_if<MPILayer::isGridTopoBuilder<TopoType>::value>::type * =
 * nullptr
 *            typename
 * std::enable_if<MPILayer::isPolymorphic<TopoType>::value>::type * = nullptr
 *   >
 *   void foo(){}
 *
 *   becomes =>
 *
 *   template<typename TopoType,
 *           SFINAE_ENABLE_IF( MPILayer::isGridTopoBuilder<TopoType>::value ),
 *           SFINAE_ENABLE_IF( MPILayer::isPolymorphic<TopoType>::value ),
 *   >
 *   void foo(){}
 */
#define EXECGRAPH_SFINAE_ENABLE_IF(__meta__) typename std::enable_if<(__meta__)>::type* = nullptr
#define EXECGRAPH_SFINAE_ENABLE_IMPL_IF(__meta__) typename std::enable_if<(__meta__)>::type*

#endif
