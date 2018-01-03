
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

set(ExecutionGraph_TESTS_OUTPUT_FILES_DIR "${PROJECT_BINARY_DIR}")
set(ExecutionGraph_TESTS_INPUT_FILES_DIR "${CMAKE_CURRENT_SOURCE_DIR}/files")
set(ExecutionGraph_TESTS_INPUT_FILES_ADD_DIR "${ExecutionGraph_ROOT_DIR}/additional/tests/files")
set(ExecutionGraph_TESTS_VALIDATION_FILES_DIR "${ExecutionGraph_ROOT_DIR}/additional/tests/files/validation")

# WRITE CONFIGURATION FILE
configure_file (
  "${CMAKE_CURRENT_SOURCE_DIR}/include/TestConfig.hpp.in.cmake"
  ${PROJECT_BINARY_DIR}/include/TestConfig.hpp
)
#=========================


set(SOURCE_FILES
    ${ExecutionGraph_SRC}
    ${CMAKE_CURRENT_SOURCE_DIR}/include/TestFunctions.hpp
)

set(INCLUDE_FILES
    ${ExecutionGraph_INC}
    ${CMAKE_CURRENT_SOURCE_DIR}/src/TestFunctions.cpp
)


#include directories
set(INCLUDE_DIRS
    ${ExecutionGraph_INC_DIRS}
    ${CMAKE_CURRENT_SOURCE_DIR}/include/
    ${PROJECT_BINARY_DIR}/include/
)

# Define function to define compile definitions for all tests
function(defineCompileDefs Target)
    if(${CMAKE_CXX_COMPILER_ID} STREQUAL "GNU" OR ${CMAKE_CXX_COMPILER_ID} STREQUAL "Clang" OR ${CMAKE_CXX_COMPILER_ID} STREQUAL "Intel")
        target_compile_options(${Target} PRIVATE -DExecutionGraph_BUILD_TESTS )
    else()
        target_compile_options(${Target} PRIVATE /DExecutionGraph_BUILD_TESTS )
    endif()
endfunction()

# NodeTests
set(EXEC_NAME1 ${PROJECT_NAME}-LogicNodes)
add_executable(${EXEC_NAME1}  ${CMAKE_CURRENT_SOURCE_DIR}/src/main_LogicNode.cpp ${SOURCE_FILES} ${INCLUDE_FILES}   )
target_include_directories(${EXEC_NAME1} ${INCLUDE_DIRS})
target_link_libraries(${EXEC_NAME1} ExecutionGraphForTestsOnly gtest gmock_main)
add_dependencies(build_and_test ${EXEC_NAME1})
if( NOT "${ExecutionGraph_DEPENDING_TARGETS}" STREQUAL "")
    add_dependencies(${EXEC_NAME1} ${ExecutionGraph_DEPENDING_TARGETS})
endif()
defineCompileDefs(${EXEC_NAME1})
cotire(${EXEC_NAME1})

#ExecutionTree Test
set(EXEC_NAME2 ${PROJECT_NAME}-ExecutionTree)
add_executable(${EXEC_NAME2}  ${CMAKE_CURRENT_SOURCE_DIR}/src/main_ExecutionTree.cpp ${SOURCE_FILES} ${INCLUDE_FILES}   )
target_include_directories(${EXEC_NAME2} ${INCLUDE_DIRS})
target_link_libraries(${EXEC_NAME2} ExecutionGraphForTestsOnly gtest gmock_main)
add_dependencies(build_and_test ${EXEC_NAME2})
if( NOT "${ExecutionGraph_DEPENDING_TARGETS}" STREQUAL "")
    add_dependencies(${EXEC_NAME2} ${ExecutionGraph_DEPENDING_TARGETS})
endif()
defineCompileDefs(${EXEC_NAME2})
cotire(${EXEC_NAME2})

#ExecutionTree Test
set(EXEC_NAME3 ${PROJECT_NAME}-CustomSocketTypes)
add_executable(${EXEC_NAME3}  ${CMAKE_CURRENT_SOURCE_DIR}/src/main_CustomSocketTypes.cpp ${SOURCE_FILES} ${INCLUDE_FILES}   )
target_include_directories(${EXEC_NAME3} ${INCLUDE_DIRS})
target_link_libraries(${EXEC_NAME3} ExecutionGraphForTestsOnly gtest gmock_main)
add_dependencies(build_and_test ${EXEC_NAME3})
if( NOT "${ExecutionGraph_DEPENDING_TARGETS}" STREQUAL "")
    add_dependencies(${EXEC_NAME3} ${ExecutionGraph_DEPENDING_TARGETS})
endif()
defineCompileDefs(${EXEC_NAME3})
cotire(${EXEC_NAME3})

add_test(NAME    ${EXEC_NAME1} COMMAND ${EXEC_NAME1})
add_test(NAME    ${EXEC_NAME2} COMMAND ${EXEC_NAME2})
add_test(NAME    ${EXEC_NAME3} COMMAND ${EXEC_NAME3})