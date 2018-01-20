if(NOT EXISTS "${CEF_ROOT}")

    # Determine the platform.
    if("${CMAKE_SYSTEM_NAME}" STREQUAL "Darwin")
        set(CEF_URL "http://opensource.spotify.com/cefbuilds/cef_binary_3.3239.1710.g85f637a_macosx64.tar.bz2")
    elseif("${CMAKE_SYSTEM_NAME}" STREQUAL "Linux")
        set(CEF_URL "http://opensource.spotify.com/cefbuilds/cef_binary_3.3239.1710.g85f637a_linux64.tar.bz2")
    elseif("${CMAKE_SYSTEM_NAME}" STREQUAL "Windows")
    set(OS_WINDOWS 1)
        set(CEF_URL "http://opensource.spotify.com/cefbuilds/cef_binary_3.3239.1709.g093cae4_windows64.tar.bz2")
    endif()

    if(NOT DEFINED CEF_URL)
        message(FATAL_ERROR "No download URL specified for Chromium Embedded Framework")
    endif()

    message(STATUS "CEF library: not found -> download from ${CEF_URL}")

    # Download CEF3
    include(DownloadProject)
    if (CMAKE_VERSION VERSION_LESS 3.2)
        set(UPDATE_DISCONNECTED_IF_AVAILABLE "")
    else()
        set(UPDATE_DISCONNECTED_IF_AVAILABLE "UPDATE_DISCONNECTED 1")
    endif()

    download_project(PROJ "cefbinaries"
                    PREFIX     ${CMAKE_BINARY_DIR}/external/cefbinaries
                    URL        ${CEF_URL}
                    ${UPDATE_DISCONNECTED_IF_AVAILABLE})

    set(CEF_ROOT "${cefbinaries_SOURCE_DIR}" CACHE STRING "CEF director path" FORCE)

else()
    message(STATUS "CEF library found - CEF_ROOT defined!")
endif()

find_package_handle_standard_args(CEF DEFAULT_MSG CEF_ROOT )
mark_as_advanced(CEF_ROOT)