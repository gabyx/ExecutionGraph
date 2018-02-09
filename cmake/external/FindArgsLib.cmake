# Try to find the Argument Parser Library https://github.com/Taywee/args 
# defines args_INCLUDE_DIR
include(FindPackageHandleStandardArgs)

if(NOT EXISTS "${args_INCLUDE_DIR}")
    message(STATUS "args library: find includes")
  find_path(args_INCLUDE_DIR
        NAMES args.hxx 
        DOC "Args library header files"
        PATH ${args_DIR})
endif()

if(NOT EXISTS "${args_INCLUDE_DIR}")
    message(STATUS "args library: inlcude dir not found -> download from https://github.com/Taywee/args")

    include(DownloadProject)
    download_project(PROJ               args
                    PREFIX              "${ExecutionGraph_EXTERNAL_BUILD_DIR}/args"
                    GIT_REPOSITORY      https://github.com/Taywee/args 
                    GIT_TAG             master #d8905de2
                    GIT_SHALLOW         OFF
                    UPDATE_DISCONNECTED 1)

    set(args_INCLUDE_DIR ${args_SOURCE_DIR} CACHE PATH "args library (https://github.com/Taywee/args) include directory" FORCE)
    set(args_DIR "${args_INCLUDE_DIR}" CACHE PATH "args library directory" FORCE)

else()
    message(STATUS "args library found!")
endif()

find_package_handle_standard_args(ArgsLib DEFAULT_MSG args_INCLUDE_DIR)
mark_as_advanced(args_INCLUDE_DIR)

add_library(argsLib INTERFACE IMPORTED)
set_property(TARGET argsLib PROPERTY INTERFACE_INCLUDE_DIRECTORIES ${args_INCLUDE_DIR})

message(STATUS "args library added target: argsLib")