# this file is executed outside of ExecutionGraph to get the revision describtion
include(cmake/GetGitRevisionDescription.cmake)
git_describe(ExecutionGraph_VERSION "--tags" "--abbrev=0")

if( NOT ExecutionGraph_VERSION OR (NOT "${ExecutionGraph_VERSION}" MATCHES ".") )
message(FATAL_ERROR "ExecutionGraph library version could not be determined or contains errors!, ${ExecutionGraph_VERSION}")
endif()


message(STATUS "${ExecutionGraph_VERSION}")