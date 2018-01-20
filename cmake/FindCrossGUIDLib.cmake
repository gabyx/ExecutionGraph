# Try to find the crossguid Library Library https://github.com/graeme-hill/crossguid.git
# We use this library header only!, so we dont build it!
# defines crossguid_INCLUDE_DIR
include(FindPackageHandleStandardArgs)

if(NOT EXISTS "${crossguid_INCLUDE_DIR}")
    message(STATUS "crossguid library: include dir not found -> download from https://github.com/graeme-hill/crossguid.git")

    include(DownloadProject)
    download_project(PROJ               crossguid
                    PREFIX              ${CMAKE_BINARY_DIR}/external/crossguid
                    GIT_REPOSITORY      https://github.com/graeme-hill/crossguid.git
                    GIT_TAG             e6c8514
                    GIT_SHALLOW         ON
                    ${UPDATE_DISCONNECTED_IF_AVAILABLE}
    )
    set(crossguid_SOURCE_DIR "${crossguid_SOURCE_DIR}" CACHE STRING "crossguid library src" FORCE)
    set(crossguid_BINARY_DIR "${crossguid_BINARY_DIR}" CACHE STRING "crossguid library src" FORCE)
    set(crossguid_INCLUDE_DIR "${crossguid_SOURCE_DIR}" CACHE STRING "crossguid library (https://github.com/graeme-hill/crossguid.git) include directory" FORCE)
    set(crossguid_TARGET "xg" CACHE STRING "crossguid target (https://github.com/graeme-hill/crossguid.git) include directory" FORCE)
else()
    message(STATUS "crossguid library found!")
endif()

find_package_handle_standard_args(CrossGUIDLib DEFAULT_MSG crossguid_INCLUDE_DIR crossguid_TARGET)
mark_as_advanced(crossguid_INCLUDE_DIR)