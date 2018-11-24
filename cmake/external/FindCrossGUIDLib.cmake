# Try to find the crossguid Library Library 
include(FindPackageHandleStandardArgs)

set(URL "https://github.com/gabyx/crossguid.git")
set(INSTALL_DIR "${ExecutionGraph_EXTERNAL_INSTALL_DIR}/crossguid")

message(STATUS "crossguid library finding ...")
find_package(crossguid CONFIG PATHS ${INSTALL_DIR})

if(${USE_SUPERBUILD})
    if(NOT TARGET "crossguid")
        message(STATUS "crossguid library: include dir not found -> download from ${URL}")

        include(ExternalProject)
        ExternalProject_Add(crossguid
                            PREFIX              "${ExecutionGraph_EXTERNAL_BUILD_DIR}/crossguid"
                            GIT_REPOSITORY      "${URL}"
                            GIT_TAG             master
                            GIT_SHALLOW         ON
                            TIMEOUT 10
                            UPDATE_DISCONNECTED  ON
                            CMAKE_ARGS "-DCMAKE_BUILD_TYPE=Release" 
                                       "-DCMAKE_VERBOSE_MAKEFILE=ON" 
                                       "-DCROSSGUID_TESTS=OFF" 
                                       "-DCMAKE_INSTALL_PREFIX=${INSTALL_DIR}" 
                                       "-DCMAKE_POSITION_INDEPENDENT_CODE=ON"
                            INSTALL_DIR "${INSTALL_DIR}")
                            
        message(STATUS "crossguid library setup -> build it!")
    endif()
else()
    if(NOT TARGET "crossguid")
        if(${CrossGUIDLib_FIND_REQUIRED})
            message(FATAL_ERROR "crossguid library could not be found!")
        else()
            message(WARNING "crossguid library could not be found!")
        endif()
    endif()
endif()

if(TARGET "crossguid")
    add_library(crossguidLib INTERFACE IMPORTED)
    set_property(TARGET crossguidLib PROPERTY INTERFACE_LINK_LIBRARIES "crossguid")

    message(STATUS "crossguid library found! Config File: ${crossguid_CONFIG}")
    message(STATUS "crossguid library added targets: crossguidLib")
endif()