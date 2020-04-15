# Try to find the polymorphic value library

include(FindPackageHandleStandardArgs)

# Try to find the library, if it is installed!
# otherwise download it
set(URL "https://github.com/jbcoe/polymorphic_value.git")
set(INSTALL_DIR "${ExecutionGraph_EXTERNAL_INSTALL_DIR}/polymorphic-value")

message(STATUS "polymorphic value library finding ...")
find_package(polymorphic_value CONFIG PATHS ${INSTALL_DIR})

if(${USE_SUPERBUILD})

    if(NOT TARGET "polymorphic_value::polymorphic_value")

        message(STATUS "polymorphic value library: target not found -> download from ${URL}")

        include(ExternalProject)

        ExternalProject_Add(
            polymorphicValue
            GIT_REPOSITORY "${URL}"
            GIT_TAG "master"
            GIT_SHALLOW ON
            PREFIX "${ExecutionGraph_EXTERNAL_BUILD_DIR}/polymorphic-value"
            TIMEOUT 10
            UPDATE_DISCONNECTED ON
            CMAKE_ARGS "-DCMAKE_CXX_COMPILER=${CMAKE_CXX_COMPILER}"
                       "-DCMAKE_C_COMPILER=${CMAKE_C_COMPILER}"
                       "-DCMAKE_BUILD_TYPE=Release"
                       "-DCMAKE_VERBOSE_MAKEFILE=ON"
                       "-DCMAKE_INSTALL_PREFIX=${INSTALL_DIR}"
                       "-DBUILD_TESTING=OFF"
            INSTALL_DIR "${INSTALL_DIR}")

        message(STATUS "polymorphic value library setup -> build it!")
    endif()

else()
    if(NOT TARGET "polymorphic_value::polymorphic_value")
        if(${polymorphicValue_FIND_REQUIRED})
            message(FATAL_ERROR "polymorphic value library could not be found!")
        else()
            message(WARNING "polymorphic value library could not be found!")
        endif()
    endif()
endif()

if(TARGET "polymorphic_value::polymorphic_value")
    add_library(polymorphicValueLib INTERFACE IMPORTED)
    set_property(TARGET polymorphicValueLib PROPERTY INTERFACE_LINK_LIBRARIES
                                                     polymorphic_value::polymorphic_value)

    message(STATUS "polymorphic value library found! Config File: ${polymorphicValue_CONFIG}")
    message(STATUS "polymorphic value library added targets: polymorphicValueLib")
endif()
