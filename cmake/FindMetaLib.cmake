# Find the Meta include directory
# The following variables are set if Meta is found.
#  Meta_FOUND        - True when the Meta include directory is found.
#  Meta_INCLUDE_DIR  - The path to where the meta include files are.
#  Meta_TARGET       - If meta is downloaded from source (if the user has not installed it system-wide, 
#                       this target is used for add_dependency for any project using meta
# If Meta is not found, Meta_FOUND is set to false.

find_package(PkgConfig)

include(FindPackageHandleStandardArgs)

if(NOT EXISTS "${Meta_INCLUDE_DIR}")
    message(STATUS "meta library: finding...")
    find_path(Meta_INCLUDE_DIR
      NAMES meta/meta.hpp 
      DOC "Meta library header files"
      PATH ${Meta_INCLUDE_DIR}
      )
endif()

if(NOT EXISTS "${Meta_INCLUDE_DIR}")

    message(STATUS "meta library: inlcude dir not found -> download from https://github.com/ericniebler/meta.git")
    include(DownloadProject)
    download_project(PROJ               meta
                    PREFIX              ${CMAKE_BINARY_DIR}/external/meta
                    GIT_REPOSITORY      https://github.com/ericniebler/meta.git
                    GIT_TAG             master
                    GIT_SHALLOW         ON
                    UPDATE_DISCONNECTED 1
                    CMAKE_ARGS -DCMAKE_CXX_COMPILER=${CMAKE_CXX_COMPILER} -DCMAKE_CXX_FLAGS=${CMAKE_CXX_FLAGS}
    )
  
    set(Meta_INCLUDE_DIR "${meta_SOURCE_DIR}/include" CACHE STRING "meta library (https://github.com/ericniebler/meta.git) include directory" FORCE)
    set(Meta_DIR "${Meta_INCLUDE_DIR}" CACHE STRING "meta library directory" FORCE)
endif()

find_package_handle_standard_args(MetaLib DEFAULT_MSG Meta_INCLUDE_DIR)
mark_as_advanced(Meta_INCLUDE_DIR)