#The CMake Minimum version that is required.
cmake_minimum_required(VERSION 3.4)

list(APPEND CMAKE_MODULE_PATH "${CMAKE_CURRENT_SOURCE_DIR}/cmake/" "${CMAKE_CURRENT_SOURCE_DIR}/cmake/external")
message(STATUS "Module path is now set to: " ${CMAKE_MODULE_PATH} )

#Optional Stuff ========================================================
mark_as_advanced( CMAKE_DEBUG_POSTFIX )
set(CMAKE_DEBUG_POSTFIX "-dbg" CACHE STRING "Debug postfix for library/executable")

mark_as_advanced( ExecutionGraph_BUILD_LIBRARY )
set(ExecutionGraph_BUILD_LIBRARY ON CACHE BOOL "Build a shared library")

mark_as_advanced( ExecutionGraph_BUILD_TESTS)
set(ExecutionGraph_BUILD_TESTS ON CACHE BOOL "Build the tests")

mark_as_advanced( ExecutionGraph_BUILD_BENCHMARKS)
set(ExecutionGraph_BUILD_BENCHMARKS OFF CACHE BOOL "Build the benchmarks")

mark_as_advanced( ExecutionGraph_BUILD_EXAMPLE )
set(ExecutionGraph_BUILD_EXAMPLE OFF CACHE BOOL "Build the example, the library is automatically built if this is true")

mark_as_advanced( ExecutionGraph_BUILD_GUI )
set(ExecutionGraph_BUILD_GUI ON CACHE BOOL "Build Graphical User Interface for the Execution Graph")

mark_as_advanced( ExecutionGraph_FORCE_MSGLOG_LEVEL)
set(ExecutionGraph_FORCE_MSGLOG_LEVEL "DEBUG" CACHE STRING "Force the message log level (TRACE, DEBUG, INFO, WARN, ERROR, FATAL)")

mark_as_advanced( ExecutionGraph_THROW_IF_BAD_SOCKET_CASTS)
set(ExecutionGraph_THROW_IF_BAD_SOCKET_CASTS ON CACHE BOOL "Force an exception if a bad socket cast happens! (if false, the exception is thrown only in Debug mode!)")

#~ mark_as_advanced( ExecutionGraph_USE_OPENMP)
#~ set(ExecutionGraph_USE_OPENMP ON CACHE BOOL "Try to use OpenMp for parallel speedup")

set(ExecutionGraph_EXTERNAL_BUILD_DIR_POSTFIX "external" CACHE STRING "Folder where all external shit is build!")
set(ExecutionGraph_EXTERNAL_BUILD_DIR "${CMAKE_BINARY_DIR}/${ExecutionGraph_EXTERNAL_BUILD_DIR_POSTFIX}" )
set(ExecutionGraph_EXTERNAL_INSTALL_DIR "${ExecutionGraph_EXTERNAL_BUILD_DIR}/install")
#=======================================================================

set(CMAKE_VERBOSE_MAKEFILE OFF)

# Load print macros
include(PrintMacros)
# Load important modules
include(FirstCMakeRun)
# Speed up the build
include(cotire) 

# Define the Project ===============================================
set(ExecutionGraphProjectName "ExecutionGraph")
message(STATUS "Project name is: " ${ExecutionGraphProjectName})
project(${ExecutionGraphProjectName})

# here we specify the installation directory
if(CMAKE_INSTALL_PREFIX_INITIALIZED_TO_DEFAULT)
  set(CMAKE_INSTALL_PREFIX "${CMAKE_BINARY_DIR}/install" CACHE PATH  "ExecutionGraph install prefix" FORCE)
endif()

# Handle superbuild first ===============
option (USE_SUPERBUILD "Whether or not a superbuild should be invoked" ON)
if (USE_SUPERBUILD)
  project (SUPERBUILD NONE)
  # execute the superbuild (this script will be invoked again without the
  # USE_SUPERBUILD option this time)
  include (SuperBuild)
  message("SuperBuild configured! -> exit...")
  return()                      # stop processing this file further
endif()
# Set the superbuild off into the cache! super build should only run once!
set(USE_SUPERBUILD OFF CACHE STRING "If a superbuild should be made (building all dependencies)" FORCE)

message(STATUS " =================================================================")
message(STATUS "                   NORMAL BUILD CONFIGURE [START]                 ")
message(STATUS " =================================================================")

set(ExecutionGraph_BINARY_DIR ${PROJECT_BINARY_DIR})
set(ExecutionGraph_ROOT_DIR   ${PROJECT_SOURCE_DIR})

if("${CMAKE_BUILD_TYPE}" STREQUAL "")
    message(STATUS "Detect build type from folder")
    #Detect Build Type if Debug appears in CMAKE_BINARY_DIR path
    string(REGEX MATCH ".*(Debug|debug).*" DEBUG_MATCH ${CMAKE_BINARY_DIR})
    if(DEBUG_MATCH)
        set(CMAKE_BUILD_TYPE Debug CACHE STRING "The build type for makefile based generators")
        set(PROJECTNAMEPREFIX Debug)
    else()
        set(CMAKE_BUILD_TYPE Release CACHE STRING "The build type for makefile based generators")
        set(PROJECTNAMEPREFIX Release)
    endif()
else()
    set(PROJECTNAMEPREFIX ${CMAKE_BUILD_TYPE})
endif()

# use CCACHE if possible
find_program(CCACHE_FOUND ccache)
if(CCACHE_FOUND)
    set(CMAKE_CXX_COMPILER_LAUNCHER ccache)
endif(CCACHE_FOUND)

message("Default Compiler Flags: '${CMAKE_CXX_FLAGS}' | '${CMAKE_CXX_FLAGS_DEBUG}' | '${CMAKE_CXX_FLAGS_RELEASE}'")
message("Default Linker Flags: '${CMAKE_EXE_LINKER_FLAGS}' | '${CMAKE_SHARED_LINKER_FLAGS}' | '${CMAKE_STATIC_LINKER_FLAGS}'")

#Add some multithreaded build support 
mark_as_advanced(MULTITHREADED_BUILD)
set(MULTITHREADED_BUILD ON CACHE BOOL "Parallel build with as many threads as possible!")
if(MULTITHREADED_BUILD)
	if(${CMAKE_GENERATOR} MATCHES "Unix Makefiles")
            file(COPY ${ExecutionGraph_ROOT_DIR}/cmake/parallelmake.sh DESTINATION ${PROJECT_BINARY_DIR}
                FILE_PERMISSIONS OWNER_READ OWNER_WRITE OWNER_EXECUTE GROUP_READ GROUP_EXECUTE WORLD_READ WORLD_EXECUTE
                NO_SOURCE_PERMISSIONS
            )
            set(CMAKE_MAKE_PROGRAM "${PROJECT_BINARY_DIR}/parallelmake.sh")
            message(STATUS "Set make program to ${PROJECT_BINARY_DIR}/parallelmake.sh")
    elseif(MSVC)
      set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS}" "/MP")
      message(STATUS "Added parallel build arguments to CMAKE_CXX_FLAGS: ${CMAKE_CXX_FLAGS}")
    endif()
endif()
# ======================================================================


# Dependencies =========================================================
# Define Eigen
find_package(Eigen3Lib REQUIRED)
set(EIGEN_INCLUDE_DIR ${EIGEN3_INCLUDE_DIR})
list(APPEND ExecutionGraph_INC_DIRS_DEP ${EIGEN_INCLUDE_DIR})

# Try to find Meta 
find_package(MetaLib REQUIRED)
list(APPEND ExecutionGraph_INC_DIRS_DEP ${Meta_INCLUDE_DIR})
set(ExecutionGraph_DEPENDING_TARGETS_DEP ${Meta_TARGET})

# Try to find the Argumentparser
find_package(ArgsLib REQUIRED)
list(APPEND ExecutionGraph_INC_DIRS_DEP ${args_INCLUDE_DIR})

# Try to find the RTTR Library (use our own script)
find_package(RTTRLib REQUIRED)
list(APPEND ExecutionGraph_DEPENDING_TARGETS_DEP "RTTR::Core")
#list(APPEND ExecutionGraph_INC_DIRS_DEP ${RTTR_INCLUDE_DIR})

# Try to find the crossguid Library (use our own script)
find_package(CrossGUIDLib REQUIRED)
list(APPEND ExecutionGraph_INC_DIRS_DEP ${crossguid_INCLUDE_DIR})
# build the library
set(XG_TESTS OFF)
add_subdirectory(${crossguid_SOURCE_DIR} ${crossguid_BINARY_DIR} EXCLUDE_FROM_ALL)

find_package(PugiXMLLib)
if(PUGIXML_FOUND)
    list(APPEND ExecutionGraph_INC_DIRS_DEP ${PUGIXML_INCLUDE_DIR} )
    list(APPEND ExecutionGraph_LIBRARIES_DEP ${PUGIXML_LIBRARIES})
    set(ExecutionGraph_SUPPORT_XML ON)
endif()

#~ if(ExecutionGraph_USE_OPENMP)

  #~ include(ProcessorCount)
  #~ ProcessorCount(NPROCESSES)

  #~ find_package(OpenMP)
  #~ if(OpenMP_FOUND)
    #~ set(MYPROJECT_CXX_FLAGS "${MYPROJECT_CXX_FLAGS} ${OpenMP_CXX_FLAGS}")
    #~ set(ExecutionGraph_OPENMP_SUPPORT ON)

    #~ mark_as_advanced( ExecutionGraph_OPENMP_USE_NTHREADS)
    #~ set(ExecutionGraph_OPENMP_USE_NTHREADS OFF CACHE BOOL "If the number of threads should be fixed, otherwise determined at runtime!")

    #~ if(ExecutionGraph_OPENMP_USE_NTHREADS)
        #~ set(ExecutionGraph_OPENMP_NTHREADS ${NPROCESSES} CACHE STRING "The number of threads to use.")
        #~ mark_as_advanced( ExecutionGraph_OPENMP_NTHREADS )
    #~ endif()

  #~ else()
    #~ set(ExecutionGraph_OPENMP_SUPPORT ON)
  #~ endif()
#~ endif()
# ======================================================================

# Define all ExecutionGraph Source files
include(DefineExecutionGraphSources)

set(ExecutionGraph_INCLUDE_DIR "${PROJECT_SOURCE_DIR}/include/ExecutionGraph")
set(ExecutionGraph_EXTERNAL_INCLUDE_DIRS "${PROJECT_SOURCE_DIR}/external/")

#Include all relevant sources
include_all_source_ExecutionGraph(ExecutionGraph_SRC
                                  ExecutionGraph_INC
                                  ExecutionGraph_INC_DIRS
                                  ExecutionGraph_DEPENDING_TARGETS
                                  ${PROJECT_SOURCE_DIR} ${ExecutionGraph_BINARY_DIR} )

list(APPEND ExecutionGraph_INC_DIRS ${ExecutionGraph_INC_DIRS_DEP})
set(ExecutionGraph_LINK_LIBS ${ExecutionGraph_LIBRARIES_DEP})
list(APPEND ExecutionGraph_DEPENDING_TARGETS ${ExecutionGraph_DEPENDING_TARGETS_DEP})

if( ${ExecutionGraph_BUILD_EXAMPLE} OR 
    ${ExecutionGraph_BUILD_TESTS} OR 
    ${ExecutionGraph_BUILD_GUI} OR 
    ${ExecutionGraph_BUILD_BENCHMARKS})
  set(ExecutionGraph_BUILD_LIBRARY On CACHE BOOL "Build a shared library" FORCE)
endif()
if(ExecutionGraph_BUILD_LIBRARY)
    add_subdirectory(lib)
endif()

if(ExecutionGraph_BUILD_TESTS)
    enable_testing()
    add_custom_target(build_and_test ${CMAKE_CTEST_COMMAND} -V)
    add_subdirectory(tests)
endif()

if(ExecutionGraph_BUILD_BENCHMARKS)
    enable_testing()
    add_custom_target(build_and_benchmark ${CMAKE_CTEST_COMMAND} -V)
    add_subdirectory(benchmarks)
endif()

if(ExecutionGraph_BUILD_EXAMPLE)
    add_subdirectory(examples)
endif()

if(ExecutionGraph_BUILD_GUI)
    add_subdirectory(gui)
endif()

message(STATUS "=================================================================")
message(STATUS "                   NORMAL BUILD CONFIGURE [DONE ]                 ")
message(STATUS "=================================================================")