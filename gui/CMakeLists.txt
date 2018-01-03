# Download CEF as ###########################################################

# Determine the platform.
if("${CMAKE_SYSTEM_NAME}" STREQUAL "Darwin")
  set(OS_MACOSX 1)
  set(CEF_URL "http://opensource.spotify.com/cefbuilds/cef_binary_3.3239.1710.g85f637a_macosx64.tar.bz2")
elseif("${CMAKE_SYSTEM_NAME}" STREQUAL "Linux")
  set(OS_LINUX 1)
  set(CEF_URL "http://opensource.spotify.com/cefbuilds/cef_binary_3.3239.1710.g85f637a_linux64.tar.bz2")
elseif("${CMAKE_SYSTEM_NAME}" STREQUAL "Windows")
  set(OS_WINDOWS 1)
  set(CEF_URL "http://opensource.spotify.com/cefbuilds/cef_binary_3.3239.1709.g093cae4_windows64.tar.bz2")
endif()

if(NOT DEFINED CEF_URL)
  message(FATAL_ERROR "No download URL specified for Chromium Embedded Framework")
endif()

# Download CEF3
include(DownloadProject)
if (CMAKE_VERSION VERSION_LESS 3.2)
    set(UPDATE_DISCONNECTED_IF_AVAILABLE "")
else()
    set(UPDATE_DISCONNECTED_IF_AVAILABLE "UPDATE_DISCONNECTED 1")
endif()

download_project(PROJ "cefbinaries"
                 PREFIX     ${CMAKE_BINARY_DIR}/external
                 URL        ${CEF_URL}
                 ${UPDATE_DISCONNECTED_IF_AVAILABLE})


# Use the find_package script, we use our copied one because changed some 
# compile flags since it will not compile otherwise...?
set(CMAKE_MODULE_PATH ${CMAKE_MODULE_PATH} "${CMAKE_CURRENT_SOURCE_DIR}/cmake")
set(CEF_ROOT "${cefbinaries_SOURCE_DIR}")
find_package(CEF REQUIRED)

# Include the libcef_dll_wrapper target.
# Comes from the libcef_dll/CMakeLists.txt file in the binary distribution
# directory.
add_subdirectory(${CEF_LIBCEF_DLL_WRAPPER_PATH} libcef_dll_wrapper)
###########################################################

add_subdirectory(cefsimple)
add_subdirectory(executionGraphGui)