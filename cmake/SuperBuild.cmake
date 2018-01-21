
message(STATUS "=================================================================")
message(STATUS "                   SUPER BUILD CONFIGURE [START]                  ")
message(STATUS "=================================================================")

include (ExternalProject)
set(ExecutionGraph_DEPENDENCIES)
set(EXTRA_CMAKE_ARGS)

include(FindEigen3Lib)
include(FindMetaLib)

if(${ExecutionGraph_BUILD_GUI})
    include(FindArgsLib)
    include(FindCrossGUIDLib)
    include(FindCEFLib)
    
    include(FindRTTRLib)
    list (APPEND DEPENDENCIES ${RTTR_TARGET})
endif()

if(${ExecutionGraph_BUILD_TESTS})
    include(FindGoogleTestLib)
    list (APPEND DEPENDENCIES google)
endif()


# ExternalProject_Add(ExecutionGraph
#                     DEPENDS ${DEPENDENCIES}
#                     SOURCE_DIR ${PROJECT_SOURCE_DIR}
#                     BINARY_DIR ${CMAKE_BINARY_DIR}
#                     CMAKE_ARGS -DUSE_SUPERBUILD=OFF ${EXTRA_CMAKE_ARGS}
#                     INSTALL_COMMAND "")


message(STATUS "=================================================================")
message(STATUS "                      SUPER BUILD CONFIGURE [DONE]                ")
message(STATUS "=================================================================")

set(USE_SUPERBUILD OFF CACHE STRING "If a superbuild should be made (building all dependencies)" FORCE)