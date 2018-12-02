set(ExecutionGraphGUIServer_SRCS

    ${ExecutionGraphGUICommon_SRCS}
    ${ExecutionGraphGUIBackend_SRCS}

    ${CMAKE_CURRENT_SOURCE_DIR}/main.cpp
)


set(ExecutionGraphServer_INC_DIRS
    ${CMAKE_CURRENT_SOURCE_DIR}/../..
)

function(defineCompileDefs target includeDirs)

    message(STATUS "====================================================")
    message(STATUS "Configuring target: ${target}")

    setTargetCompileOptionsExecutionGraph(${target} ${ExecutionGraph_USE_ADDRESS_SANITIZER} ${ExecutionGraph_USE_LEAK_SANITIZER})
    target_include_directories(${target} PRIVATE ${includeDirs})
    target_link_libraries(${target} PRIVATE ExecutionGraph::Core 
                                            ExecutionGraphGui::Core-Dependencies 
                                            boostBeastLib)

    cotire(${target})

    print_target_properties(${target})

endfunction()

add_executable(ExecutionGraphServer ${ExecutionGraphGUIServer_SRCS})
defineCompileDefs(ExecutionGraphServer ${ExecutionGraphServer_INC_DIRS})


