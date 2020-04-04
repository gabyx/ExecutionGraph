
# Add googletest
find_package(GoogleTestLib)

set(ExecutionGraph_TESTS_OUTPUT_FILES_DIR "${PROJECT_BINARY_DIR}")
set(ExecutionGraph_TESTS_INPUT_FILES_DIR "${CMAKE_CURRENT_SOURCE_DIR}/files")
set(ExecutionGraph_TESTS_INPUT_FILES_ADD_DIR "${ExecutionGraph_ROOT_DIR}/additional/tests/files")
set(ExecutionGraph_TESTS_VALIDATION_FILES_DIR "${ExecutionGraph_ROOT_DIR}/additional/tests/files/validation")

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

function(addTarget targetName name)

    message(STATUS "Configuring target: ${targetName}")
    add_executable(${targetName} 
                   ${CMAKE_CURRENT_SOURCE_DIR}/src/main-${name}.cpp 
                   ${SOURCE_FILES} 
                   ${INCLUDE_FILES})

    setTargetCompileOptionsExecutionGraph(${target} ${ExecutionGraph_USE_ADDRESS_SANITIZER} ${ExecutionGraph_USE_LEAK_SANITIZER})
    target_include_directories(${target} PRIVATE ${INCLUDE_DIRS})
    target_link_libraries(${target} PRIVATE ExecutionGraph::CoreForTests gtest gmock_main)
    add_dependencies(build_and_test ${target})

    if(${ExecutionGraph_SETUP_COTIRE_TARGETS}) 
        cotire(${target})
    endif()

endfunction()

# Define function to define compile definitions for all tests
function(addTest name noCompile noCompileTests)

    message(STATUS "====================================================")
    set(target ${PROJECT_NAME}Test-${name})

    #print_target_properties(${target})

    addTarget(${target} ${name})
    add_test(NAME ${target} COMMAND ${target})  

    if(${noCompile})
        message(STATUS "Adding '${noCompileTests}' no compile tests")
        
        foreach(testIndex RANGE noCompileTests)
            set(targetNC "${target}-NoCompile-${testIndex}")

            addTarget(${targetNC} ${name})
            target_compile_definitions(${targetNC} PRIVATE "EG_NO_COMPILE_TEST_INDEX=${testIndex}")
            set_target_properties(${targetNC} PROPERTIES EXCLUDE_FROM_ALL TRUE)

            add_test(NAME ${targetNC}
                    COMMAND ${CMAKE_COMMAND} --build . --target ${targetNC} --config $<CONFIGURATION>
                    WORKING_DIRECTORY ${CMAKE_BINARY_DIR})
            set_tests_properties(${targetNC} PROPERTIES WILL_FAIL TRUE)

        endforeach()
    endif()

endfunction()

addTest("LogicNode" ON 1)
addTest("LogicSocketData" OFF 0)
addTest("ExecutionTree" OFF 0)
addTest("CustomSocketTypes" OFF 0)
addTest("Factory" OFF 0)
addTest("TaskQueue" OFF 0)
addTest("MetaProgramming" OFF 0)
addTest("Synchronized" OFF 0)
addTest("Flags" OFF 0)
addTest("ConstExpr" OFF 0)
