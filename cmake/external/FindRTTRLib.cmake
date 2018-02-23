# Try to find the RTTR Library Library https://github.com/rttrorg/rttr.git
# We use this library header only!, so we dont build it!
# defines rttr_INCLUDE_DIR
include(FindPackageHandleStandardArgs)

# Try to find the library, if it is installed!
# otherwise download it
set(URL "https://github.com/rttrorg/rttr.git")
set(INSTALL_DIR "${ExecutionGraph_EXTERNAL_INSTALL_DIR}/rttr")
set(RTTR_COMPONENTS CORE)

message(STATUS "rttr library finding ...")
find_package(RTTR QUIET CONFIG COMPONENTS ${RTTR_COMPONENTS} PATHS ${INSTALL_DIR})

if(${USE_SUPERBUILD})

    if(NOT TARGET "RTTR::Core")

        message(STATUS "rttr library: targer not found -> download from ${URL}")

        include(ExternalProject)
        
        ExternalProject_Add(rttr
                            GIT_REPOSITORY      "${URL}"
                            GIT_TAG             master
                            GIT_SHALLOW         ON
                            PREFIX              "${ExecutionGraph_EXTERNAL_BUILD_DIR}/rttr"
                            TIMEOUT 10
                            UPDATE_DISCONNECTED  ON
                            CMAKE_ARGS "-DCMAKE_BUILD_TYPE=Release" "-DCMAKE_VERBOSE_MAKEFILE=ON" "-DBUILD_STATIC=OFF" 
                                        "-DBUILD_BENCHMARKS=OFF" "-DBUILD_UNIT_TESTS=OFF" "-DCMAKE_INSTALL_PREFIX=${INSTALL_DIR}"
                            INSTALL_DIR "${INSTALL_DIR}")

        message(STATUS "rttr library setup -> build it!")
    endif()

else()
    if(NOT TARGET "RTTR::Core")
        if(${RTTRLib_FIND_REQUIRED})
            message(FATAL_ERROR "rttr library could not be found!")
        else()
            message(WARNING "rttr library could not be found!")
        endif()
    endif()
endif()

if(TARGET "RTTR::Core")
    add_library(rttrLib INTERFACE IMPORTED)
    set_property(TARGET rttrLib PROPERTY INTERFACE_LINK_LIBRARIES RTTR::Core)

    message(STATUS "rttr library found! Config File: ${RTTR_CONFIG}")
    message(STATUS "rttr library added targets: rttrLib")
endif()