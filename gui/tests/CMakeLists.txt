set(SOURCE_FILES
    ${PROJECT_SOURCE_DIR}/tests/src/TestFunctions.cpp
)

set(INCLUDE_FILES
)

#include directories
set(INCLUDE_DIRS
    ${PROJECT_SOURCE_DIR}/tests/include/
    ${CMAKE_CURRENT_SOURCE_DIR}/../
    ${CMAKE_CURRENT_SOURCE_DIR}/include
    ${CMAKE_CURRENT_SOURCE_DIR}/files
)

# Define function to define compile definitions for all tests
function(defineCompileDefs target addLibs addTest)

    message(STATUS "====================================================")
    message(STATUS "Configuring target: ${target}")

    setTargetCompileOptionsExecutionGraph(${target} ${ExecutionGraph_USE_ADDRESS_SANITIZER} ${ExecutionGraph_USE_LEAK_SANITIZER})
    target_include_directories(${target} PRIVATE ${INCLUDE_DIRS})
    target_link_libraries(${target} PRIVATE ExecutionGraph::CoreForTests ExecutionGraph::Serialization gtest gmock_main ${addLibs})
    add_dependencies(build_and_test ${target})

     if(${ExecutionGraph_SETUP_COTIRE_TARGETS}) 
        cotire(${target})
    endif()

    print_target_properties(${target})

    if(${addTest})
        add_test(NAME ${target} COMMAND ${target})
    endif()

endfunction()

# Flatbuffer Test
set(EXEC_NAME ${PROJECT_NAME}Test-Flatbuffer)
add_executable(${EXEC_NAME}  ${CMAKE_CURRENT_SOURCE_DIR}/src/main_Flatbuffer.cpp ${SOURCE_FILES} ${INCLUDE_FILES}   )
defineCompileDefs(${EXEC_NAME} "flatbuffersLib" "rttrLib")

# Memory Tests
set(EXEC_NAME ${PROJECT_NAME}GuiTest-MemoryPoolNormal)
add_executable(${EXEC_NAME}  ${CMAKE_CURRENT_SOURCE_DIR}/src/main_MemoryPoolNormal.cpp ${SOURCE_FILES} ${INCLUDE_FILES})
defineCompileDefs(${EXEC_NAME} "memoryLib" "on")

# Http Server
set(EXEC_NAME ${PROJECT_NAME}GuiTest-HttpServer)
add_executable(${EXEC_NAME}  ${CMAKE_CURRENT_SOURCE_DIR}/src/main_HttpServer.cpp ${SOURCE_FILES} ${INCLUDE_FILES})
defineCompileDefs(${EXEC_NAME} "boostBeastLib" "off")

# Http Server Async
set(EXEC_NAME ${PROJECT_NAME}GuiTest-HttpServerAsync)
add_executable(${EXEC_NAME}  ${CMAKE_CURRENT_SOURCE_DIR}/src/main_HttpServerAsync.cpp ${SOURCE_FILES} ${INCLUDE_FILES})
defineCompileDefs(${EXEC_NAME} "boostBeastLib" "off")