#The CMake Minimum version that is required.
CMAKE_MINIMUM_REQUIRED(VERSION 2.8)

MESSAGE(STATUS "Binary dir:" ${CMAKE_BINARY_DIR})

IF("${CMAKE_BUILD_TYPE}" STREQUAL "")
    MESSAGE(STATUS "Detect build type from folder")
    #Detect Build Type if Debug appears in CMAKE_BINARY_DIR path
    STRING(REGEX MATCH ".*(Debug|debug).*" DEBUG_MATCH ${CMAKE_BINARY_DIR})
    IF(DEBUG_MATCH)
        SET(CMAKE_BUILD_TYPE Debug CACHE STRING "The build type for makefile based generators")
        SET(PROJECTNAMEPREFIX Debug)
    ELSE()
        SET(CMAKE_BUILD_TYPE Release CACHE STRING "The build type for makefile based generators")
        SET(PROJECTNAMEPREFIX Release)
    ENDIF()
ELSE()
    SET(PROJECTNAMEPREFIX ${CMAKE_BUILD_TYPE})
ENDIF()


SET(CMAKE_MODULE_PATH "${CMAKE_CURRENT_SOURCE_DIR}/cmake/" "${CMAKE_SOURCE_DIR}/additional/sanitizers-cmake/cmake" ${CMAKE_MODULE_PATH})
MESSAGE(STATUS "Module path is now set to: " ${CMAKE_MODULE_PATH} )

# Load important modules
INCLUDE(FirstCMakeRun)
# Speed up the build
INCLUDE(cotire) 

#The name of the project ===============================================
SET(ExecutionGraphProjectName "ExecutionGraph")
IF(${CMAKE_GENERATOR} MATCHES "Unix Makefiles")
        SET(ExecutionGraphProjectName  "${ExecutionGraphProjectName}-${PROJECTNAMEPREFIX}")
ENDIF()
MESSAGE(STATUS "Project name is: " ${ExecutionGraphProjectName})
PROJECT(${ExecutionGraphProjectName})

SET(ExecutionGraph_BINARY_DIR ${PROJECT_BINARY_DIR})
SET(ExecutionGraph_ROOT_DIR   ${PROJECT_SOURCE_DIR})


MESSAGE(STATUS "Compiler ID is: " ${CMAKE_CXX_COMPILER_ID})
IF(NOT MYPROJECT_DONTSET_COMPILER_FLAGS_INTERNAL)
	IF(${CMAKE_CXX_COMPILER_ID} STREQUAL "GNU")

		SET(MYPROJECT_DONTSET_COMPILER_FLAGS_INTERNAL TRUE CACHE INTERNAL "x" FORCE)
		MESSAGE(STATUS "Setting Values for GNU")
		SET(CMAKE_C_FLAGS "-fmessage-length=0 -fmax-errors=50" CACHE STRING "Flags for C Compiler" FORCE)
		SET(CMAKE_CXX_FLAGS "-std=c++14 -fmax-errors=50 -Werror=return-type" CACHE STRING "Flags for CXX Compiler" FORCE)
		SET(CMAKE_CXX_FLAGS_DEBUG "-g -fno-omit-frame-pointer -fsanitize=leak -Wall -Wpedantic -Wno-char-subscripts" CACHE STRING "Flags for CXX Compiler for debug builds" FORCE)

    SET(ExecutionGraph_ASAN_OPTIONS_DBG "-fsanitize=leak")

	ELSEIF ( ${CMAKE_CXX_COMPILER_ID} STREQUAL "Clang" )

		MESSAGE(STATUS "Setting Values for Clang")
		SET(MYPROJECT_DONTSET_COMPILER_FLAGS_INTERNAL TRUE CACHE INTERNAL "x" FORCE)
		SET(CMAKE_C_FLAGS                "" CACHE STRING "Flags for C Compiler" FORCE)
		SET(CMAKE_C_FLAGS_DEBUG          "-g" CACHE STRING "Flags for C Compiler for debug builds" FORCE)
		SET(CMAKE_C_FLAGS_MINSIZEREL     "-Os -DNDEBUG" CACHE STRING "Flags for C Compiler for release minsize builds" FORCE)
		SET(CMAKE_C_FLAGS_RELEASE        "-O3 -DNDEBUG" CACHE STRING "Flags for C Compiler for release builds" FORCE)
		SET(CMAKE_C_FLAGS_RELWITHDEBINFO "-O2 -g" CACHE STRING "Flags for C Compiler for release builds with debug info" FORCE)

		SET(CMAKE_CXX_FLAGS                "-std=c++14 -ferror-limit=50 -Werror=return-type" CACHE STRING "Flags for CXX Compiler" FORCE)
		SET(CMAKE_CXX_FLAGS_DEBUG          "-g3 -fno-omit-frame-pointer -fsanitize=leak -fsanitize=address -Weverything -Wpedantic -Wno-deprecated-register -Wno-documentation -Wno-old-style-cast -Wno-comment -Wno-float-equal -Wno-deprecated -Wno-c++98-compat-pedantic -Wno-undef -Wno-unused-macros" CACHE STRING "Flags for CXX Compiler for debug builds" FORCE)
		SET(CMAKE_CXX_FLAGS_MINSIZEREL     "-Os -DNDEBUG" CACHE STRING "Flags for CXX Compiler for release minsize builds" FORCE)
		SET(CMAKE_CXX_FLAGS_RELEASE        "-O3 -DNDEBUG" CACHE STRING "Flags for CXX Compiler for release builds" FORCE)
		SET(CMAKE_CXX_FLAGS_RELWITHDEBINFO "-O2 -g3" CACHE STRING "Flags for CXX Compiler for release builds with debug info" FORCE)

    SET(ExecutionGraph_ASAN_OPTIONS_DBG "-fsanitize=leak" "-fsanitize=address")

	ELSEIF ( ${CMAKE_CXX_COMPILER_ID} STREQUAL "Intel" )

        MESSAGE(STATUS "Setting Values for Intel")
		SET(MYPROJECT_DONTSET_COMPILER_FLAGS_INTERNAL TRUE CACHE INTERNAL "x" FORCE)
		SET(CMAKE_C_FLAGS                "" CACHE STRING "Flags for C Compiler" FORCE)
		SET(CMAKE_C_FLAGS_DEBUG          "-g -Wall" CACHE STRING "Flags for C Compiler for debug builds" FORCE)
		SET(CMAKE_C_FLAGS_MINSIZEREL     "-Os -DNDEBUG" CACHE STRING "Flags for C Compiler for release minsize builds" FORCE)
		SET(CMAKE_C_FLAGS_RELEASE        "-O3 -DNDEBUG" CACHE STRING "Flags for C Compiler for release builds" FORCE)
		SET(CMAKE_C_FLAGS_RELWITHDEBINFO "-O2 -g" CACHE STRING "Flags for C Compiler for release builds with debug info" FORCE)

		SET(CMAKE_CXX_FLAGS                "-std=c++14 " CACHE STRING "Flags for CXX Compiler" FORCE)
		SET(CMAKE_CXX_FLAGS_DEBUG          "-g -Wall" CACHE STRING "Flags for CXX Compiler for debug builds" FORCE)
		SET(CMAKE_CXX_FLAGS_MINSIZEREL     "-Os -DNDEBUG" CACHE STRING "Flags for CXX Compiler for release minsize builds" FORCE)
		SET(CMAKE_CXX_FLAGS_RELEASE        "-O3 -DNDEBUG" CACHE STRING "Flags for CXX Compiler for release builds" FORCE)
		SET(CMAKE_CXX_FLAGS_RELWITHDEBINFO "-O2 -g" CACHE STRING "Flags for CXX Compiler for release builds with debug info" FORCE)

  ELSEIF ( ${CMAKE_CXX_COMPILER_ID} STREQUAL "MSVC" )
    MESSAGE(ERROR "MSVC is not yet supported!")
  ENDIF()

ENDIF()


#Add some multithreaded build support ==================================
MARK_AS_ADVANCED(MULTITHREADED_BUILD)
SET(MULTITHREADED_BUILD ON CACHE BOOL "Parallel build with as many threads as possible!")
IF(MULTITHREADED_BUILD)
	IF(${CMAKE_GENERATOR} MATCHES "Unix Makefiles")
            FILE(COPY ${ExecutionGraph_ROOT_DIR}/cmake/parallelmake.sh DESTINATION ${PROJECT_BINARY_DIR}
                FILE_PERMISSIONS OWNER_READ OWNER_WRITE OWNER_EXECUTE GROUP_READ GROUP_EXECUTE WORLD_READ WORLD_EXECUTE
                NO_SOURCE_PERMISSIONS
            )
            SET(CMAKE_MAKE_PROGRAM "${PROJECT_BINARY_DIR}/parallelmake.sh")
            MESSAGE(STATUS "Set make program to ${PROJECT_BINARY_DIR}/parallelmake.sh")
    ELSEIF(MSVC)
      SET(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS}" "/MP")
      MESSAGE(STATUS "Added parallel build arguments to CMAKE_CXX_FLAGS: ${CMAKE_CXX_FLAGS}")
    ENDIF()
ENDIF()
# ======================================================================


#Optional Stuff ========================================================

MARK_AS_ADVANCED( CMAKE_DEBUG_POSTFIX )
SET(CMAKE_DEBUG_POSTFIX "-dbg" CACHE STRING "Debug postfix for library/executable")

MARK_AS_ADVANCED( ExecutionGraph_BUILD_LIBRARY )
SET(ExecutionGraph_BUILD_LIBRARY ON CACHE BOOL "Build a shared library")

MARK_AS_ADVANCED( ExecutionGraph_BUILD_TESTS)
SET(ExecutionGraph_BUILD_TESTS ON CACHE BOOL "Build the tests")

MARK_AS_ADVANCED( ExecutionGraph_BUILD_BENCHMARKS)
SET(ExecutionGraph_BUILD_BENCHMARKS ON CACHE BOOL "Build the benchmarks")

MARK_AS_ADVANCED( ExecutionGraph_BUILD_EXAMPLE )
SET(ExecutionGraph_BUILD_EXAMPLE ON CACHE BOOL "Build the example, the library is automatically built if this is true")

MARK_AS_ADVANCED( ExecutionGraph_FORCE_MSGLOG_LEVEL)
SET(ExecutionGraph_FORCE_MSGLOG_LEVEL "DEBUG" CACHE STRING "Force the message log level (TRACE, DEBUG, INFO, WARN, ERROR, FATAL)")

MARK_AS_ADVANCED( ExecutionGraph_THROW_IF_BAD_SOCKET_CASTS)
SET(ExecutionGraph_THROW_IF_BAD_SOCKET_CASTS ON CACHE BOOL "Force an exception if a bad socket cast happens! (if false, the exception is thrown only in Debug mode!)")

#~ MARK_AS_ADVANCED( ExecutionGraph_USE_OPENMP)
#~ SET(ExecutionGraph_USE_OPENMP ON CACHE BOOL "Try to use OpenMp for parallel speedup")
#=======================================================================

# Dependencies =========================================================
# Define Eigen
FIND_PACKAGE(Eigen3 REQUIRED)
SET(EIGEN_INCLUDE_DIR ${EIGEN3_INCLUDE_DIR})
LIST(APPEND ExecutionGraph_INC_DIRS_DEP ${EIGEN_INCLUDE_DIR})

# Try to find Meta 
FIND_PACKAGE(Meta REQUIRED)
LIST(APPEND ExecutionGraph_INC_DIRS_DEP ${Meta_INCLUDE_DIR})
SET(ExecutionGraph_DEPENDING_TARGETS_DEP ${Meta_TARGET})

FIND_PACKAGE(PugiXML)
IF(PUGIXML_FOUND)
    LIST(APPEND ExecutionGraph_INC_DIRS_DEP ${PUGIXML_INCLUDE_DIR} )
    LIST(APPEND ExecutionGraph_LIBRARIES_DEP ${PUGIXML_LIBRARIES})
    SET(ExecutionGraph_SUPPORT_XML ON)
ENDIF()

#~ IF(ExecutionGraph_USE_OPENMP)

  #~ include(ProcessorCount)
  #~ ProcessorCount(NPROCESSES)

  #~ FIND_PACKAGE(OpenMP)
  #~ IF(OpenMP_FOUND)
    #~ SET(MYPROJECT_CXX_FLAGS "${MYPROJECT_CXX_FLAGS} ${OpenMP_CXX_FLAGS}")
    #~ SET(ExecutionGraph_OPENMP_SUPPORT ON)

    #~ MARK_AS_ADVANCED( ExecutionGraph_OPENMP_USE_NTHREADS)
    #~ SET(ExecutionGraph_OPENMP_USE_NTHREADS OFF CACHE BOOL "If the number of threads should be fixed, otherwise determined at runtime!")

    #~ IF(ExecutionGraph_OPENMP_USE_NTHREADS)
        #~ SET(ExecutionGraph_OPENMP_NTHREADS ${NPROCESSES} CACHE STRING "The number of threads to use.")
        #~ MARK_AS_ADVANCED( ExecutionGraph_OPENMP_NTHREADS )
    #~ ENDIF()

  #~ ELSE()
    #~ SET(ExecutionGraph_OPENMP_SUPPORT ON)
  #~ ENDIF()
#~ ENDIF()
# ======================================================================

IF(ExecutionGraph_BUILD_EXAMPLE)
  SET(ExecutionGraph_BUILD_LIBRARY On CACHE BOOL "Build a shared library" FORCE)
ENDIF()

SET(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} ${MYPROJECT_CXX_FLAGS}")
SET(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} ${MYPROJECT_CXX_FLAGS}")
MESSAGE(STATUS "Added '${MYPROJECT_CXX_FLAGS}' to CMAKE_CXX and CMAKE_C_FLAGS: ${CMAKE_CXX_FLAGS} and ${CMAKE_C_FLAGS}")

# Define all MVBB Source files
INCLUDE(DefineExecutionGraphSources)

SET(ExecutionGraph_INCLUDE_DIR "${PROJECT_SOURCE_DIR}/include/ExecutionGraph")
SET(ExecutionGraph_EXTERNAL_INCLUDE_DIRS "${PROJECT_SOURCE_DIR}/external/")

#Include all relevant sources
INCLUDE_ALL_ExecutionGraph_SOURCE(ExecutionGraph_SRC
                                  ExecutionGraph_INC
                                  ExecutionGraph_INC_DIRS
                                  ExecutionGraph_DEPENDING_TARGETS
                                  ${PROJECT_SOURCE_DIR} ${ExecutionGraph_BINARY_DIR} )

LIST(APPEND ExecutionGraph_INC_DIRS ${ExecutionGraph_INC_DIRS_DEP})
SET(ExecutionGraph_LINK_LIBS ${ExecutionGraph_LIBRARIES_DEP})
#LIST(APPEND ${ExecutionGraph_DEPENDING_TARGETS} ${ExecutionGraph_DEPENDING_TARGETS_DEP})

IF(ExecutionGraph_BUILD_LIBRARY)
    ADD_SUBDIRECTORY(lib)
ENDIF()

IF(ExecutionGraph_BUILD_TESTS)
    enable_testing()
    add_custom_target(build_and_test ${CMAKE_CTEST_COMMAND} -V)
    ADD_SUBDIRECTORY(tests)
ENDIF()

#IF(ExecutionGraph_BUILD_BENCHMARKS)
    #enable_testing()
    #add_custom_target(build_and_benchmark ${CMAKE_CTEST_COMMAND} -V)
    #ADD_SUBDIRECTORY(benchmarks)
#ENDIF()


IF(ExecutionGraph_BUILD_EXAMPLE)
    ADD_SUBDIRECTORY(examples)
ENDIF()

ADD_SUBDIRECTORY(gui)
