
# Options ==============================================================
MARK_AS_ADVANCED( ExecutionGraph_BUILD_LIBRARY_STATIC  )
SET(ExecutionGraph_BUILD_LIBRARY_STATIC OFF CACHE BOOL "Build a static library")

MARK_AS_ADVANCED(ExecutionGraph_LIB_INSTALL_DIR ExecutionGraph_INCLUDE_INSTALL_DIR )
# ======================================================================

SET(SOURCE_FILES
    ${ExecutionGraph_SRC}
)

SET(INCLUDE_FILES
    ${ExecutionGraph_INC}
)

#include directories
SET(INCLUDE_DIRS
    ${EIGEN_INCLUDE_DIR}
    ${ExecutionGraph_INC_DIRS}
)

SET(LINK_LIBS ${ExecutionGraph_LINK_LIBS})

# Make targets library shared/static ============================================
IF(ExecutionGraph_BUILD_LIBRARY_STATIC)
ADD_LIBRARY(ExecutionGraph STATIC ${SOURCE_FILES} ${INCLUDE_FILES})
SET_TARGET_PROPERTIES(ExecutionGraph PROPERTIES OUTPUT_NAME ExecutionGraphStatic)
TARGET_INCLUDE_DIRECTORIES(${INCLUDE_DIRS})
ELSE()
ADD_LIBRARY(ExecutionGraph SHARED ${SOURCE_FILES} ${INCLUDE_FILES})
ENDIF()

IF( NOT "${ExecutionGraph_DEPENDING_TARGETS}" STREQUAL "")
    ADD_DEPENDENCIES(ExecutionGraph ${ExecutionGraph_DEPENDING_TARGETS})
ENDIF()

TARGET_LINK_LIBRARIES(ExecutionGraph ${LINK_LIBS})
SET_TARGET_PROPERTIES(ExecutionGraph PROPERTIES
    VERSION ${ExecutionGraph_VERSION_STRING} SOVERSION ${ExecutionGraph_VERSION_STRING}
)

# set build flag, for import/export specifiers in platform.hpp
# Building a library means we only export the symbols which have ExecutionGraph_EXPORT set in the Platform.hpp file
# We define all other symbols to be hidden by default as suggested by https://gcc.gnu.org/wiki/Visibility
IF(${CMAKE_CXX_COMPILER_ID} STREQUAL "GNU" OR ${CMAKE_CXX_COMPILER_ID} STREQUAL "Clang" OR ${CMAKE_CXX_COMPILER_ID} STREQUAL "Intel")
    target_compile_options(ExecutionGraph PRIVATE -DExecutionGraph_BUILD_LIBRARY -fvisibility=hidden)
ELSE()
    target_compile_options(ExecutionGraph PRIVATE /DExecutionGraph_BUILD_LIBRARY)
ENDIF()
# ======================================================================

# Make target library for tests ================================================
IF(ExecutionGraph_BUILD_TESTS)
  IF(ExecutionGraph_BUILD_LIBRARY_STATIC)
  ADD_LIBRARY(ExecutionGraphForTestsOnly STATIC ${SOURCE_FILES} ${INCLUDE_FILES})
  SET_TARGET_PROPERTIES(ExecutionGraphForTestsOnly PROPERTIES OUTPUT_NAME ExecutionGraphForTestsOnlyStatic)
  ELSE()
  ADD_LIBRARY(ExecutionGraphForTestsOnly SHARED ${SOURCE_FILES} ${INCLUDE_FILES})
  ENDIF()

  IF( NOT "${ExecutionGraph_DEPENDING_TARGETS}" STREQUAL "")
      ADD_DEPENDENCIES(ExecutionGraphForTestsOnly ${ExecutionGraph_DEPENDING_TARGETS})
  ENDIF()

  TARGET_LINK_LIBRARIES(ExecutionGraphForTestsOnly ${LINK_LIBS})
  SET_TARGET_PROPERTIES(ExecutionGraphForTestsOnly PROPERTIES
      VERSION ${ExecutionGraph_VERSION_STRING} SOVERSION ${ExecutionGraph_VERSION_STRING}
  )

  # set build flag, for import/export specifiers in platform.hpp
  # Building a library means we only export the symbols which have ExecutionGraph_EXPORT set in the Platform.hpp file
  # We define all other symbols to be hidden by default as suggested by https://gcc.gnu.org/wiki/Visibility
  IF(${CMAKE_CXX_COMPILER_ID} STREQUAL "GNU" OR ${CMAKE_CXX_COMPILER_ID} STREQUAL "Clang" OR ${CMAKE_CXX_COMPILER_ID} STREQUAL "Intel")
      target_compile_options(ExecutionGraphForTestsOnly PRIVATE -DExecutionGraph_BUILD_LIBRARY -DExecutionGraph_BUILD_TESTS -fvisibility=hidden)
  ELSE()
      target_compile_options(ExecutionGraphForTestsOnly PRIVATE /DExecutionGraph_BUILD_LIBRARY /DExecutionGraph_BUILD_TESTS)
  ENDIF()
ENDIF()
# ======================================================================

SET(ExecutionGraph_LIBRARY_NAME_REL "ExecutionGraph")                       # used for cmake config
SET(ExecutionGraph_LIBRARY_NAME_DBG "ExecutionGraph${CMAKE_DEBUG_POSTFIX}") # used for cmake config
SET(ExecutionGraph_INCLUDE_CHARATERISTIC_HEADER ${ExecutionGraph/config/Config.hpp})

# Install stuff (default location is not some where on the system! for safty reasons
IF(FIRST_CMAKE_RUN)
    SET(CMAKE_INSTALL_PREFIX "${PROJECT_BINARY_DIR}/../install" CACHE STRING "Install prefix (e.g. /usr/local/)" FORCE)
ENDIF()

# Set install directories (if changed CMAKE_INSTALL_PREFIX adjust values)
SET(ExecutionGraph_LIB_INSTALL_DIR ${CMAKE_INSTALL_PREFIX}/lib CACHE STRING "Install dir library")
SET(ExecutionGraph_INC_INSTALL_DIR ${CMAKE_INSTALL_PREFIX}/include/ExecutionGraph CACHE STRING "Install dir includes")


IF(NOT( CMAKE_INSTALL_PREFIX STREQUAL INSTALL_PREFIX_LAST_VALUE ) )
    SET(INSTALL_PREFIX_LAST_VALUE ${CMAKE_INSTALL_PREFIX} CACHE INTERNAL "Last install prefix value")
    MESSAGE(WARNING "Check install dir! They have been changed!")
    UNSET(ExecutionGraph_LIB_INSTALL_DIR)
    UNSET(ExecutionGraph_INCLUDE_INSTALL_DIR)
    SET(ExecutionGraph_LIB_INSTALL_DIR ${CMAKE_INSTALL_PREFIX}/lib CACHE STRING "Install dir library" FORCE)
    SET(ExecutionGraph_INC_INSTALL_DIR ${CMAKE_INSTALL_PREFIX}/include CACHE STRING "Install dir includes" FORCE)
    MARK_AS_ADVANCED(FORCE ExecutionGraph_LIB_INSTALL_DIR ExecutionGraph_INC_INSTALL_DIR )
ENDIF()

# INSTALLING FILES ================================================================================================================================
MESSAGE( STATUS "Installing library to " "${ExecutionGraph_LIB_INSTALL_DIR}")
INSTALL(TARGETS ExecutionGraph
        LIBRARY DESTINATION ${ExecutionGraph_LIB_INSTALL_DIR}
        ARCHIVE DESTINATION ${ExecutionGraph_LIB_INSTALL_DIR})

MESSAGE( STATUS "Installing include folders:" ${ExecutionGraph_INCLUDE_DIR} " to ${ExecutionGraph_INC_INSTALL_DIR}")

INSTALL(DIRECTORY  ${ExecutionGraph_INCLUDE_DIR}  DESTINATION ${ExecutionGraph_INC_INSTALL_DIR}
        FILE_PERMISSIONS      OWNER_READ OWNER_WRITE               GROUP_READ               WORLD_READ
        DIRECTORY_PERMISSIONS OWNER_READ OWNER_WRITE OWNER_EXECUTE GROUP_READ GROUP_EXECUTE WORLD_READ WORLD_EXECUTE
        FILES_MATCHING PATTERN "*.hpp" PATTERN "*.h" PATTERN "*.cmake" EXCLUDE)


MESSAGE( STATUS "Installing external include folders:" ${ExecutionGraph_EXTERNAL_INCLUDE_DIRS} " to ${ExecutionGraph_INC_INSTALL_DIR}/ExecutionGraph")
INSTALL(DIRECTORY  ${ExecutionGraph_EXTERNAL_INCLUDE_DIRS}
        DESTINATION "${ExecutionGraph_INC_INSTALL_DIR}/ExecutionGraph"
        FILE_PERMISSIONS      OWNER_READ OWNER_WRITE               GROUP_READ               WORLD_READ
        DIRECTORY_PERMISSIONS OWNER_READ OWNER_WRITE OWNER_EXECUTE GROUP_READ GROUP_EXECUTE WORLD_READ WORLD_EXECUTE
        FILES_MATCHING PATTERN "*.hpp" PATTERN "*.h" PATTERN "*.cmake" EXCLUDE)

MESSAGE( STATUS "Installing configuration dependend files:" ${ExecutionGraph_BINARY_DIR}/include/ExecutionGraph " to ${ExecutionGraph_INC_INSTALL_DIR}")
INSTALL(DIRECTORY  ${ExecutionGraph_BINARY_DIR}/include/ExecutionGraph
        DESTINATION ${ExecutionGraph_INC_INSTALL_DIR}
        FILE_PERMISSIONS      OWNER_READ OWNER_WRITE               GROUP_READ               WORLD_READ
        DIRECTORY_PERMISSIONS OWNER_READ OWNER_WRITE OWNER_EXECUTE GROUP_READ GROUP_EXECUTE WORLD_READ WORLD_EXECUTE)



# Make configuration files for cmake ============================================================================

SET(CMAKE_CONFIG_FOLDER ${ExecutionGraph_LIB_INSTALL_DIR}/cmake/ExecutionGraph-${ExecutionGraph_VERSION_STRING})

#=========================
# Generate ExecutionGraph-config.cmake file which needs to be installed afterwards
SET(ExecutionGraph_LIBRARY_NAME $<TARGET_FILE_NAME:ExecutionGraph>)
SET(ExecutionGraph_CMAKE_CONFIG_FILE "${PROJECT_BINARY_DIR}/executiongraph-config.cmake")
CONFIGURE_FILE(
  ${CMAKE_CURRENT_SOURCE_DIR}/executiongraph-config.cmake.in
  ${ExecutionGraph_CMAKE_CONFIG_FILE} @ONLY
)
#=========================
# Generate ExecutionGraph-config-version.cmake file which needs to be installed afterwards
SET(ExecutionGraph_LIBRARY_NAME $<TARGET_FILE_NAME:ExecutionGraph>)
SET(ExecutionGraph_CMAKE_CONFIG_VERSION_FILE "${PROJECT_BINARY_DIR}/executiongraph-config-version.cmake")
CONFIGURE_FILE(
  ${CMAKE_CURRENT_SOURCE_DIR}/executiongraph-config-version.cmake.in
  ${ExecutionGraph_CMAKE_CONFIG_VERSION_FILE} @ONLY
)
#=========================


MESSAGE( STATUS "Installing cmake config files to ${CMAKE_CONFIG_FOLDER}")
INSTALL(FILES  ${ExecutionGraph_CMAKE_CONFIG_FILE} ${ExecutionGraph_CMAKE_CONFIG_VERSION_FILE}
        DESTINATION ${CMAKE_CONFIG_FOLDER}
        PERMISSIONS OWNER_READ OWNER_WRITE GROUP_READ WORLD_READ)

MESSAGE( STATUS "Installing dependency cmake find scripts for 'eigen3,meta,pugixml' to ${CMAKE_CONFIG_FOLDER}")
INSTALL(FILES  ${ExecutionGraph_ROOT_DIR}/cmake/FindEigen3.cmake
               ${ExecutionGraph_ROOT_DIR}/cmake/FindMeta.cmake
               ${ExecutionGraph_ROOT_DIR}/cmake/FindPugiXML.cmake
        DESTINATION ${CMAKE_CONFIG_FOLDER}
        PERMISSIONS OWNER_READ OWNER_WRITE GROUP_READ WORLD_READ)

# ==================================================================================================================
