
if(NOT EXISTS ${CEF_ROOT})

    # Determine the platform.
    if("${CMAKE_SYSTEM_NAME}" STREQUAL "Darwin")
        set(CEF_URL "http://opensource.spotify.com/cefbuilds/cef_binary_3.3282.1733.g9091548_macosx64.tar.bz2")
    elseif("${CMAKE_SYSTEM_NAME}" STREQUAL "Linux")
        set(CEF_URL "http://opensource.spotify.com/cefbuilds/cef_binary_3.3282.1733.g9091548_linux64.tar.bz2")
    elseif("${CMAKE_SYSTEM_NAME}" STREQUAL "Windows")
    set(OS_WINDOWS 1)
        set(CEF_URL "http://opensource.spotify.com/cefbuilds/cef_binary_3.3282.1733.g9091548_windows64.tar.bz2")
    endif()

    if(NOT DEFINED CEF_URL)
        message(FATAL_ERROR "No download URL specified for Chromium Embedded Framework")
    endif()

    message(STATUS "CEF library (super-build): not found -> download from ${CEF_URL}")

    download_project(PROJ               "cefbinaries"
                    PREFIX              "${ExecutionGraph_EXTERNAL_BUILD_DIR}/cefbinaries"
                    URL                 ${CEF_URL}
                    UPDATE_DISCONNECTED 1)

    set(CEF_ROOT "${cefbinaries_SOURCE_DIR}" CACHE PATH "CEF director path" FORCE)

endif()

## Use CEF ################################################
get_filename_component(_DIR "${CMAKE_CURRENT_LIST_FILE}" PATH)
list(APPEND CMAKE_MODULE_PATH "${_DIR}/CEF")
message(STATUS "${CMAKE_MODULE_PATH}")
find_package(CEF REQUIRED)

print_cef_config()
# Include the libcef_dll_wrapper target.
# Comes from the libcef_dll/CMakeLists.txt file in the binary distribution
# directory.
add_subdirectory(${CEF_LIBCEF_DLL_WRAPPER_PATH} libcef_dll_wrapper)
# Logical target used to link the libcef library.
add_logical_target("libcef_lib" "${CEF_LIB_DEBUG}" "${CEF_LIB_RELEASE}")

message(STATUS "CEF Include Path: ${CEF_INCLUDE_PATH}")
list(APPEND INC_DIRS "${CEF_INCLUDE_PATH}/include" "${CEF_INCLUDE_PATH}")
set_target_properties(libcef_lib PROPERTIES 
                                 INTERFACE_INCLUDE_DIRECTORIES "${INC_DIRS}")

print_target_properties("libcef_lib")
###########################################################


message(STATUS "CEF library found: target libcef_lib defined!")
find_package_handle_standard_args(CEFLib DEFAULT_MSG CEF_ROOT)
