# Try to find the cereal library
include(FindPackageHandleStandardArgs)

set(URL "https://github.com/USCiLab/cereal.git")

if(NOT EXISTS "${cereal_INCLUDE_DIR}")
    message(STATUS "cereal library: finding...")
    find_path(cereal_INCLUDE_DIR
            NAMES cereal/cereal.hpp 
            DOC "cereal library header files"
            PATHS "${cereal_DIR}/include")
endif()

if(NOT EXISTS "${cereal_INCLUDE_DIR}")

    message(STATUS "cereal library: inlcude dir not found -> download from ${URL}")
    include(DownloadProject)
    download_project(PROJ               cereal
                    PREFIX              "${ExecutionGraph_EXTERNAL_BUILD_DIR}/cereal"
                    GIT_REPOSITORY      "${URL}"
                    GIT_TAG             master
                    GIT_SHALLOW         ON
                    UPDATE_DISCONNECTED 1
    )
  
    set(cereal_INCLUDE_DIR "${cereal_SOURCE_DIR}/include" CACHE PATH "cereal library (${URL}) include directory" FORCE)
    # define a path in the cache where to find this downloaded library (for cmake find_package)
    set(cereal_DIR "${cereal_SOURCE_DIR}" CACHE PATH "cereal library directory" FORCE)
    
else()
    message(STATUS "cereal library found!")
endif()

find_package_handle_standard_args(cerealLib DEFAULT_MSG cereal_INCLUDE_DIR)
mark_as_advanced(cereal_INCLUDE_DIR)

add_library(cerealLib INTERFACE IMPORTED)
set_property(TARGET cerealLib PROPERTY INTERFACE_INCLUDE_DIRECTORIES ${cereal_INCLUDE_DIR})
message(STATUS "cereal library target added: cerealLib")