#The CMake Minimum version that is required.
cmake_minimum_required(VERSION 3.10)

list(APPEND CMAKE_MODULE_PATH "${CMAKE_CURRENT_SOURCE_DIR}/cmake/" "${CMAKE_CURRENT_SOURCE_DIR}/cmake/external")
message(STATUS "Module path is now set to: " ${CMAKE_MODULE_PATH} )

# Define the Project ===============================================
set(ExecutionGraphProjectName "ExecutionGraph")
message(STATUS "Project name is: " ${ExecutionGraphProjectName})
project(${ExecutionGraphProjectName})

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)
set(CMAKE_VERBOSE_MAKEFILE ON)
set(CMAKE_EXPORT_COMPILE_COMMANDS ON) # Needed by VS Code for Intellisense

if(OS_MACOSX)
    if(NOT "${CMAKE_OSX_SYSROOT}" )
        message(FATAL_ERROR "OSX_SYSROOT needs to be defined!")
    endif()
endif()

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
set(ExecutionGraph_FORCE_MSGLOG_LEVEL "TRACE" CACHE STRING "Force the message log level (TRACE, DEBUG, INFO, WARN, ERROR, FATAL)")

mark_as_advanced( ExecutionGraph_THROW_IF_BAD_SOCKET_CASTS)
set(ExecutionGraph_THROW_IF_BAD_SOCKET_CASTS ON CACHE BOOL "Force an exception if a bad socket cast happens! (if false, the exception is thrown only in Debug mode!)")

#mark_as_advanced( ExecutionGraph_USE_OPENMP)
#set(ExecutionGraph_USE_OPENMP ON CACHE BOOL "Try to use OpenMp for parallel speedup")

set(ExecutionGraph_EXTERNAL_BUILD_DIR_POSTFIX "external" CACHE STRING "Folder where all external shit is build!")
set(ExecutionGraph_EXTERNAL_BUILD_DIR "${CMAKE_BINARY_DIR}/${ExecutionGraph_EXTERNAL_BUILD_DIR_POSTFIX}" )
set(ExecutionGraph_EXTERNAL_INSTALL_DIR "${ExecutionGraph_EXTERNAL_BUILD_DIR}/install")
#=======================================================================

# Load print macros
include(PrintMacros)
# Load important modules
include(FirstCMakeRun)
# Speed up the build
include(cotire) 

# Define Install Directories
# Install stuff (default location is not some where on the system! for safty reasons)
if(CMAKE_INSTALL_PREFIX_INITIALIZED_TO_DEFAULT)
    set(CMAKE_INSTALL_PREFIX "${CMAKE_BINARY_DIR}/install" CACHE STRING "Install prefix (e.g. /usr/local/)" FORCE)
endif()
if(UNIX)
    include(GNUInstallDirs)
    set(ExecutionGraph_INC_INSTALL_DIR "${CMAKE_INSTALL_INCDIR}") 
    set(ExecutionGraph_RUNTIME_INSTALL_DIR "${CMAKE_INSTALL_BINDIR}") 
    set(ExecutionGraph_LIBRARY_INSTALL_DIR "${CMAKE_INSTALL_LIBDIR}")
    set(ExecutionGraph_ARCHIVE_INSTALL_DIR "${CMAKE_INSTALL_LIBDIR}")
    set(ExecutionGraph_FRAMEWORK_INSTALL_DIR "${CMAKE_INSTALL_LIBDIR}")

    set(ExecutionGraph_INSTALL_FULL_LIBDIR "${CMAKE_INSTALL_FULL_LIBDIR}")

    set(ExecutionGraph_CMAKE_CONFIG_INSTALL_DIR "${CMAKE_INSTALL_DATADIR}/ExecutionGraph/cmake")
    set(ExecutionGraph_ADDITIONAL_FILES_INSTALL_DIR "${CMAKE_INSTALL_DATADIR}/ExecutionGraph")
else(WINDOWS)
    set(ExecutionGraph_INC_INSTALL_DIR "${CMAKE_INSTALL_INCDIR}") 
    set(ExecutionGraph_RUNTIME_INSTALL_DIR "bin") 
    set(ExecutionGraph_LIBRARY_INSTALL_DIR "bin")
    set(ExecutionGraph_ARCHIVE_INSTALL_DIR "lib")
    set(ExecutionGraph_FRAMEWORK_INSTALL_DIR "bin")

    set(ExecutionGraph_CMAKE_CONFIG_INSTALL_DIR "${CMAKE_INSTALL_PREFIX}/cmake")
    set(ExecutionGraph_ADDITIONAL_FILES_INSTALL_DIR "${CMAKE_INSTALL_PREFIX}")
endif()

# set the rpath for executables
set(CMAKE_SKIP_BUILD_RPATH OFF)            # use, i.e. don't skip the full RPATH for the build tree
set(CMAKE_BUILD_WITH_INSTALL_RPATH OFF)    # when building, don't use the install RPATH already (but later on when installing)
set(CMAKE_INSTALL_RPATH_USE_LINK_PATH OFF) # NO automatic rpath for INSTALL
if(APPLE)
  set(MACOSX_RPATH ON CACHE STRING "Set this to off if you dont want @rpath in install names") # uses a install name @rpath/... for libraries
elseif(UNIX)
elseif(WINDOWS)
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
add_library(ExecutionGraph::Core-Dependencies INTERFACE IMPORTED)
# Define Eigen
find_package(Eigen3Lib REQUIRED)
list(APPEND ExecutionGraph_LINK_TARGETS_CORE_DEP "eigenLib")
# Try to find Meta 
find_package(MetaLib REQUIRED)
list(APPEND ExecutionGraph_LINK_TARGETS_CORE_DEP "metaLib")
# Try to find the spdlog
find_package(SpdLogLib REQUIRED)
list(APPEND ExecutionGraph_LINK_TARGETS_CORE_DEP "spdlogLib")
# Try to find the Argumentparser
find_package(ArgsLib REQUIRED)
list(APPEND ExecutionGraph_LINK_TARGETS_CORE_DEP "argsLib")
# Try to find the RTTR Library (use our own script)
find_package(RTTRLib REQUIRED)
list(APPEND ExecutionGraph_LINK_TARGETS_CORE_DEP "rttrLib")
# Try to find the crossguid Library (use our own script)
find_package(CrossGUIDLib REQUIRED)
list(APPEND ExecutionGraph_LINK_TARGETS_CORE_DEP "crossguidLib")
# Try find cereal
find_package(CerealLib REQUIRED)
list(APPEND ExecutionGraph_LINK_TARGETS_CORE_DEP "cerealLib")

set_target_properties(ExecutionGraph::Core-Dependencies PROPERTIES INTERFACE_LINK_LIBRARIES "${ExecutionGraph_LINK_TARGETS_CORE_DEP}")

if(ExecutionGraph_USE_OPENMP)

    include(ProcessorCount)
    ProcessorCount(NPROCESSES)

    find_package(OpenMP)
    if(OpenMP_FOUND)
        set(ExecutionGraph_OPENMP_SUPPORT ON)

        mark_as_advanced( ExecutionGraph_OPENMP_USE_NTHREADS)
        set(ExecutionGraph_OPENMP_USE_NTHREADS OFF CACHE BOOL "If the number of threads should be fixed, otherwise determined at runtime!")

        if(ExecutionGraph_OPENMP_USE_NTHREADS)
            set(ExecutionGraph_OPENMP_NTHREADS ${NPROCESSES} CACHE STRING "The number of threads to use.")
            mark_as_advanced( ExecutionGraph_OPENMP_NTHREADS )
        endif()

    else()
        set(ExecutionGraph_OPENMP_SUPPORT OFF)
    endif()
endif()
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

set(ExecutionGraph_LICENSE_FILE "${CMAKE_CURRENT_SOURCE_DIR}/LICENSE")
set(ExecutionGraph_README_FILE "${CMAKE_CURRENT_SOURCE_DIR}/README.md")

if( ${ExecutionGraph_BUILD_EXAMPLE} OR ${ExecutionGraph_BUILD_TESTS} OR ${ExecutionGraph_BUILD_GUI} OR ${ExecutionGraph_BUILD_BENCHMARKS})
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
message(STATUS "                   NORMAL BUILD CONFIGURE [DONE ]                ")
message(STATUS "=================================================================")