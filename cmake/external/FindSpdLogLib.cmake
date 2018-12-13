# Try to find the spdlog Library https://github.com/gabime/spdlog
include(FindPackageHandleStandardArgs)

set(URL "https://github.com/gabime/spdlog")

if(NOT EXISTS "${spdlog_INCLUDE_DIR}")
    message(STATUS "spdlog library: finding...")
    find_path(spdlog_INCLUDE_DIR
            NAMES spdlog/spdlog.hpp 
            DOC "spdlog library header files"
            PATHS "${spdlog_DIR}/include")
endif()

if(NOT EXISTS spdlog_INCLUDE_DIR)
    message(STATUS "spdlog library: inlcude dir not found -> download from ${URL}")

    include(DownloadProject)
    download_project(PROJ               spdlog
                    PREFIX              "${ExecutionGraph_EXTERNAL_BUILD_DIR}/spdlog"
                    GIT_REPOSITORY      ${URL}
                    GIT_TAG             "v1.2.1"
                    GIT_SHALLOW         ON
                    UPDATE_DISCONNECTED 1)

    set(spdlog_INCLUDE_DIR "${spdlog_SOURCE_DIR}/include" CACHE PATH "spdlog library ${URL} include directory" FORCE)
    set(spdlog_DIR "${spdlog_SOURCE_DIR}" CACHE PATH "spdlog library directory" FORCE)

else()
    message(STATUS "spdlog library found!")
endif()

find_package_handle_standard_args(spdlogLib DEFAULT_MSG spdlog_INCLUDE_DIR)
mark_as_advanced(spdlog_INCLUDE_DIR)

add_library(spdlogLib INTERFACE IMPORTED)
set_property(TARGET spdlogLib PROPERTY INTERFACE_INCLUDE_DIRECTORIES ${spdlog_INCLUDE_DIR})
set_property(TARGET spdlogLib PROPERTY INTERFACE_COMPILE_DEFINITIONS "SPDLOG_FMT_EXTERNAL")

message(STATUS "spdlog library added target: spdlogLib")