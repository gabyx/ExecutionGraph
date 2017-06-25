cmake_minimum_required(VERSION 2.8)

SET(PROJECT_NAME libraryUsage)


# only set this variable if ExecutionGraph is not installed system-wide
# set(ExecutionGraph_DIR "path/to/ExecutionGraph/Build/Directory/install/lib/cmake/ExecutionGraph-1.0.8")

# add ExecutionGraph
find_package(ExecutionGraph REQUIRED)

# add CXX flags from library
SET(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} ${ExecutionGraph_CXX_FLAGS}")

# add include directory
include_directories(${ExecutionGraph_INCLUDE_DIR})

# add simple executable
ADD_EXECUTABLE(libraryUsage ./main.cpp)

# link with ExecutionGraph
MESSAGE(STATUS "Linking with: " "${ExecutionGraph_LIBRARIES}")
target_link_libraries(libraryUsage ${ExecutionGraph_LIBRARIES})
target_compile_definitions(libraryUsage PRIVATE ${ExecutionGraph_COMPILE_DEFINITIONS})
target_compile_options(libraryUsage PRIVATE $<$<CONFIG:Debug>:${ExecutionGraph_COMPILE_OPTIONS_DBG}>)

# add dependecies (if any)
if( NOT "${ExecutionGraph_DEPENDING_TARGETS}" STREQUAL "")
    ADD_DEPENDENCIES(libraryUsage ${ExecutionGraph_DEPENDING_TARGETS})
endif()
