

set(CMAKE_MODULE_PATH "${CMAKE_CURRENT_SOURCE_DIR}/cmake" ${CMAKE_MODULE_PATH})


set(ExecutionGraph_CLIENT_DIR ${CMAKE_CURRENT_SOURCE_DIR})
set(ExecutionGraph_CLIENT_BINARY_DIR ${CMAKE_CURRENT_BINARY_DIR})
set(ExecutionGraph_CLIENT_OUT_DIR "${CMAKE_CURRENT_BINARY_DIR}/build" CACHE INTERNAL "" FORCE)

message(STATUS "Configuring target: ExecutionGraphApp ...")

find_package(Angular REQUIRED)

configure_file(
  ${ExecutionGraph_CLIENT_DIR}/cmake/package.json.cmake
  ${ExecutionGraph_CLIENT_BINARY_DIR}/package.json
)

if (UNIX)
  set(buildFor "linux")
elseif (APPLE)
  set(buildFor "macos")
elseif(WIN32)
  set(buildFor "win")
endif()

add_custom_command(OUTPUT "${ExecutionGraph_CLIENT_OUT_DIR}/main.js"
                   COMMAND "${NODE_PACKAGE_MANAGER_EXECUTABLE}"
                           "run" "native:package-to" "--"
                           "${ExecutionGraph_CLIENT_OUT_DIR}"
                           "${ExecutionGraph_CLIENT_BINARY_DIR}"
                           "${buildFor}"
                   WORKING_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR})

add_custom_target("ExecutionGraphApp"
                  DEPENDS "${ExecutionGraph_CLIENT_OUT_DIR}/main.js")
