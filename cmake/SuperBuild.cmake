
message(STATUS "=================================================================")
message(STATUS "                   SUPER BUILD CONFIGURE [START]                  ")
message(STATUS "=================================================================")

include (ExternalProject)
set(ExecutionGraph_DEPENDENCIES)
set(EXTRA_CMAKE_ARGS)

find_package(Eigen3Lib) # not really dependency yet
find_package(MetaLib) # needed

#find_package(CerealLib) # for serialization
find_package(fmtLib) # string formatting library
find_package(SpdLogLib) # for logging
find_package(ArgsLib) # for commandline args
find_package(CrossGUIDLib) # guid
find_package(MemoryLib) # memory pool

if(${ExecutionGraph_BUILD_GUI})
    find_package(CEFLib)
    find_package(RTTRLib)
    find_package(FlatBuffersLib)
endif()

if(${ExecutionGraph_BUILD_TESTS})
    find_package(GoogleTestLib)
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