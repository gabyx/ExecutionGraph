# Try to find the boost Library Library 
include(FindPackageHandleStandardArgs)

set(URL "https://dl.bintray.com/boostorg/release/1.68.0/source/boost_1_68_0.tar.bz2")
set(URL_HASH "7f6130bc3cf65f56a618888ce9d5ea704fa10b462be126ad053e80e553d6d8b7")
set(INSTALL_DIR "${ExecutionGraph_EXTERNAL_INSTALL_DIR}/boost")

message(STATUS "boost library finding ...")
find_package(Boost COMPONENTS date_time system CONFIG PATHS ${INSTALL_DIR})

if(${USE_SUPERBUILD})
    if(NOT TARGET "Boost::boost")
        message(STATUS "boost library: not found -> download from ${URL}")

        ExternalProject_Add(boost
                            PREFIX "${ExecutionGraph_EXTERNAL_BUILD_DIR}/boost"
                            URL ${URL}
                            URL_HASH SHA1=${URL_HASH}
                            BUILD_IN_SOURCE 1
                            CONFIGURE_COMMAND ./bootstrap.sh
                                --with-libraries=system
                                --with-libraries=date_time
                                --prefix=<INSTALL_DIR>
                            BUILD_COMMAND
                            ./b2 install link=static variant=release threading=multi runtime-link=static
                            INSTALL_COMMAND ""
                            INSTALL_DIR ${INSTALL_DIR})
                            
        message(STATUS "boost library setup -> build it!")

    endif()
else()
    if(NOT ${Boost_FOUND})
        if(${BoostBeastLib_FIND_REQUIRED})
            message(FATAL_ERROR "boost library could not be found!")
        else()
            message(WARNING "boost library could not be found!")
        endif()
    endif()
endif()

if(TARGET "Boost::boost")
    add_library(boostbeastLib INTERFACE IMPORTED)
    set_property(TARGET boostbeastLib PROPERTY INTERFACE_LINK_LIBRARIES "Boost::system")

    message(STATUS "boostbeast library found! Config File: ${Boost_CONFIG}")
    message(STATUS "boostbeast library added targets: boostbeastLib")
endif()