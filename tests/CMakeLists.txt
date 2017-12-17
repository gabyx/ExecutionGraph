#The name of the project ====================================================================
PROJECT("ExecutionGraphTests")


# Add GoogleTests ===========================================================================
if (CMAKE_VERSION VERSION_LESS 3.2)
    set(UPDATE_DISCONNECTED_IF_AVAILABLE "")
else()
    set(UPDATE_DISCONNECTED_IF_AVAILABLE "UPDATE_DISCONNECTED 1")
endif()

include(DownloadProject)
download_project(PROJ                googletest
                 GIT_REPOSITORY      https://github.com/google/googletest.git
                 GIT_TAG             master
                 ${UPDATE_DISCONNECTED_IF_AVAILABLE}
)

# Prevent GoogleTest from overriding our compiler/linker options
# when building with Visual Studio
set(gtest_force_shared_crt ON CACHE BOOL "" FORCE)


add_subdirectory(${googletest_SOURCE_DIR} ${googletest_BINARY_DIR} EXCLUDE_FROM_ALL)

# When using CMake 2.8.11 or later, header path dependencies
# are automatically added to the gtest and gmock targets.
# For earlier CMake versions, we have to explicitly add the
# required directories to the header search path ourselves.
#if (CMAKE_VERSION VERSION_LESS 2.8.11)
    #include_directories("${gtest_SOURCE_DIR}/include"
                        #"${gmock_SOURCE_DIR}/include")
#endif()
# ==============================================================================================

SET(ExecutionGraph_TESTS_OUTPUT_FILES_DIR "${PROJECT_BINARY_DIR}")
SET(ExecutionGraph_TESTS_INPUT_FILES_DIR "${PROJECT_SOURCE_DIR}/files")
SET(ExecutionGraph_TESTS_INPUT_FILES_ADD_DIR "${ExecutionGraph_ROOT_DIR}/additional/tests/files")
SET(ExecutionGraph_TESTS_VALIDATION_FILES_DIR "${ExecutionGraph_ROOT_DIR}/additional/tests/files/validation")

# WRITE CONFIGURATION FILE
configure_file (
  "${PROJECT_SOURCE_DIR}/include/TestConfig.hpp.in.cmake"
  ${PROJECT_BINARY_DIR}/include/TestConfig.hpp
)
#=========================


SET(SOURCE_FILES
    ${ExecutionGraph_SRC}
    ${PROJECT_SOURCE_DIR}/include/TestFunctions.hpp
)

SET(INCLUDE_FILES
    ${ExecutionGraph_INC}
    ${PROJECT_SOURCE_DIR}/src/TestFunctions.cpp
)


#include directories
set(INCLUDE_DIRS
    ${ExecutionGraph_INC_DIRS}
    ${PROJECT_SOURCE_DIR}/include/
    ${PROJECT_BINARY_DIR}/include/
)

include_directories(${INCLUDE_DIRS})

# Define function to define compile definitions for all tests
function(defineCompileDefs Target)
    IF(${CMAKE_CXX_COMPILER_ID} STREQUAL "GNU" OR ${CMAKE_CXX_COMPILER_ID} STREQUAL "Clang" OR ${CMAKE_CXX_COMPILER_ID} STREQUAL "Intel")
        target_compile_options(${Target} PRIVATE -DExecutionGraph_BUILD_TESTS )
    ELSE()
        target_compile_options(${Target} PRIVATE /DExecutionGraph_BUILD_TESTS )
    ENDIF()
endfunction()

# NodeTests
SET(EXEC_NAME1 ${PROJECT_NAME}-LogicNodes)
ADD_EXECUTABLE(${EXEC_NAME1}  ${PROJECT_SOURCE_DIR}/src/main_LogicNode.cpp ${SOURCE_FILES} ${INCLUDE_FILES}   )
TARGET_LINK_LIBRARIES(${EXEC_NAME1} ExecutionGraphForTestsOnly gtest gmock_main)
ADD_DEPENDENCIES(build_and_test ${EXEC_NAME1})
if( NOT "${ExecutionGraph_DEPENDING_TARGETS}" STREQUAL "")
    ADD_DEPENDENCIES(${EXEC_NAME1} ${ExecutionGraph_DEPENDING_TARGETS})
endif()
defineCompileDefs(${EXEC_NAME1})
cotire(${EXEC_NAME1})

#ExecutionTree Test
SET(EXEC_NAME2 ${PROJECT_NAME}-ExecutionTree)
ADD_EXECUTABLE(${EXEC_NAME2}  ${PROJECT_SOURCE_DIR}/src/main_ExecutionTree.cpp ${SOURCE_FILES} ${INCLUDE_FILES}   )
TARGET_LINK_LIBRARIES(${EXEC_NAME2} ExecutionGraphForTestsOnly gtest gmock_main)
ADD_DEPENDENCIES(build_and_test ${EXEC_NAME2})
if( NOT "${ExecutionGraph_DEPENDING_TARGETS}" STREQUAL "")
    ADD_DEPENDENCIES(${EXEC_NAME2} ${ExecutionGraph_DEPENDING_TARGETS})
endif()
defineCompileDefs(${EXEC_NAME2})
cotire(${EXEC_NAME2})

#ExecutionTree Test
SET(EXEC_NAME3 ${PROJECT_NAME}-CustomSocketTypes)
ADD_EXECUTABLE(${EXEC_NAME3}  ${PROJECT_SOURCE_DIR}/src/main_CustomSocketTypes.cpp ${SOURCE_FILES} ${INCLUDE_FILES}   )
TARGET_LINK_LIBRARIES(${EXEC_NAME3} ExecutionGraphForTestsOnly gtest gmock_main)
ADD_DEPENDENCIES(build_and_test ${EXEC_NAME3})
if( NOT "${ExecutionGraph_DEPENDING_TARGETS}" STREQUAL "")
    ADD_DEPENDENCIES(${EXEC_NAME3} ${ExecutionGraph_DEPENDING_TARGETS})
endif()
defineCompileDefs(${EXEC_NAME3})
cotire(${EXEC_NAME3})

add_test(NAME    ${EXEC_NAME1} COMMAND ${EXEC_NAME1})
add_test(NAME    ${EXEC_NAME2} COMMAND ${EXEC_NAME2})
add_test(NAME    ${EXEC_NAME3} COMMAND ${EXEC_NAME3})
