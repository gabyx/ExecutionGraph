#The CMake Minimum version that is required.
cmake_minimum_required(VERSION 3.4)

message(STATUS "Binary dir:" ${CMAKE_BINARY_DIR})

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

set(CMAKE_VERBOSE_MAKEFILE OFF)

set(CMAKE_MODULE_PATH "${CMAKE_CURRENT_SOURCE_DIR}/cmake/" ${CMAKE_MODULE_PATH})
message(STATUS "Module path is now set to: " ${CMAKE_MODULE_PATH} )

# Load print macros
include(PrintMacros)
# Load important modules
include(FirstCMakeRun)
# Speed up the build
include(cotire) 

#The name of the project ===============================================
set(ExecutionGraphProjectName "ExecutionGraph")
# if(${CMAKE_GENERATOR} MATCHES "Unix Makefiles")
#         set(ExecutionGraphProjectName  "${ExecutionGraphProjectName}-${PROJECTNAMEPREFIX}")
# endif()
message(STATUS "Project name is: " ${ExecutionGraphProjectName})
project(${ExecutionGraphProjectName})

set(ExecutionGraph_BINARY_DIR ${PROJECT_BINARY_DIR})
set(ExecutionGraph_ROOT_DIR   ${PROJECT_SOURCE_DIR})


message(STATUS "Compiler ID is: " ${CMAKE_CXX_COMPILER_ID})
if(NOT MYPROJECT_DONTSET_COMPILER_FLAGS_INTERNAL)
	if(${CMAKE_CXX_COMPILER_ID} STREQUAL "GNU")

		set(MYPROJECT_DONTSET_COMPILER_FLAGS_INTERNAL TRUE CACHE INTERNAL "x" FORCE)
		message(STATUS "Setting Values for GNU")
		set(CMAKE_C_FLAGS "-fmessage-length=0 -fmax-errors=50" CACHE STRING "Flags for C Compiler" FORCE)
		set(CMAKE_CXX_FLAGS "-std=c++14 -fmax-errors=50 -Werror=return-type" CACHE STRING "Flags for CXX Compiler" FORCE)
		set(CMAKE_CXX_FLAGS_DEBUG "-g -fno-omit-frame-pointer -fsanitize=leak -Wall -Wpedantic -Wno-char-subscripts" CACHE STRING "Flags for CXX Compiler for debug builds" FORCE)

    set(ExecutionGraph_ASAN_OPTIONS_DBG "-fsanitize=leak")

	ELSEIF ( ${CMAKE_CXX_COMPILER_ID} STREQUAL "Clang" )

		message(STATUS "Setting Values for Clang")
		set(MYPROJECT_DONTSET_COMPILER_FLAGS_INTERNAL TRUE CACHE INTERNAL "x" FORCE)
		set(CMAKE_C_FLAGS                "" CACHE STRING "Flags for C Compiler" FORCE)
		set(CMAKE_C_FLAGS_DEBUG          "-g" CACHE STRING "Flags for C Compiler for debug builds" FORCE)
		set(CMAKE_C_FLAGS_MINSIZEREL     "-Os -DNDEBUG" CACHE STRING "Flags for C Compiler for release minsize builds" FORCE)
		set(CMAKE_C_FLAGS_RELEASE        "-O3 -DNDEBUG" CACHE STRING "Flags for C Compiler for release builds" FORCE)
		set(CMAKE_C_FLAGS_RELWITHDEBINFO "-O2 -g" CACHE STRING "Flags for C Compiler for release builds with debug info" FORCE)

		set(CMAKE_CXX_FLAGS                "-std=c++14 -ferror-limit=50 -Werror=return-type" CACHE STRING "Flags for CXX Compiler" FORCE)
		set(CMAKE_CXX_FLAGS_DEBUG          "-g3 -fno-omit-frame-pointer -fsanitize=leak -fsanitize=address -Weverything -Wpedantic -Wno-deprecated-register -Wno-documentation -Wno-old-style-cast -Wno-comment -Wno-float-equal -Wno-deprecated -Wno-c++98-compat-pedantic -Wno-undef -Wno-unused-macros" CACHE STRING "Flags for CXX Compiler for debug builds" FORCE)
		set(CMAKE_CXX_FLAGS_MINSIZEREL     "-Os -DNDEBUG" CACHE STRING "Flags for CXX Compiler for release minsize builds" FORCE)
		set(CMAKE_CXX_FLAGS_RELEASE        "-O3 -DNDEBUG" CACHE STRING "Flags for CXX Compiler for release builds" FORCE)
		set(CMAKE_CXX_FLAGS_RELWITHDEBINFO "-O2 -g3" CACHE STRING "Flags for CXX Compiler for release builds with debug info" FORCE)

    set(ExecutionGraph_ASAN_OPTIONS_DBG "-fsanitize=leak" "-fsanitize=address")

	ELSEIF ( ${CMAKE_CXX_COMPILER_ID} STREQUAL "Intel" )

        message(STATUS "Setting Values for Intel")
		set(MYPROJECT_DONTSET_COMPILER_FLAGS_INTERNAL TRUE CACHE INTERNAL "x" FORCE)
		set(CMAKE_C_FLAGS                "" CACHE STRING "Flags for C Compiler" FORCE)
		set(CMAKE_C_FLAGS_DEBUG          "-g -Wall" CACHE STRING "Flags for C Compiler for debug builds" FORCE)
		set(CMAKE_C_FLAGS_MINSIZEREL     "-Os -DNDEBUG" CACHE STRING "Flags for C Compiler for release minsize builds" FORCE)
		set(CMAKE_C_FLAGS_RELEASE        "-O3 -DNDEBUG" CACHE STRING "Flags for C Compiler for release builds" FORCE)
		set(CMAKE_C_FLAGS_RELWITHDEBINFO "-O2 -g" CACHE STRING "Flags for C Compiler for release builds with debug info" FORCE)

		set(CMAKE_CXX_FLAGS                "-std=c++14 " CACHE STRING "Flags for CXX Compiler" FORCE)
		set(CMAKE_CXX_FLAGS_DEBUG          "-g -Wall" CACHE STRING "Flags for CXX Compiler for debug builds" FORCE)
		set(CMAKE_CXX_FLAGS_MINSIZEREL     "-Os -DNDEBUG" CACHE STRING "Flags for CXX Compiler for release minsize builds" FORCE)
		set(CMAKE_CXX_FLAGS_RELEASE        "-O3 -DNDEBUG" CACHE STRING "Flags for CXX Compiler for release builds" FORCE)
        set(CMAKE_CXX_FLAGS_RELWITHDEBINFO "-O2 -g" CACHE STRING "Flags for CXX Compiler for release builds with debug info" FORCE)

    ELSEIF ( ${CMAKE_CXX_COMPILER_ID} STREQUAL "MSVC" )
        message(ERROR "MSVC is not yet supported!")
    endif()

    # use CCACHE if possible
    find_program(CCACHE_FOUND ccache)
    if(CCACHE_FOUND)
        set(CMAKE_CXX_COMPILER_LAUNCHER ccache)
    endif(CCACHE_FOUND)

endif()



#Add some multithreaded build support ==================================
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


#Optional Stuff ========================================================

mark_as_advanced( CMAKE_DEBUG_POSTFIX )
set(CMAKE_DEBUG_POSTFIX "-dbg" CACHE STRING "Debug postfix for library/executable")

mark_as_advanced( ExecutionGraph_BUILD_LIBRARY )
set(ExecutionGraph_BUILD_LIBRARY ON CACHE BOOL "Build a shared library")

mark_as_advanced( ExecutionGraph_BUILD_TESTS)
set(ExecutionGraph_BUILD_TESTS OFF CACHE BOOL "Build the tests")

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
#=======================================================================

# Dependencies =========================================================
# Define Eigen
find_package(Eigen3 REQUIRED)
set(EIGEN_INCLUDE_DIR ${EIGEN3_INCLUDE_DIR})
list(APPEND ExecutionGraph_INC_DIRS_DEP ${EIGEN_INCLUDE_DIR})

# Try to find Meta 
find_package(Meta REQUIRED)
list(APPEND ExecutionGraph_INC_DIRS_DEP ${Meta_INCLUDE_DIR})
set(ExecutionGraph_DEPENDING_TARGETS_DEP ${Meta_TARGET})

find_package(PugiXML)
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

if(ExecutionGraph_BUILD_EXAMPLE)
  set(ExecutionGraph_BUILD_LIBRARY On CACHE BOOL "Build a shared library" FORCE)
endif()

set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} ${MYPROJECT_CXX_FLAGS}")
set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} ${MYPROJECT_CXX_FLAGS}")
message(STATUS "Added '${MYPROJECT_CXX_FLAGS}' to CMAKE_CXX and CMAKE_C_FLAGS: ${CMAKE_CXX_FLAGS} and ${CMAKE_C_FLAGS}")

# Define all MVBB Source files
include(DefineExecutionGraphSources)

set(ExecutionGraph_INCLUDE_DIR "${PROJECT_SOURCE_DIR}/include/ExecutionGraph")
set(ExecutionGraph_EXTERNAL_INCLUDE_DIRS "${PROJECT_SOURCE_DIR}/external/")

#Include all relevant sources
INCLUDE_ALL_ExecutionGraph_source(ExecutionGraph_SRC
                                  ExecutionGraph_INC
                                  ExecutionGraph_INC_DIRS
                                  ExecutionGraph_DEPENDING_TARGETS
                                  ${PROJECT_SOURCE_DIR} ${ExecutionGraph_BINARY_DIR} )

list(APPEND ExecutionGraph_INC_DIRS ${ExecutionGraph_INC_DIRS_DEP})
set(ExecutionGraph_LINK_LIBS ${ExecutionGraph_LIBRARIES_DEP})
list(APPEND ExecutionGraph_DEPENDING_TARGETS ${ExecutionGraph_DEPENDING_TARGETS_DEP})

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