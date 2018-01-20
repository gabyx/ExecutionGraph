# Try to find the Argument Parser Library https://github.com/Taywee/args 
# defines args_INCLUDE_DIR
include(FindPackageHandleStandardArgs)

if(NOT EXISTS "${args_INCLUDE_DIR}")
    message(STATUS "args library: find includes")
  find_path(args_INCLUDE_DIR
    NAMES args.hxx 
    DOC "Args library header files"
    PATH ${args_DIR}
    )
endif()

if(NOT EXISTS "${args_INCLUDE_DIR}")
    message(STATUS "args library: inlcude dir not found -> download from https://github.com/Taywee/args")

    include(DownloadProject)
    download_project(PROJ               args
                    PREFIX              ${CMAKE_BINARY_DIR}/external/args
                    GIT_REPOSITORY      https://github.com/Taywee/args 
                    GIT_TAG             d8905de2
                    GIT_SHALLOW         ON
                    ${UPDATE_DISCONNECTED_IF_AVAILABLE}
    )

    set(args_INCLUDE_DIR ${args_SOURCE_DIR} CACHE STRING "args library (https://github.com/Taywee/args) include directory" FORCE)
    set(args_DIR "${args_INCLUDE_DIR}" CACHE STRING "args library directory" FORCE)

endif()

find_package_handle_standard_args(ArgsLib DEFAULT_MSG args_INCLUDE_DIR)
mark_as_advanced(args_INCLUDE_DIR)