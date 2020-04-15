# Add googletest
find_package(GoogleTestLib)

set(ExecutionGraph_TESTS_OUTPUT_FILES_DIR "${PROJECT_BINARY_DIR}")
set(ExecutionGraph_TESTS_INPUT_FILES_DIR "${CMAKE_CURRENT_SOURCE_DIR}/files")
set(ExecutionGraph_TESTS_INPUT_FILES_ADD_DIR "${ExecutionGraph_ROOT_DIR}/additional/tests/files")
set(ExecutionGraph_TESTS_VALIDATION_FILES_DIR
    "${ExecutionGraph_ROOT_DIR}/additional/tests/files/validation")

set(INCLUDE_FILES ${CMAKE_CURRENT_SOURCE_DIR}/include/TestFunctions.hpp)

set(SOURCE_FILES ${CMAKE_CURRENT_SOURCE_DIR}/src/TestFunctions.cpp)

#include directories
set(INCLUDE_DIRS ${CMAKE_CURRENT_SOURCE_DIR}/include ${CMAKE_CURRENT_BINARY_DIR}/include)

include(AddTestTargets)

addTest("LogicNode" OFF ON 1)
addTest("LogicSocketData" OFF OFF 0)
# addTest("ExecutionTree" OFF OFF 0)
# addTest("CustomSocketTypes" OFF OFF 0)
addTest("Factory" OFF OFF 0)
addTest("TaskQueue" OFF OFF 0)
addTest("MetaProgramming" OFF OFF 0)
addTest("Synchronized" OFF OFF 0)
addTest("Flags" OFF OFF 0)
addTest("MemoryPool" "memoryLib" OFF 0)
