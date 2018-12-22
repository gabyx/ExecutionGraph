# Try to find the RTTR Library Library https://github.com/fmtlib/fmt.git
include(FindPackageHandleStandardArgs)

# Try to find the library, if it is installed!
# otherwise download it
set(URL "https://github.com/fmtlib/fmt.git")
set(INSTALL_DIR "${ExecutionGraph_EXTERNAL_INSTALL_DIR}/fmt")

message(STATUS "fmt library finding ...")
find_package(fmt QUIET CONFIG PATHS ${INSTALL_DIR})

if(${USE_SUPERBUILD})

    if(NOT TARGET "fmt::fmt-header-only")

        message(STATUS "fmt library: targer not found -> download from ${URL}")

        include(ExternalProject)
        
        ExternalProject_Add(fmt
                            GIT_REPOSITORY      "${URL}"
                            GIT_TAG             "5.2.1"
                            GIT_SHALLOW         ON
                            PREFIX              "${ExecutionGraph_EXTERNAL_BUILD_DIR}/fmt"
                            TIMEOUT 10
                            UPDATE_DISCONNECTED  ON
                            CMAKE_ARGS "-DCMAKE_BUILD_TYPE=Release" 
                                       "-DCMAKE_VERBOSE_MAKEFILE=ON" 
                                       "-DFMT_TEST=OFF" 
                                       "-DFMT_INSTALL=ON"
                                       "-DCMAKE_INSTALL_PREFIX=${INSTALL_DIR}"
                                       "-DCMAKE_POSITION_INDEPENDENT_CODE=ON"
                            INSTALL_DIR "${INSTALL_DIR}")

        message(STATUS "fmt library setup -> build it!")
    endif()

else()
    if(NOT TARGET "fmt::fmt")
        if(${FmtLib_FIND_REQUIRED})
            message(FATAL_ERROR "fmt library could not be found!")
        else()
            message(WARNING "fmt library could not be found!")
        endif()
    endif()
endif()

if(TARGET "fmt::fmt")
    add_library(fmtLib INTERFACE IMPORTED)
    set_property(TARGET fmtLib PROPERTY INTERFACE_LINK_LIBRARIES fmt::fmt)

    message(STATUS "fmt library found! Config File: ${fmt_CONFIG}")
    message(STATUS "fmt library added targets: fmtLib")
endif()