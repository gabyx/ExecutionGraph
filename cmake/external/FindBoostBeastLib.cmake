# Try to find the boost Library Library 
include(FindPackageHandleStandardArgs)

set(URL "https://dl.bintray.com/boostorg/release/1.69.0/source/boost_1_69_0.tar.bz2")
set(URL_HASH "8f32d4617390d1c2d16f26a27ab60d97807b35440d45891fa340fc2648b04406")
set(INSTALL_DIR "${ExecutionGraph_EXTERNAL_INSTALL_DIR}/boost")

message(STATUS "boost library finding ...")
set(BOOST_ROOT ${INSTALL_DIR})
find_package(Boost COMPONENTS date_time system)

if(${USE_SUPERBUILD})
    if(NOT TARGET "Boost::boost")
        message(STATUS "boost library: not found -> download from ${URL}")

        ExternalProject_Add(boost
                            PREFIX "${ExecutionGraph_EXTERNAL_BUILD_DIR}/boost"
                            URL ${URL}
                            URL_HASH SHA256=${URL_HASH}
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
    add_library(boostBeastLib INTERFACE IMPORTED)
    set_property(TARGET boostBeastLib PROPERTY INTERFACE_LINK_LIBRARIES "Boost::boost" "Boost::system")
    set_property(TARGET boostBeastLib PROPERTY INTERFACE_COMPILE_DEFINITIONS BOOST_BEAST_USE_STD_STRING_VIEW)
    message(STATUS "boostbeast library found! Config File: ${Boost_CONFIG}")
    message(STATUS "boostbeast library added targets: boostBeastLib")
endif()