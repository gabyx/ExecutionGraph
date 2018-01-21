

include(FindPackageHandleStandardArgs)

if(NOT EXISTS "${googletest_SOURCE_DIR}" OR 
   NOT EXISTS "${googletest_BINARY_DIR}")

    include(DownloadProject)
    download_project(PROJ               googletest
                    PREFIX              "${ExecutionGraph_EXTERNAL_DIR}/googletest"
                    GIT_REPOSITORY      https://github.com/google/googletest.git
                    GIT_TAG             master
                    UPDATE_DISCONNECTED 1
    )
    set(googletest_SOURCE_DIR "${googletest_SOURCE_DIR}" CACHE STRING "googletest library src dir" FORCE)
    set(googletest_BINARY_DIR "${googletest_BINARY_DIR}" CACHE STRING "googletest library binary dir" FORCE)

endif()

# Prevent GoogleTest from overriding our compiler/linker options
# when building with Visual Studio
set(gtest_force_shared_crt ON CACHE BOOL "" FORCE)

find_package_handle_standard_args(GoogleTestLib DEFAULT_MSG googletest_SOURCE_DIR googletest_BINARY_DIR)
mark_as_advanced(googletest_SOURCE_DIR googletest_BINARY_DIR)
message(STATUS "googletest library: src dir: ${googletest_SOURCE_DIR}")