
# Add googletest
find_package(GoogleTestLib)

set(ExecutionGraph_TESTS_OUTPUT_FILES_DIR "${PROJECT_BINARY_DIR}")
set(ExecutionGraph_TESTS_INPUT_FILES_DIR "${CMAKE_CURRENT_SOURCE_DIR}/files")
set(ExecutionGraph_TESTS_INPUT_FILES_ADD_DIR "${ExecutionGraph_ROOT_DIR}/additional/tests/files")
set(ExecutionGraph_TESTS_VALIDATION_FILES_DIR "${ExecutionGraph_ROOT_DIR}/additional/tests/files/validation")

# WRITE CONFIGURATION FILE
configure_file (
  "${CMAKE_CURRENT_SOURCE_DIR}/include/TestConfig.hpp.in.cmake"
  ${CMAKE_CURRENT_BINARY_DIR}/include/TestConfig.hpp
)
#=========================

set(INCLUDE_FILES
    ${CMAKE_CURRENT_SOURCE_DIR}/include/TestFunctions.hpp
)

set(SOURCE_FILES
    ${CMAKE_CURRENT_SOURCE_DIR}/src/TestFunctions.cpp
)

#include directories
set(INCLUDE_DIRS
    ${CMAKE_CURRENT_SOURCE_DIR}/include
    ${CMAKE_CURRENT_BINARY_DIR}/include
)

# Define function to define compile definitions for all tests
function(defineCompileDefs target)

    message(STATUS "====================================================")
    message(STATUS "Configuring target: ${target}")

    setTargetCompileOptionsExecutionGraph(${target} ${ExecutionGraph_USE_ADDRESS_SANITIZER} ${ExecutionGraph_USE_LEAK_SANITIZER})
    target_include_directories(${target} PRIVATE ${INCLUDE_DIRS})
    target_link_libraries(${target} PRIVATE ExecutionGraph::CoreForTests gtest gmock_main)
    add_dependencies(build_and_test ${target})

    cotire(${target})

    print_target_properties(${target})

    add_test(NAME ${target} COMMAND ${target})

endfunction()

# NodeTests
set(EXEC_NAME ${PROJECT_NAME}Test-LogicNodes)
add_executable(${EXEC_NAME}  ${CMAKE_CURRENT_SOURCE_DIR}/src/main_LogicNode.cpp ${SOURCE_FILES} ${INCLUDE_FILES}   )
defineCompileDefs(${EXEC_NAME})

#ExecutionTree Test
set(EXEC_NAME ${PROJECT_NAME}Test-ExecutionTree)
add_executable(${EXEC_NAME}  ${CMAKE_CURRENT_SOURCE_DIR}/src/main_ExecutionTree.cpp ${SOURCE_FILES} ${INCLUDE_FILES}   )
defineCompileDefs(${EXEC_NAME})

#ExecutionTree Test
set(EXEC_NAME ${PROJECT_NAME}Test-CustomSocketTypes)
add_executable(${EXEC_NAME}  ${CMAKE_CURRENT_SOURCE_DIR}/src/main_CustomSocketTypes.cpp ${SOURCE_FILES} ${INCLUDE_FILES}   )
defineCompileDefs(${EXEC_NAME})

#Factory Test
set(EXEC_NAME ${PROJECT_NAME}Test-Factory)
add_executable(${EXEC_NAME}  ${CMAKE_CURRENT_SOURCE_DIR}/src/main_Factory.cpp ${SOURCE_FILES} ${INCLUDE_FILES}   )
defineCompileDefs(${EXEC_NAME})

# JSON Test
set(EXEC_NAME ${PROJECT_NAME}Test-Json)
add_executable(${EXEC_NAME}  ${CMAKE_CURRENT_SOURCE_DIR}/src/main_Json.cpp ${SOURCE_FILES} ${INCLUDE_FILES}   )
defineCompileDefs(${EXEC_NAME})

# TaskQueue Test
set(EXEC_NAME ${PROJECT_NAME}Test-TaskQueue)
add_executable(${EXEC_NAME}  ${CMAKE_CURRENT_SOURCE_DIR}/src/main_TaskQueue.cpp ${SOURCE_FILES} ${INCLUDE_FILES}   )
defineCompileDefs(${EXEC_NAME})

# Flatbuffer Test
set(EXEC_NAME ${PROJECT_NAME}Test-Flatbuffer)
add_executable(${EXEC_NAME}  ${CMAKE_CURRENT_SOURCE_DIR}/src/main_Flatbuffer.cpp ${SOURCE_FILES} ${INCLUDE_FILES}   )
defineCompileDefs(${EXEC_NAME})

# MetaProgramming Test
set(EXEC_NAME ${PROJECT_NAME}Test-MetaProgramming)
add_executable(${EXEC_NAME}  ${CMAKE_CURRENT_SOURCE_DIR}/src/main_MetaProg.cpp ${SOURCE_FILES} ${INCLUDE_FILES}   )
defineCompileDefs(${EXEC_NAME})

# Synchronized Test
set(EXEC_NAME ${PROJECT_NAME}Test-Synchronized)
add_executable(${EXEC_NAME}  ${CMAKE_CURRENT_SOURCE_DIR}/src/main_Synchronized.cpp ${SOURCE_FILES} ${INCLUDE_FILES}   )
defineCompileDefs(${EXEC_NAME})