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

function(addTarget targetName file)

    message(STATUS "Configuring target: ${targetName}")
    add_executable(${targetName} ${file} ${SOURCE_FILES} ${INCLUDE_FILES})

    setTargetCompileOptionsExecutionGraph(${targetName} ${ExecutionGraph_USE_ADDRESS_SANITIZER}
                                          ${ExecutionGraph_USE_LEAK_SANITIZER})
    target_include_directories(${targetName} PRIVATE ${INCLUDE_DIRS})
    target_link_libraries(${targetName} PRIVATE ExecutionGraph::CoreForTests gtest gmock_main)

    if(${ExecutionGraph_SETUP_COTIRE_TARGETS})
        cotire(${targetName})
    endif()

endfunction()

# Define function to define compile definitions for all tests
function(addTest name noCompile noCompileTests)

    message(STATUS "====================================================")
    set(target ${PROJECT_NAME}Test-${name})

    #print_target_properties(${target})
    set(file ${CMAKE_CURRENT_SOURCE_DIR}/src/main-${name}.cpp)

    addTarget(${target} ${file})
    add_dependencies(build_and_test ${target})
    add_test(NAME ${target} COMMAND ${target})

    if(${noCompile})
        message(STATUS "Adding '${noCompileTests}' no compile tests")

        foreach(testIndex RANGE ${noCompileTests})
            set(targetNC "${target}-NoCompile-${testIndex}")

            addTarget(${targetNC} ${file})
            target_compile_definitions(${targetNC} PRIVATE "EG_NO_COMPILE_TEST_INDEX=${testIndex}")
            set_target_properties(${targetNC} PROPERTIES EXCLUDE_FROM_ALL TRUE)

            add_test(NAME ${targetNC}
                     COMMAND bash ${CMAKE_CURRENT_SOURCE_DIR}/scripts/noCompile.sh ${CMAKE_COMMAND}
                             ${targetNC} $<CONFIGURATION> ${CMAKE_BINARY_DIR} ${testIndex} ${file})

        endforeach()
    endif()

endfunction()

addTest("LogicNode" ON 1)
addTest("LogicSocketData" OFF 0)
# addTest("ExecutionTree" OFF 0)
# addTest("CustomSocketTypes" OFF 0)
addTest("Factory" OFF 0)
addTest("TaskQueue" OFF 0)
addTest("MetaProgramming" OFF 0)
addTest("Synchronized" OFF 0)
addTest("Flags" OFF 0)
