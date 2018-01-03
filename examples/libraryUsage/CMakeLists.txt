cmake_minimum_required(VERSION 2.8)

# only set this variable if ExecutionGraph is not installed system-wide
# set(ExecutionGraph_DIR "path/to/ExecutionGraph/Build/Directory/install/lib/cmake/ExecutionGraph-1.0.8")

# add ExecutionGraph
find_package(ExecutionGraph REQUIRED)

# add simple executable
add_executable(libraryUsage ./main.cpp)

# add include directory
target_include_directories(libraryUsage PRIVATE ${ExecutionGraph_INCLUDE_DIR})

# link with ExecutionGraph
message(STATUS "Linking with: " "${ExecutionGraph_LIBRARIES}")
target_link_libraries(libraryUsage ${ExecutionGraph_LIBRARIES})
target_compile_definitions(libraryUsage PRIVATE ${ExecutionGraph_COMPILE_DEFINITIONS})
target_compile_options(libraryUsage PRIVATE ${ExecutionGraph_COMPILE_OPTIONS})
target_compile_options(libraryUsage PRIVATE $<$<CONFIG:Debug>:${ExecutionGraph_COMPILE_OPTIONS_DBG}>)
