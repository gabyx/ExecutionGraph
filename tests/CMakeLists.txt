
# Add googletest
find_package(GoogleTestLib)

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
    ${CMAKE_CURRENT_SOURCE_DIR}/include
    ${PROJECT_BINARY_DIR}/include

    ${googletest_SOURCE_DIR}/googletest/include 
    ${googletest_SOURCE_DIR}/googlemock/include 
)

# Define function to define compile definitions for all tests
function(defineCompileDefs target)

    message(STATUS "====================================================")
    message(STATUS "Configuring target: ${target}")

    setTargetCompileOptionsExecutionGraph(${target})
    target_include_directories(${target} PRIVATE ${INCLUDE_DIRS})
    target_link_libraries(${target} ExecutionGraph::CoreForTests gtest gmock_main ${ExecutionGraph_LINK_TARGETS_GUI_DEP})

    if(${CMAKE_CXX_COMPILER_ID} STREQUAL "GNU" OR ${CMAKE_CXX_COMPILER_ID} STREQUAL "Clang" OR ${CMAKE_CXX_COMPILER_ID} STREQUAL "Intel")
        target_compile_options(${target} PRIVATE -DExecutionGraph_BUILD_TESTS )
    else()
        target_compile_options(${target} PRIVATE /DExecutionGraph_BUILD_TESTS )
    endif()

    cotire(${target})

    print_target_properties(${target})

    add_test(NAME ${target} COMMAND ${target})

endfunction()

# NodeTests
set(EXEC_NAME1 ${PROJECT_NAME}Test-LogicNodes)
add_executable(${EXEC_NAME1}  ${CMAKE_CURRENT_SOURCE_DIR}/src/main_LogicNode.cpp ${SOURCE_FILES} ${INCLUDE_FILES}   )
defineCompileDefs(${EXEC_NAME1})

#ExecutionTree Test
set(EXEC_NAME2 ${PROJECT_NAME}Test-ExecutionTree)
add_executable(${EXEC_NAME2}  ${CMAKE_CURRENT_SOURCE_DIR}/src/main_ExecutionTree.cpp ${SOURCE_FILES} ${INCLUDE_FILES}   )
defineCompileDefs(${EXEC_NAME2})

#ExecutionTree Test
set(EXEC_NAME3 ${PROJECT_NAME}Test-CustomSocketTypes)
add_executable(${EXEC_NAME3}  ${CMAKE_CURRENT_SOURCE_DIR}/src/main_CustomSocketTypes.cpp ${SOURCE_FILES} ${INCLUDE_FILES}   )
defineCompileDefs(${EXEC_NAME3})

#Factory Test
set(EXEC_NAME4 ${PROJECT_NAME}Test-Factory)
add_executable(${EXEC_NAME4}  ${CMAKE_CURRENT_SOURCE_DIR}/src/main_Factory.cpp ${SOURCE_FILES} ${INCLUDE_FILES}   )
defineCompileDefs(${EXEC_NAME4})