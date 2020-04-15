# Find goole test library

include(FindPackageHandleStandardArgs)

if(NOT EXISTS "${googletest_SOURCE_DIR}" OR NOT EXISTS "${googletest_BINARY_DIR}")

    include(DownloadProject)
    download_project(
        PROJ googletest
        PREFIX "${ExecutionGraph_EXTERNAL_BUILD_DIR}/googletest"
        GIT_REPOSITORY https://github.com/google/googletest.git
        GIT_TAG master
        GIT_SHALLOW ON
        UPDATE_DISCONNECTED 1)
    set(googletest_SOURCE_DIR
        "${googletest_SOURCE_DIR}"
        CACHE PATH "googletest library src dir" FORCE)
    set(googletest_BINARY_DIR
        "${googletest_BINARY_DIR}"
        CACHE PATH "googletest library binary dir" FORCE)
else()
    message(STATUS "googletest library found!")
endif()

find_package_handle_standard_args(GoogleTestLib DEFAULT_MSG googletest_SOURCE_DIR
                                  googletest_BINARY_DIR)
mark_as_advanced(googletest_SOURCE_DIR googletest_BINARY_DIR)

# Prevent GoogleTest from overriding our compiler/linker options
# when building with Visual Studio
set(gtest_force_shared_crt
    ON
    CACHE BOOL "" FORCE)
add_subdirectory(${googletest_SOURCE_DIR} ${googletest_BINARY_DIR} EXCLUDE_FROM_ALL)

add_library(googletestLib INTERFACE IMPORTED)
set_property(TARGET googletestLib PROPERTY INTERFACE_LINK_LIBRARIES gtest gmock_main)
message(STATUS "googletest library added target: googletestLib")
