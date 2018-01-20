# Try to find the Eigen3 Library Library https://bitbucket.org/eigen/eigen
# We use this library header only!, so we dont build it!
# defines EIGEN_INCLUDE_DIR
include(FindPackageHandleStandardArgs)

# Try to find the library, if it is installed!
# otherwise download it
find_package(Eigen3)
message(STATUS "eigen3 library: finding...")

if(NOT EXISTS "${EIGEN3_INCLUDE_DIR}")
    message(STATUS "eigen3 library: include dir not found -> download from https://bitbucket.org/eigen/eigen")

    include(DownloadProject)
    download_project(PROJ              eigen3
                    PREFIX             ${CMAKE_BINARY_DIR}/external/eigen
                    # HG_REPOSITORY     https://bitbucket.org/eigen/eigen
                    # HG_TAG            3.2
                    URL                 http://bitbucket.org/eigen/eigen/get/3.3.4.zip
                    UPDATE_DISCONNECTED 1
                    INSTALL_DIR "${CMAKE_BINARY_DIR}/external/install/eigen")

    set(EIGEN3_INCLUDE_DIR "${eigen3_SOURCE_DIR}" CACHE STRING "eigen library (https://bitbucket.org/eigen/eigen) include directory" FORCE)
    set(EIGEN3_DIR "${CMAKE_BINARY_DIR}/external/install/eigen" CACHE STRING "eigen directory" FORCE)
else()
    message(STATUS "eigen3 library found!")
endif()

find_package_handle_standard_args(Eigen3Lib DEFAULT_MSG EIGEN3_INCLUDE_DIR)
mark_as_advanced(EIGEN3_INCLUDE_DIR EIGEN3_DIR)
message(STATUS "eigen3 library: include dir: ${EIGEN3_INCLUDE_DIR}")