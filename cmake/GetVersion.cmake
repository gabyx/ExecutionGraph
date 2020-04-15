macro(get_version_ExecutionGraph)

    # Get the version of the project ExecutionGraph
    execute_process(
        COMMAND "cmake" "-P" "cmake/GetGitRevisionDescriptionExtern.cmake"
        WORKING_DIRECTORY "${ExecutionGraph_ROOT_DIR}"
        OUTPUT_VARIABLE ExecutionGraph_VERSION
        ERROR_VARIABLE Error)

    if(Error)
        message(FATAL_ERROR "Error in getting version of ExecutionGraph ${Error}" FATAL)
    endif()

    string(REGEX REPLACE "^.*v([0-9]+)\\..*" "\\1" ExecutionGraph_VERSION_MAJOR
                         "${ExecutionGraph_VERSION}")
    string(REGEX REPLACE "^.*v[0-9]+\\.([0-9]+).*" "\\1" ExecutionGraph_VERSION_MINOR
                         "${ExecutionGraph_VERSION}")
    string(REGEX REPLACE "^.*v[0-9]+\\.[0-9]+\\.([0-9]+).*" "\\1" ExecutionGraph_VERSION_PATCH
                         "${ExecutionGraph_VERSION}")
    string(REGEX REPLACE "^.*v[0-9]+\\.[0-9]+\\.[0-9]+(.*)" "\\1" ExecutionGraph_VERSION_SHA1
                         "${ExecutionGraph_VERSION}")
    set(ExecutionGraph_VERSION_STRING
        "${ExecutionGraph_VERSION_MAJOR}.${ExecutionGraph_VERSION_MINOR}.${ExecutionGraph_VERSION_PATCH}"
    )
    MESSAGE(
        STATUS "ExecutionGraph Version: ${ExecutionGraph_VERSION_STRING} extracted from git tags!")

    if("${ExecutionGraph_VERSION_MAJOR}" STREQUAL ""
       OR "${ExecutionGraph_VERSION_MINOR}" STREQUAL ""
       OR "${ExecutionGraph_VERSION_PATCH}" STREQUAL "")

        MESSAGE(STATUS "ExecutionGraph Version is bogus!")
    endif()

endmacro()
