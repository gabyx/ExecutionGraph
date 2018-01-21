// ========================================================================================
// Execution Graph
//  Copyright (C) 2014 by Gabriel Nützi <gnuetzi (at) gmail (døt) com>
//
//  This Source Code Form is subject to the terms of the Mozilla Public
//  License, v. 2.0. If a copy of the MPL was not distributed with this
//  file, You can obtain one at http://mozilla.org/MPL/2.0/.
// ========================================================================================

#ifndef executionGraph_Common_MyContainerTypeDefs_hpp
#define executionGraph_Common_MyContainerTypeDefs_hpp

//#define EIGEN_DONT_VECTORIZE
//#define EIGEN_DISABLE_UNALIGNED_ARRAY_ASSERT

#include <Eigen/StdVector>
#include <map>
#include <unordered_map>
#include <vector>

#include <Eigen/Dense>

/** @brief
 *	These are some container definitions
 */
namespace executionGraph
{
    namespace MyContainers
    {
        // Sepcial STL map where the type is 16byte aligned
        template<typename Key, typename Type, typename Comp = std::less<Key>>
        using StdMapAligned = std::map<Key, Type, Comp, Eigen::aligned_allocator<std::pair<const Key, Type>>>;

        // Sepcial STL map where the type is 16byte aligned
        template<typename Key, typename Type, typename Hash = std::hash<Key>, typename Pred = std::equal_to<Key>>
        using StdUMapAligned = std::unordered_map<Key, Type, Hash, Pred, Eigen::aligned_allocator<std::pair<const Key, Type>>>;

        // Special STL vectors where the type is 16byte aligned
        template<typename Type>
        using StdVecAligned = std::vector<Type, Eigen::aligned_allocator<Type>>;
    }  // namespace MyContainers
}  // namespace executionGraph

/**
 * @brief This macro is used to typedef all custom container types.
 */
#define EXECGRAPH_DEFINE_CONTAINER_TYPES                                                                      \
                                                                                                              \
    template<typename Key, typename Type, typename Comp = std::less<Key>>                                     \
    using StdMapAligned = executionGraph::MyContainers::StdMapAligned<Key, Type, Comp>;                       \
                                                                                                              \
    template<typename Key, typename Type, typename Hash = std::hash<Key>, typename Pred = std::equal_to<Key>> \
    using StdUMapAligned = executionGraph::MyContainers::StdUMapAligned<Key, Type, Hash, Pred>;               \
                                                                                                              \
    template<typename Type>                                                                                   \
    using StdVecAligned = executionGraph::MyContainers::StdVecAligned<Type>;

#endif
