# this file is executed outside of ExecutionGraph to get the revision describtion
include(cmake/GetGitRevisionDescription.cmake)
git_describe(ExecutionGraph_VERSION "--tags" "--abbrev=0")

if( NOT ExecutionGraph_VERSION )
message(FATAL_ERROR "ExecutionGraph library version could not be determined!, ${ExecutionGraph_VERSION}")
endif()

message(STATUS "${ExecutionGraph_VERSION}")