# Try to find the Argument Parser Library https://github.com/Taywee/args 
# defines args_INCLUDE_DIR
include(FindPackageHandleStandardArgs)

if(NOT EXISTS "${args_INCLUDE_DIR}")
    message(STATUS "args library: find includes")
  find_path(args_INCLUDE_DIR
    NAMES args.hxx 
    DOC "Args library header files"
    )
endif()

if(NOT EXISTS "${args_INCLUDE_DIR}")
    message(STATUS "args library: inlcude dir not found -> download from https://github.com/Taywee/args")

    if(CMAKE_VERSION VERSION_LESS 3.2)
        set(UPDATE_DISCONNECTED_IF_AVAILABLE "")
    else()
        set(UPDATE_DISCONNECTED_IF_AVAILABLE "UPDATE_DISCONNECTED 1")
    endif()

    include(DownloadProject)
    download_project(PROJ               args
                    PREFIX              ${CMAKE_BINARY_DIR}/external
                    GIT_REPOSITORY      https://github.com/Taywee/args 
                    GIT_TAG             master
                    ${UPDATE_DISCONNECTED_IF_AVAILABLE}
    )

    set(args_INCLUDE_DIR ${args_SOURCE_DIR} CACHE STRING "args library (https://github.com/Taywee/args) include directory" FORCE)
endif()

find_package_handle_standard_args(Args DEFAULT_MSG args_INCLUDE_DIR)
mark_as_advanced(args_INCLUDE_DIR)