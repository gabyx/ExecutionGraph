include (ExternalProject)

project("SuperBuild")

set(ExecutionGraph_DEPENDENCIES)
set(EXTRA_CMAKE_ARGS)


include(FindEigen3Lib)
include(FindMetaLib)
include(FindArgsLib)

list (APPEND DEPENDENCIES rttr)
include(FindRTTRLib)


# FIXME add to default target "all"?
# ExternalProject_Add(ExecutionGraph
#                     DEPENDS ${DEPENDENCIES}
#                     SOURCE_DIR ${PROJECT_SOURCE_DIR}
#                     BINARY_DIR ${CMAKE_BINARY_DIR}
#                     CMAKE_ARGS -DUSE_SUPERBUILD=OFF ${EXTRA_CMAKE_ARGS}
#                     INSTALL_COMMAND "")