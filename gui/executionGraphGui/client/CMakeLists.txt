

set(CMAKE_MODULE_PATH "${CMAKE_CURRENT_SOURCE_DIR}/cmake" ${CMAKE_MODULE_PATH})

set(ExecutionGraphGui_CLIENT_TARGET "ExecutionGraphGui" CACHE INTERNAL "" FORCE)
set(ExecutionGraphGui_CLIENT_TARGET_OUT_DIR "${CMAKE_CURRENT_BINARY_DIR}/build" CACHE INTERNAL "" FORCE)

message(STATUS "Configuring target: ${ExecutionGraphGui_CLIENT_TARGET} ...")

find_package(Angular REQUIRED)
add_custom_target(${ExecutionGraphGui_CLIENT_TARGET} ALL
                  COMMAND "${CMAKE_CURRENT_SOURCE_DIR}/cmake/RunAngularBuild.sh"
                          "${NODE_PACKAGE_MANAGER_EXECUTABLE}"
                          "${ExecutionGraphGui_CLIENT_TARGET_OUT_DIR}/apps/eg"
                          "native:package"
                  WORKING_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR})

add_custom_target("${ExecutionGraphGui_CLIENT_TARGET}-Clean"
                  COMMAND "rm" "-r" "${ExecutionGraphGui_CLIENT_TARGET_OUT_DIR}"
                  WORKING_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR})
