
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
#define EXEC_GRAPH_SFINAE_ENABLE_IF(__meta__) typename std::enable_if<(__meta__)>::type* = nullptr
#define EXEC_GRAPH_SFINAE_ENABLE_IMPL_IF(__meta__) typename std::enable_if<(__meta__)>::type*

#endif
