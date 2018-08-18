

set(CMAKE_MODULE_PATH "${CMAKE_CURRENT_SOURCE_DIR}/cmake" ${CMAKE_MODULE_PATH})


set(ExecutionGraphGUI_CLIENT_TARGET "ExecutionGraphGUI-Client" CACHE INTERNAL "" FORCE)
set(ExecutionGraphGUI_CLIENT_TARGET_OUT_DIR "${CMAKE_CURRENT_BINARY_DIR}/build" CACHE INTERNAL "" FORCE)

message(STATUS "Configuring target: ${ExecutionGraphGUI_CLIENT_TARGET} ...")

find_package(Angular REQUIRED)
add_custom_target(${ExecutionGraphGUI_CLIENT_TARGET} ALL
                  COMMAND "${CMAKE_CURRENT_SOURCE_DIR}/cmake/RunAngularBuild.sh"
                          "${NODE_PACKAGE_MANAGER_EXECUTABLE}"
                          "${ExecutionGraphGUI_CLIENT_TARGET_OUT_DIR}/apps/eg"
                          "build-prod-app-eg"
                  WORKING_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR})

add_custom_target("${ExecutionGraphGUI_CLIENT_TARGET}-Clean"
                  COMMAND "rm" "-r" "${ExecutionGraphGUI_CLIENT_TARGET_OUT_DIR}"
                  WORKING_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR})
