# Try to find the RTTR Library Library https://github.com/rttrorg/rttr.git
# We use this library header only!, so we dont build it!
# defines rttr_INCLUDE_DIR
include(FindPackageHandleStandardArgs)

# Try to find the library, if it is installed!
# otherwise download it
message(STATUS "rttr library: finding...")
find_package(RTTR QUIET)

if(NOT ${RTTR_FOUND})
    message(STATUS "rttr library: inlcude dir not found -> download from https://github.com/gabyx/rttr.git")

    include(ExternalProject)
    set(INSTALL_DIR ${CMAKE_BINARY_DIR}/external/install/rttr)
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

    set(RTTR_DIR "${INSTALL_DIR}" CACHE STRING "rttr library directory" FORCE)
endif()
