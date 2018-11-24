cmake_minimum_required(VERSION 3.10)

project(libraryUsage C CXX)

set(CMAKE_CXX_STANDARD 17)

# only set this variable if ExecutionGraph is not installed system-wide
# set(ExecutionGraph_DIR "path/to/ExecutionGraph/Build/Directory/install/lib/cmake/ExecutionGraph-1.0.8")
message("ExecutionGraph_DIR set to '${ExecutionGraph_DIR}'")

# add ExecutionGraph
find_package(ExecutionGraph CONFIG)

# add simple executable
add_executable(libraryUsage ./main.cpp)

# link with ExecutionGraph
target_link_libraries(libraryUsage PRIVATE ExecutionGraph::Core)

