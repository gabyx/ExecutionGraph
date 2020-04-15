if(NOT EXISTS ${CEF_ROOT})

    # Determine the platform.
    set(CEF_DSYM_URL "")
    if("${CMAKE_SYSTEM_NAME}" STREQUAL "Darwin")
        set(CEF_DSYM_URL
            "http://opensource.spotify.com/cefbuilds/cef_binary_3.3282.1741.gcd94615_macosx64_debug_symbols.tar.bz2"
        )
        set(CEF_URL
            "http://opensource.spotify.com/cefbuilds/cef_binary_3.3282.1733.g9091548_macosx64.tar.bz2"
        )
    elseif("${CMAKE_SYSTEM_NAME}" STREQUAL "Linux")
        set(CEF_URL
            "http://opensource.spotify.com/cefbuilds/cef_binary_3.3282.1733.g9091548_linux64.tar.bz2"
        )
    elseif("${CMAKE_SYSTEM_NAME}" STREQUAL "Windows")
        set(OS_WINDOWS 1)
        set(CEF_URL
            "http://opensource.spotify.com/cefbuilds/cef_binary_3.3282.1733.g9091548_windows64.tar.bz2"
        )
    endif()

    if(NOT DEFINED CEF_URL)
        message(FATAL_ERROR "No download URL specified for Chromium Embedded Framework")
    endif()

    message(STATUS "CEF library (super-build): not found -> download from ${CEF_URL}")

    download_project(
        PROJ "cefbinaries"
        PREFIX "${ExecutionGraph_EXTERNAL_BUILD_DIR}/cefbinaries"
        URL ${CEF_URL}
        UPDATE_DISCONNECTED 1)

    set(CEF_ROOT
        "${cefbinaries_SOURCE_DIR}"
        CACHE PATH "CEF director path" FORCE)

    # Download the dSYM file for Debugging.
    if(${ExecutionGraph_USE_CEF_DSYM_FILE} AND (NOT ${CEF_DSYM_URL} STREQUAL ""))
        download_project(
            PROJ "cefbinaries-dsymfile"
            PREFIX "${ExecutionGraph_EXTERNAL_BUILD_DIR}/cefbinaries"
            URL ${CEF_DSYM_URL}
            UPDATE_DISCONNECTED 1)
        set(CEF_DSYM_FILE
            "${cefbinaries-dsym_SOURCE_DIR}/Chromium Embedded Framework.dSYM"
            CACHE PATH "CEF dSYM file" FORCE)
    endif()
endif()

message(STATUS "CEF library found: target libcef_lib defined!")
find_package_handle_standard_args(CEFLib DEFAULT_MSG CEF_ROOT)
