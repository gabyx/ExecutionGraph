
# Options ==============================================================
mark_as_advanced( ExecutionGraph_BUILD_LIBRARY_STATIC  )
set(ExecutionGraph_BUILD_LIBRARY_STATIC OFF CACHE BOOL "Build a static library")

mark_as_advanced(ExecutionGraph_LIB_INSTALL_DIR ExecutionGraph_INCLUDE_INSTALL_DIR )
# ======================================================================

set(SOURCE_FILES
    ${ExecutionGraph_SRC}
)

set(INCLUDE_FILES
    ${ExecutionGraph_INC}
)

#include directories
set(INCLUDE_DIRS
    ${EIGEN_INCLUDE_DIR}
    ${ExecutionGraph_INC_DIRS}
)

set(LINK_LIBS ${ExecutionGraph_LINK_LIBS})

# Make targets library shared/static ============================================
if(ExecutionGraph_BUILD_LIBRARY_STATIC)
add_library(ExecutionGraph STATIC ${SOURCE_FILES} ${INCLUDE_FILES})
set_target_properties(ExecutionGraph PROPERTIES OUTPUT_NAME ExecutionGraphStatic)
target_include_directories(${INCLUDE_DIRS})
else()
add_library(ExecutionGraph SHARED ${SOURCE_FILES} ${INCLUDE_FILES})
endif()

if( NOT "${ExecutionGraph_DEPENDING_TARGETS}" STREQUAL "")
    add_dependencies(ExecutionGraph ${ExecutionGraph_DEPENDING_TARGETS})
endif()

target_link_libraries(ExecutionGraph ${LINK_LIBS})
set_target_properties(ExecutionGraph PROPERTIES
    VERSION ${ExecutionGraph_VERSION_STRING} SOVERSION ${ExecutionGraph_VERSION_STRING}
)

# set build flag, for import/export specifiers in platform.hpp
# Building a library means we only export the symbols which have ExecutionGraph_EXPORT set in the Platform.hpp file
# We define all other symbols to be hidden by default as suggested by https://gcc.gnu.org/wiki/Visibility
if(${CMAKE_CXX_COMPILER_ID} STREQUAL "GNU" OR ${CMAKE_CXX_COMPILER_ID} STREQUAL "Clang" OR ${CMAKE_CXX_COMPILER_ID} STREQUAL "Intel")
    target_compile_options(ExecutionGraph PRIVATE -DExecutionGraph_BUILD_LIBRARY -fvisibility=hidden)
else()
    target_compile_options(ExecutionGraph PRIVATE /DExecutionGraph_BUILD_LIBRARY)
endif()
# ======================================================================

# Make target library for tests ================================================
if(ExecutionGraph_BUILD_TESTS)
  if(ExecutionGraph_BUILD_LIBRARY_STATIC)
  add_library(ExecutionGraphForTestsOnly STATIC ${SOURCE_FILES} ${INCLUDE_FILES})
  set_target_properties(ExecutionGraphForTestsOnly PROPERTIES OUTPUT_NAME ExecutionGraphForTestsOnlyStatic)
  else()
  add_library(ExecutionGraphForTestsOnly SHARED ${SOURCE_FILES} ${INCLUDE_FILES})
  endif()

  if( NOT "${ExecutionGraph_DEPENDING_TARGETS}" STREQUAL "")
      add_dependencies(ExecutionGraphForTestsOnly ${ExecutionGraph_DEPENDING_TARGETS})
  endif()

  target_link_libraries(ExecutionGraphForTestsOnly ${LINK_LIBS})
  set_target_properties(ExecutionGraphForTestsOnly PROPERTIES
      VERSION ${ExecutionGraph_VERSION_STRING} SOVERSION ${ExecutionGraph_VERSION_STRING}
  )

  # set build flag, for import/export specifiers in platform.hpp
  # Building a library means we only export the symbols which have ExecutionGraph_EXPORT set in the Platform.hpp file
  # We define all other symbols to be hidden by default as suggested by https://gcc.gnu.org/wiki/Visibility
  if(${CMAKE_CXX_COMPILER_ID} STREQUAL "GNU" OR ${CMAKE_CXX_COMPILER_ID} STREQUAL "Clang" OR ${CMAKE_CXX_COMPILER_ID} STREQUAL "Intel")
      target_compile_options(ExecutionGraphForTestsOnly PRIVATE -DExecutionGraph_BUILD_LIBRARY -DExecutionGraph_BUILD_TESTS -fvisibility=hidden)
  else()
      target_compile_options(ExecutionGraphForTestsOnly PRIVATE /DExecutionGraph_BUILD_LIBRARY /DExecutionGraph_BUILD_TESTS)
  endif()
endif()
# ======================================================================

set(ExecutionGraph_LIBRARY_NAME_REL "ExecutionGraph")                       # used for cmake config
set(ExecutionGraph_LIBRARY_NAME_DBG "ExecutionGraph${CMAKE_DEBUG_POSTFIX}") # used for cmake config
set(ExecutionGraph_INCLUDE_CHARATERISTIC_HEADER ${ExecutionGraph/config/Config.hpp})

# Install stuff (default location is not some where on the system! for safty reasons
if(FIRST_CMAKE_RUN)
    set(CMAKE_INSTALL_PREFIX "${PROJECT_BINARY_DIR}/../install" CACHE STRING "Install prefix (e.g. /usr/local/)" FORCE)
endif()

# Set install directories (if changed CMAKE_INSTALL_PREFIX adjust values)
set(ExecutionGraph_LIB_INSTALL_DIR ${CMAKE_INSTALL_PREFIX}/lib CACHE STRING "Install dir library")
set(ExecutionGraph_INC_INSTALL_DIR ${CMAKE_INSTALL_PREFIX}/include/ExecutionGraph CACHE STRING "Install dir includes")


if(not( CMAKE_INSTALL_PREFIX STREQUAL INSTALL_PREFIX_LAST_VALUE ) )
    set(INSTALL_PREFIX_LAST_VALUE ${CMAKE_INSTALL_PREFIX} CACHE INTERNAL "Last install prefix value")
    message(WARNING "Check install dir! They have been changed!")
    unset(ExecutionGraph_LIB_INSTALL_DIR)
    unset(ExecutionGraph_INCLUDE_INSTALL_DIR)
    set(ExecutionGraph_LIB_INSTALL_DIR ${CMAKE_INSTALL_PREFIX}/lib CACHE STRING "Install dir library" FORCE)
    set(ExecutionGraph_INC_INSTALL_DIR ${CMAKE_INSTALL_PREFIX}/include CACHE STRING "Install dir includes" FORCE)
    mark_as_advanced(FORCE ExecutionGraph_LIB_INSTALL_DIR ExecutionGraph_INC_INSTALL_DIR )
endif()

# INSTALLING FILES ================================================================================================================================
message( STATUS "Installing library to " "${ExecutionGraph_LIB_INSTALL_DIR}")
install(TARGETS ExecutionGraph
        LIBRARY DESTINATION ${ExecutionGraph_LIB_INSTALL_DIR}
        ARCHIVE DESTINATION ${ExecutionGraph_LIB_INSTALL_DIR})

message( STATUS "Installing include folders:" ${ExecutionGraph_INCLUDE_DIR} " to ${ExecutionGraph_INC_INSTALL_DIR}")

install(DIRECTORY  ${ExecutionGraph_INCLUDE_DIR}  DESTINATION ${ExecutionGraph_INC_INSTALL_DIR}
        FILE_PERMISSIONS      OWNER_READ OWNER_WRITE               GROUP_READ               WORLD_READ
        DIRECTORY_PERMISSIONS OWNER_READ OWNER_WRITE OWNER_EXECUTE GROUP_READ GROUP_EXECUTE WORLD_READ WORLD_EXECUTE
        FILES_MATCHING PATTERN "*.hpp" PATTERN "*.h" PATTERN "*.cmake" EXCLUDE)


message( STATUS "Installing external include folders:" ${ExecutionGraph_EXTERNAL_INCLUDE_DIRS} " to ${ExecutionGraph_INC_INSTALL_DIR}/ExecutionGraph")
install(DIRECTORY  ${ExecutionGraph_EXTERNAL_INCLUDE_DIRS}
        DESTINATION "${ExecutionGraph_INC_INSTALL_DIR}/ExecutionGraph"
        FILE_PERMISSIONS      OWNER_READ OWNER_WRITE               GROUP_READ               WORLD_READ
        DIRECTORY_PERMISSIONS OWNER_READ OWNER_WRITE OWNER_EXECUTE GROUP_READ GROUP_EXECUTE WORLD_READ WORLD_EXECUTE
        FILES_MATCHING PATTERN "*.hpp" PATTERN "*.h" PATTERN "*.cmake" EXCLUDE)

message( STATUS "Installing configuration dependend files:" ${ExecutionGraph_BINARY_DIR}/include/ExecutionGraph " to ${ExecutionGraph_INC_INSTALL_DIR}")
install(DIRECTORY  ${ExecutionGraph_BINARY_DIR}/include/ExecutionGraph
        DESTINATION ${ExecutionGraph_INC_INSTALL_DIR}
        FILE_PERMISSIONS      OWNER_READ OWNER_WRITE               GROUP_READ               WORLD_READ
        DIRECTORY_PERMISSIONS OWNER_READ OWNER_WRITE OWNER_EXECUTE GROUP_READ GROUP_EXECUTE WORLD_READ WORLD_EXECUTE)



# Make configuration files for cmake ============================================================================

set(CMAKE_CONFIG_FOLDER ${ExecutionGraph_LIB_INSTALL_DIR}/cmake/ExecutionGraph-${ExecutionGraph_VERSION_STRING})

#=========================
# Generate ExecutionGraph-config.cmake file which needs to be installed afterwards
set(ExecutionGraph_LIBRARY_NAME $<TARGET_FILE_NAME:ExecutionGraph>)
set(ExecutionGraph_CMAKE_CONFIG_FILE "${PROJECT_BINARY_DIR}/executiongraph-config.cmake")
configure_file(
  ${CMAKE_CURRENT_SOURCE_DIR}/executiongraph-config.cmake.in
  ${ExecutionGraph_CMAKE_CONFIG_FILE} @ONLY
)
#=========================
# Generate ExecutionGraph-config-version.cmake file which needs to be installed afterwards
set(ExecutionGraph_LIBRARY_NAME $<TARGET_FILE_NAME:ExecutionGraph>)
set(ExecutionGraph_CMAKE_CONFIG_VERSION_FILE "${PROJECT_BINARY_DIR}/executiongraph-config-version.cmake")
configure_file(
  ${CMAKE_CURRENT_SOURCE_DIR}/executiongraph-config-version.cmake.in
  ${ExecutionGraph_CMAKE_CONFIG_VERSION_FILE} @ONLY
)
#=========================


message( STATUS "Installing cmake config files to ${CMAKE_CONFIG_FOLDER}")
install(FILES  ${ExecutionGraph_CMAKE_CONFIG_FILE} ${ExecutionGraph_CMAKE_CONFIG_VERSION_FILE}
        DESTINATION ${CMAKE_CONFIG_FOLDER}
        PERMISSIONS OWNER_READ OWNER_WRITE GROUP_READ WORLD_READ)

message( STATUS "Installing dependency cmake find scripts for 'eigen3,meta,pugixml' to ${CMAKE_CONFIG_FOLDER}")
install(FILES  ${ExecutionGraph_ROOT_DIR}/cmake/FindEigen3.cmake
               ${ExecutionGraph_ROOT_DIR}/cmake/FindMeta.cmake
               ${ExecutionGraph_ROOT_DIR}/cmake/FindPugiXML.cmake
        DESTINATION ${CMAKE_CONFIG_FOLDER}
        PERMISSIONS OWNER_READ OWNER_WRITE GROUP_READ WORLD_READ)

# ==================================================================================================================
