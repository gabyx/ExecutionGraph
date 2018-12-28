# Try to find the Eigen3 Library Library https://bitbucket.org/eigen/eigen
# We use this library header only!, so we dont build it!
# defines EIGEN_INCLUDE_DIR
include(FindPackageHandleStandardArgs)

set(URL "http://bitbucket.org/eigen/eigen/get/3.3.4.zip")
set(TAG "3.2")
set(INSTALL_DIR "${ExecutionGraph_EXTERNAL_INSTALL_DIR}/memory")

# Try to find the library, if it is installed!
# otherwise download it
find_package(Eigen3)
message(STATUS "eigen3 library: finding...")

if(NOT EXISTS "${EIGEN3_INCLUDE_DIR}")
    message(STATUS "eigen3 library: include dir not found -> download from https://bitbucket.org/eigen/eigen")

    include(DownloadProject)
    download_project(PROJ              eigen3
                    PREFIX             "${ExecutionGraph_EXTERNAL_BUILD_DIR}/eigen"
                    # HG_REPOSITORY     ${URL}
                    # HG_TAG            ${TAG}
                    URL                 ${URL}
                    UPDATE_DISCONNECTED 1
                    INSTALL_DIR "${INSTALL_DIR}")

    set(EIGEN3_INCLUDE_DIR "${eigen3_SOURCE_DIR}" CACHE STRING "eigen library (https://bitbucket.org/eigen/eigen) include directory" FORCE)
    # define a path in the cache where to find this downloaded library
    set(EIGEN3_DIR "${CMAKE_BINARY_DIR}/external/install/eigen" CACHE STRING "eigen directory" FORCE)
else()
    message(STATUS "eigen3 library found!")
endif()

find_package_handle_standard_args(Eigen3Lib DEFAULT_MSG EIGEN3_INCLUDE_DIR)
mark_as_advanced(EIGEN3_INCLUDE_DIR)

add_library(eigenLib INTERFACE IMPORTED)
set_property(TARGET eigenLib PROPERTY INTERFACE_INCLUDE_DIRECTORIES ${EIGEN3_INCLUDE_DIR})
message(STATUS "eigen3 library target added: eigenLib")

