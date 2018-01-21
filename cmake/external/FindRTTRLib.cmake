# Try to find the RTTR Library Library https://github.com/rttrorg/rttr.git
# We use this library header only!, so we dont build it!
# defines rttr_INCLUDE_DIR
include(FindPackageHandleStandardArgs)

# Try to find the library, if it is installed!
# otherwise download it
set(INSTALL_DIR "${ExecutionGraph_EXTERNAL_INSTALL_DIR}/rttr")

set(RTTR_COMPONENTS CORE)

if(${USE_SUPERBUILD})
    message(STATUS "rttr library (super-build): finding...:")
    find_package(RTTR QUIET CONFIG OPTIONAL_COMPONENTS CORE)

    if(NOT TARGET "RTTR::Core")

        message(STATUS "rttr library: targer not found -> download from https://github.com/gabyx/rttr.git")

        include(ExternalProject)
        
        ExternalProject_Add(rttr
                            GIT_REPOSITORY      https://github.com/gabyx/rttr.git
                            GIT_TAG             disable-warnings
                            GIT_SHALLOW         ON
                            PREFIX              "${CMAKE_BINARY_DIR}/external/rttr"
                            TIMEOUT 10
                            UPDATE_DISCONNECTED  ON
                            CMAKE_ARGS "-DCMAKE_BUILD_TYPE=Release" "-DCMAKE_VERBOSE_MAKEFILE=ON" "-DBUILD_STATIC=OFF" 
                                        "-DBUILD_BENCHMARKS=OFF" "-DBUILD_UNIT_TESTS=OFF" "-DCMAKE_INSTALL_PREFIX=${INSTALL_DIR}"
                            INSTALL_DIR "${INSTALL_DIR}")

        set(RTTR_DIR "${INSTALL_DIR}/cmake" CACHE STRING "rttr library directory" FORCE)
        message(STATUS "rttr library downloaded -> build it!")
    else()
        message(STATUS "rttr library found! no build necessary")
    endif()

else()

    message(STATUS "rttr library: finding...:")
    find_package(RTTR QUIET CONFIG OPTIONAL_COMPONENTS ${RTTR_COMPONENTS})
    if(NOT TARGET "RTTR::Core")
        # Try again but with the super build install dir:
        set(RTTR_DIR ${INSTALL_DIR})
        message(STATUS "rttr library: finding in super build folder...")
        find_package(RTTR QUIET CONFIG OPTIONAL_COMPONENTS ${RTTR_COMPONENTS})
    endif()

    if(NOT TARGET "RTTR::Core" AND ${RTTRLib_FIND_REQUIRED})
        message(FATAL_ERROR "rttr library could not be found!")
    else()
        message(STATUS "rttr library found!")
    endif()
endif()