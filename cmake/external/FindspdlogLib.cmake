# Try to find the Argument Parser Library https://github.com/gabime/spdlog
# defines spdlog_INCLUDE_DIR
include(FindPackageHandleStandardArgs)

find_package(spdlog CONFIG REQUIRED)

if(NOT EXISTS spdlog_INCLUDE_DIR)
    message(STATUS "spdlog library: inlcude dir not found -> download from https://github.com/gabime/spdlog")

    include(DownloadProject)
    download_project(PROJ               spdlog
                    PREFIX              "${ExecutionGraph_EXTERNAL_BUILD_DIR}/spdlog"
                    GIT_REPOSITORY      https://github.com/gabime/spdlog 
                    GIT_TAG             master
                    GIT_SHALLOW         OFF
                    UPDATE_DISCONNECTED 1)

    set(spdlog_INCLUDE_DIR "${spdlog_SOURCE_DIR}/include" CACHE PATH "spdlog library (https://github.com/gabime/spdlog) include directory" FORCE)
else()
    message(STATUS "spdlog library found!")
endif()
find_package_handle_standard_args(spdlogLib DEFAULT_MSG spdlog_INCLUDE_DIR)
mark_as_advanced(spdlog_INCLUDE_DIR)

add_library(spdlogLib INTERFACE IMPORTED)
set_property(TARGET spdlogLib PROPERTY INTERFACE_INCLUDE_DIRECTORIES ${spdlog_INCLUDE_DIR})

message(STATUS "spdlog library added target: spdlogLib")