# Try to find the crossguid Library Library https://github.com/graeme-hill/crossguid.git
# We use this library header only!, so we dont build it!
# defines crossguid_INCLUDE_DIR
include(FindPackageHandleStandardArgs)

if(NOT "${crossguid_FOUND}")
    message(STATUS "crossguid library: inlcude dir not found -> download from https://github.com/graeme-hill/crossguid.git")

    if(CMAKE_VERSION VERSION_LESS 3.2)
        set(UPDATE_DISCONNECTED_IF_AVAILABLE "")
    else()
        set(UPDATE_DISCONNECTED_IF_AVAILABLE "UPDATE_DISCONNECTED 1")
    endif()

    include(DownloadProject)
    download_project(PROJ               crossguid
                    PREFIX              ${CMAKE_BINARY_DIR}/external
                    GIT_REPOSITORY      https://github.com/graeme-hill/crossguid.git
                    GIT_TAG             master
                    ${UPDATE_DISCONNECTED_IF_AVAILABLE}
    )

    set(crossguid_INCLUDE_DIR "${crossguid_SOURCE_DIR}" CACHE STRING "crossguid library (https://github.com/graeme-hill/crossguid.git) include directory" FORCE)
    set(crossguid_TARGET "xg")

endif()

find_package_handle_standard_args(crossguid DEFAULT_MSG crossguid_INCLUDE_DIR crossguid_TARGET)
mark_as_advanced(crossguid_INCLUDE_DIR)