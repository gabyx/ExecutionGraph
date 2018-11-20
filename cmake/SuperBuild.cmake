
message(STATUS "=================================================================")
message(STATUS "                   SUPER BUILD CONFIGURE [START]                  ")
message(STATUS "=================================================================")

include (ExternalProject)
set(ExecutionGraph_DEPENDENCIES)
set(EXTRA_CMAKE_ARGS)

find_package(Eigen3Lib) # not really dependency yet
find_package(MetaLib) # needed

#find_package(CerealLib) # for serialization
find_package(FmtLib REQUIRED) # string formatting library
find_package(FlatBuffersLib REQUIRED)
find_package(RTTRLib REQUIRED) 
find_package(CrossGUIDLib REQUIRED)

if(${ExecutionGraph_BUILD_GUI})
    find_package(SpdLogLib REQUIRED) # for logging
    find_package(ArgsLib REQUIRED) # for commandline args
    find_package(MemoryLib REQUIRED) # memory pool

    find_package(CEFLib REQUIRED)
    find_package(BoostBeastLib REQUIRED)
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