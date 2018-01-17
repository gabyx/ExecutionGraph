# Try to find the RTTR Library Library https://github.com/rttrorg/rttr.git
# We use this library header only!, so we dont build it!
# defines rttr_INCLUDE_DIR
include(FindPackageHandleStandardArgs)

# Try to find the library, if it is installed!
# otherwise download it
find_package(RTTR REQUIRED)

# if(NOT ${RTTR_FOUND})
#     message(STATUS "rttr library: inlcude dir not found -> download from https://github.com/rttrorg/rttr.git")

#     if(CMAKE_VERSION VERSION_LESS 3.2)
#         set(UPDATE_DISCONNECTED_IF_AVAILABLE "")
#     else()
#         set(UPDATE_DISCONNECTED_IF_AVAILABLE "UPDATE_DISCONNECTED 1")
#     endif()

#     include(ExternalProject)
#     ExternalProject_Add(rttr
#                         GIT_REPOSITORY      https://github.com/rttrorg/rttr.git
#                         GIT_TAG             v0.9.5
#                         PREFIX              "${CMAKE_BINARY_DIR}/external/rttr"
#                         TIMEOUT 10
#                         UPDATE_DISCONNECTED 1
#                         CMAKE_ARGS -DCMAKE_BUILD_TYPE=Release -DCMAKE_VERBOSE_MAKEFILE=ON -DBUILD_STATIC=OFF -DBUILD_BENCHMARKS=OFF
#                         INSTALL_DIR "${CMAKE_BINARY_DIR}/external/install"
#     )

#     ExternalProject_Get_Property(rttr install_dir)
#     set(RTTR_INCLUDE_DIR "${install_dir}/include" CACHE STRING "Meta target" FORCE)
#     set(RTTR_LINK_DIR "${install_dir}/lib" CACHE STRING "Meta target" FORCE)
#     set(RTTR_LIBRARIES "RTTR::Core" CACHE STRING "Meta target" FORCE)
#     set(RTTR_TARGET "rttr" CACHE STRING "Meta target" FORCE)

# else()
#     set(RTTR_TARGET "" CACHE STRING "Meta target" FORCE)
# endif()

# find_package_handle_standard_args(RTTR DEFAULT_MSG rttr_INCLUDE_DIR)
# mark_as_advanced(rttr_INCLUDE_DIR)