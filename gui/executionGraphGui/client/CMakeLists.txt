

set(CMAKE_MODULE_PATH "${CMAKE_CURRENT_SOURCE_DIR}/cmake" ${CMAKE_MODULE_PATH})


set(ExecutionGraphGUI_CLIENT_TARGET "ExecutionGraphGUI-Client" CACHE INTERNAL "" FORCE)
set(ExecutionGraphGUI_CLIENT_TARGET_OUT_DIR "${CMAKE_CURRENT_BINARY_DIR}/build" CACHE INTERNAL "" FORCE)

message(STATUS "Configuring target: ${ExecutionGraphGUI_CLIENT_TARGET} ...")

find_package(Angular REQUIRED)
add_custom_target(${ExecutionGraphGUI_CLIENT_TARGET} ALL
                  COMMAND "${ANGULAR_CLI_EXECUTABLE}" "build" "--output-path" "${ExecutionGraphGUI_CLIENT_TARGET_OUT_DIR}/client"
                  WORKING_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR})

# add_custom_target(${ExecutionGraphGUI_CLIENT_TARGET} ALL
#                 COMMAND "echo" "Start Angular 'ng build --watch'"
#                 WORKING_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR})