# Try to find the memory allocators library

include(FindPackageHandleStandardArgs)

# Try to find the library, if it is installed!
# otherwise download it
set(URL "https://github.com/foonathan/memory.git")
set(INSTALL_DIR "${ExecutionGraph_EXTERNAL_INSTALL_DIR}/memory")

message(STATUS "memory library finding ...")
find_package(foonathan_memory QUIET PATHS ${INSTALL_DIR})

if(${USE_SUPERBUILD})

    if(NOT TARGET "foonathan_memory")

        message(STATUS "memory library: target not found -> download from ${URL}")

        include(ExternalProject)

        ExternalProject_Add(
            memory
            GIT_REPOSITORY "${URL}"
            GIT_TAG master
            GIT_SHALLOW ON
            PREFIX "${ExecutionGraph_EXTERNAL_BUILD_DIR}/memory"
            TIMEOUT 10
            UPDATE_DISCONNECTED ON
            CMAKE_ARGS "-DCMAKE_CXX_COMPILER=${CMAKE_CXX_COMPILER}"
                       "-DCMAKE_C_COMPILER=${CMAKE_C_COMPILER}"
                       "-DCMAKE_BUILD_TYPE=${CMAKE_BUILD_TYPE}"
                       "-DFOONATHAN_MEMORY_BUILD_EXAMPLES=OFF"
                       "-DFOONATHAN_MEMORY_BUILD_TESTS=OFF"
                       "-DCMAKE_INSTALL_PREFIX=${INSTALL_DIR}"
            INSTALL_DIR "${INSTALL_DIR}")

        message(STATUS "memory library setup -> build it!")
    endif()

else()
    if(NOT TARGET "foonathan_memory")
        if(${MemoryLib_FIND_REQUIRED})
            message(FATAL_ERROR "memory library could not be found!")
        else()
            message(WARNING "memory library could not be found!")
        endif()
    endif()
endif()

if(TARGET "foonathan_memory")
    add_library(memoryLib INTERFACE IMPORTED)
    set_property(TARGET memoryLib PROPERTY INTERFACE_LINK_LIBRARIES foonathan_memory)

    message(STATUS "memory library found! Config File: ${foonathan_memory_CONFIG}")
    message(STATUS "memory library added targets: memoryLib")
endif()
