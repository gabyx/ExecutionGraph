# Try to find the RTTR Library Library https://github.com/rttrorg/rttr.git
# We use this library header only!, so we dont build it!
# defines rttr_INCLUDE_DIR
include(FindPackageHandleStandardArgs)

# Try to find the library, if it is installed!
# otherwise download it
find_package(RTTR)

if(NOT ${RTTR_FOUND})
    message(STATUS "rttr library: inlcude dir not found -> download from https://github.com/rttrorg/rttr.git")

    if(CMAKE_VERSION VERSION_LESS 3.2)
        set(UPDATE_DISCONNECTED_IF_AVAILABLE "")
    else()
        set(UPDATE_DISCONNECTED_IF_AVAILABLE "UPDATE_DISCONNECTED 1")
    endif()

    include(DownloadProject)
    download_project(PROJ               rttr
                    PREFIX              ${CMAKE_BINARY_DIR}/external
                    GIT_REPOSITORY      https://github.com/rttrorg/rttr.git
                    GIT_TAG             master
                    ${UPDATE_DISCONNECTED_IF_AVAILABLE}
    )

    set(rttr_INCLUDE_DIR "${rttr_SOURCE_DIR}/src" CACHE STRING "rttr library (https://github.com/rttrorg/rttr.git) include directory" FORCE)
endif()

find_package_handle_standard_args(RTTR DEFAULT_MSG rttr_INCLUDE_DIR)
mark_as_advanced(rttr_INCLUDE_DIR)