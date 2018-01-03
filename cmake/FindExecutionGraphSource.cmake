# - Find ExecutionGraphSource
#
# Users can set the following variables before calling the module:
# ExecutionGraph_SEARCH_PATH - The preferred installation prefix for searching for ExecutionGraph. Set by the user.
#
# ExecutionGraph_INC_DIR - the include dir of ot the ExecutionGraph
# ExecutionGraph_SRC_DIR - the source dir of ot the ExecutionGraph
# ExecutionGraph_CMAKE_DIR - the cmake dir of ot the ExecutionGraph

#Find Include Headers
find_path(ExecutionGraph_INC_DIR 
    NAMES "ExecutionGraph/Config/Config.hpp.in.cmake"
    HINTS ${ExecutionGraph_SEARCH_PATH}
    PATH_SUFFIXES
    "include"
    "ExecutionGraph"
    "ExecutionGraph/include"
    NO_DEFAULT_PATHS
)

#Find Root Dir
get_filename_component(result "${ExecutionGraph_INC_DIR}/../" ABSOLUTE)
set(ExecutionGraph_ROOT_DIR  "${result}" CACHE STRING "ExecutionGraph Root Dir")

#Find Source Dir
set(ExecutionGraph_SRC_DIR  "${ExecutionGraph_ROOT_DIR}/src" CACHE STRING "ExecutionGraph Source Dir")
#Find Cmake Dir
set(ExecutionGraph_CMAKE_DIR  "${ExecutionGraph_ROOT_DIR}/cmake" CACHE STRING "ExecutionGraph Cmake Dir")

include(FindPackageHandleStandardArgs)
find_package_handle_standard_args("ExecutionGraphSource" DEFAULT_MSG ExecutionGraph_INC_DIR ExecutionGraph_SRC_DIR ExecutionGraph_CMAKE_DIR)

mark_as_advanced( ExecutionGraph_FORCE_MSGLOG_LEVEL)
set(ExecutionGraph_FORCE_MSGLOG_LEVEL "0" CACHE STRING "Force the message log level (0-3), 0 = use deubg/release settings in LogDefines.hpp!")