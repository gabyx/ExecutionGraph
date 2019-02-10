

set(CMAKE_MODULE_PATH "${CMAKE_CURRENT_SOURCE_DIR}/cmake" ${CMAKE_MODULE_PATH})

set(ExecutionGraphGui_CLIENT_OUT_DIR "${CMAKE_CURRENT_BINARY_DIR}" CACHE INTERNAL "" FORCE)

message(STATUS "Configuring target: ${ExecutionGraphGui_CLIENT_TARGET} ...")

find_package(Angular REQUIRED)
add_custom_target("ExecutionGraphGui-client" ALL
                  COMMAND "${NODE_PACKAGE_MANAGER_EXECUTABLE}"
                          "run" "electron:package-to" "--"
                          "${ExecutionGraphGui_CLIENT_OUT_DIR}"
                  WORKING_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR})


add_custom_target("${ExecutionGraphGui_CLIENT_TARGET}-Clean"
                  COMMAND "rm" "-r" "${ExecutionGraphGui_CLIENT_OUT_DIR}"
                  WORKING_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR})
