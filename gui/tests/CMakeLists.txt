# WRITE CONFIGURATION FILE
configure_file (
  "${PROJECT_SOURCE_DIR}/tests/include/TestConfig.hpp.in.cmake"
  ${CMAKE_CURRENT_BINARY_DIR}/include/TestConfig.hpp
)
#=========================

set(SOURCE_FILES
    ${PROJECT_SOURCE_DIR}/tests/src/TestFunctions.cpp
)

set(INCLUDE_FILES
    ${PROJECT_SOURCE_DIR}/tests/include/TestFunctions.hpp
)

#include directories
set(INCLUDE_DIRS
    ${PROJECT_SOURCE_DIR}/tests/include
    ${CMAKE_CURRENT_SOURCE_DIR}/include
    ${CMAKE_CURRENT_BINARY_DIR}/include
)

# Define function to define compile definitions for all tests
function(defineCompileDefs target addLibs)

    message(STATUS "====================================================")
    message(STATUS "Configuring target: ${target}")

    setTargetCompileOptionsExecutionGraph(${target} ${ExecutionGraph_USE_ADDRESS_SANITIZER} ${ExecutionGraph_USE_LEAK_SANITIZER})
    target_include_directories(${target} PRIVATE ${INCLUDE_DIRS})
    target_link_libraries(${target} PRIVATE ExecutionGraph::CoreForTests gtest gmock_main ${addLibs})
    add_dependencies(build_and_test ${target})

    cotire(${target})

    print_target_properties(${target})

    add_test(NAME ${target} COMMAND ${target})

endfunction()

# NodeTests
# TaskQueue Test
# you need to `ln -s "$(pwd)" "Frameworks"` in `buildExternal/cefbinaries/cefbinaries-src/Debug`
set(EXEC_NAME ${PROJECT_NAME}GuiTest-MemoryPool)
add_executable(${EXEC_NAME}  ${CMAKE_CURRENT_SOURCE_DIR}/src/main_MemoryPool.cpp ${SOURCE_FILES} ${INCLUDE_FILES})
defineCompileDefs(${EXEC_NAME} cefLib)

set(EXEC_NAME ${PROJECT_NAME}GuiTest-MemoryPoolNormal)
add_executable(${EXEC_NAME}  ${CMAKE_CURRENT_SOURCE_DIR}/src/main_MemoryPoolNormal.cpp ${SOURCE_FILES} ${INCLUDE_FILES})
defineCompileDefs(${EXEC_NAME} "")

# Http Server
set(EXEC_NAME ${PROJECT_NAME}GuiTest-HttpServer)
add_executable(${EXEC_NAME}  ${CMAKE_CURRENT_SOURCE_DIR}/src/main_HttpServer.cpp ${SOURCE_FILES} ${INCLUDE_FILES})
defineCompileDefs(${EXEC_NAME} boostBeastLib)
# Http Server Async
set(EXEC_NAME ${PROJECT_NAME}GuiTest-HttpServerAsync)
add_executable(${EXEC_NAME}  ${CMAKE_CURRENT_SOURCE_DIR}/src/main_HttpServerAsync.cpp ${SOURCE_FILES} ${INCLUDE_FILES})
defineCompileDefs(${EXEC_NAME} boostBeastLib)