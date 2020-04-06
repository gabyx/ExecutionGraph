# Find the meta library

include(FindPackageHandleStandardArgs)

if(NOT EXISTS "${meta_INCLUDE_DIR}")
    message(STATUS "meta library: finding...")
    find_path(meta_INCLUDE_DIR
            NAMES meta/meta.hpp 
            DOC "Meta library header files"
            PATHS "${meta_DIR}/include")
endif()

if(NOT EXISTS "${meta_INCLUDE_DIR}")

    message(STATUS "meta library: inlcude dir not found -> download from https://github.com/ericniebler/meta.git")
    include(DownloadProject)
    download_project(PROJ               meta
                    PREFIX              "${ExecutionGraph_EXTERNAL_BUILD_DIR}/meta"
                    GIT_REPOSITORY      https://github.com/ericniebler/meta.git
                    GIT_TAG             master
                    GIT_SHALLOW         ON
                    UPDATE_DISCONNECTED 1
                    CMAKE_ARGS -DCMAKE_CXX_COMPILER=${CMAKE_CXX_COMPILER} -DCMAKE_CXX_FLAGS=${CMAKE_CXX_FLAGS}
    )
  
    set(meta_INCLUDE_DIR "${meta_SOURCE_DIR}/include" CACHE PATH "meta library (https://github.com/ericniebler/meta.git) include directory" FORCE)
    # define a path in the cache where to find this downloaded library (for cmake find_package)
    set(meta_DIR "${meta_SOURCE_DIR}" CACHE PATH "meta library directory" FORCE)
    
else()
    message(STATUS "meta library found!")
endif()

find_package_handle_standard_args(MetaLib DEFAULT_MSG meta_INCLUDE_DIR)
mark_as_advanced(meta_INCLUDE_DIR)

add_library(metaLib INTERFACE IMPORTED)
set_property(TARGET metaLib PROPERTY INTERFACE_INCLUDE_DIRECTORIES ${meta_INCLUDE_DIR})
message(STATUS "meta library target added: metaLib")