include(DefineExecutionGraphSources)

function(addTestTarget targetName file addLibs)

    message(STATUS "Configuring target: ${targetName}")
    add_executable(${targetName} ${file} ${SOURCE_FILES} ${INCLUDE_FILES})

    setTargetCompileOptionsExecutionGraph(${targetName} ${ExecutionGraph_USE_ADDRESS_SANITIZER}
                                          ${ExecutionGraph_USE_LEAK_SANITIZER})
    target_include_directories(${targetName} PRIVATE ${INCLUDE_DIRS})
    target_link_libraries(${targetName} PRIVATE ExecutionGraph::CoreForTests gtest gmock_main)
    if(addLibs)
        target_link_libraries(${targetName} PRIVATE ${addLibs})
    endif()

    if(${ExecutionGraph_SETUP_COTIRE_TARGETS})
        cotire(${targetName})
    endif()

endfunction()

# Define function to define compile definitions for all tests
function(addTest name addLibs noCompile noCompileTests)

    message(STATUS "====================================================")
    set(target ${PROJECT_NAME}Test-${name})

    set(file ${CMAKE_CURRENT_SOURCE_DIR}/src/main-${name}.cpp)

    addTestTarget(${target} ${file} ${addLibs})
    add_dependencies(build_and_test ${target})
    add_test(NAME ${target} COMMAND ${target})

    if(noCompile)
        message(STATUS "Adding '${noCompileTests}' no compile tests")

        foreach(testIndex RANGE ${noCompileTests})
            set(targetNC "${target}-NoCompile-${testIndex}")

            addTestTarget(${targetNC} ${file} ${addLibs})
            target_compile_definitions(${targetNC} PRIVATE "EG_NO_COMPILE_TEST_INDEX=${testIndex}")
            set_target_properties(${targetNC} PROPERTIES EXCLUDE_FROM_ALL TRUE)

            add_test(NAME ${targetNC}
                     COMMAND bash ${CMAKE_CURRENT_SOURCE_DIR}/scripts/noCompile.sh ${CMAKE_COMMAND}
                             ${targetNC} $<CONFIGURATION> ${CMAKE_BINARY_DIR} ${testIndex} ${file})

        endforeach()
    endif()

    print_target_properties(${target})

endfunction()
