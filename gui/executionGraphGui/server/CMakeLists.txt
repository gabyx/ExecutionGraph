
set(ExecutionGraphGUIServer_SRCS

    ${ExecutionGraphGUICommon_SRCS}
    ${ExecutionGraphGUIBackend_SRCS}

    ${CMAKE_CURRENT_SOURCE_DIR}/ServerCLArgs.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/MimeType.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/HttpCommon.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/HttpSession.cpp
    #${CMAKE_CURRENT_SOURCE_DIR}/HttpListener.cpp
    #${CMAKE_CURRENT_SOURCE_DIR}/HttpWorker.cpp
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
                                            ExecutionGraph::Serialization 
                                            ExecutionGraphGui::Core-Dependencies)

    set_target_properties(${target} PROPERTIES INSTALL_RPATH "$ORIGIN/../lib;$ORIGIN/..")

    cotire(${target})

    print_target_properties(${target})

endfunction()

add_executable(ExecutionGraphServer ${ExecutionGraphGUIServer_SRCS})
defineCompileDefs(ExecutionGraphServer ${ExecutionGraphServer_INC_DIRS})

# Spezial for 
# add_executable(ExecutionGraphServer-ForClient ${ExecutionGraphGUIServer_SRCS})
# defineCompileDefs(ExecutionGraphServer-ForClient ${ExecutionGraphServer_INC_DIRS})
# set_target_properties(ExecutionGraphServer-ForClient PROPERTIES BUILD_RPATH 
#                                                      "$ORIGIN")

install(TARGETS ExecutionGraphServer EXPORT ExecutionGraphTargets
        RUNTIME       DESTINATION ${ExecutionGraph_RUNTIME_INSTALL_DIR}
        LIBRARY       DESTINATION ${ExecutionGraph_LIBRARY_INSTALL_DIR}
        ARCHIVE       DESTINATION ${ExecutionGraph_ARCHIVE_INSTALL_DIR}
        FRAMEWORK     DESTINATION ${ExecutionGraph_FRAMEWORK_INSTALL_DIR})