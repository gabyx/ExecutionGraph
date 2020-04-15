set(CMAKE_MODULE_PATH "${CMAKE_CURRENT_SOURCE_DIR}/cmake" ${CMAKE_MODULE_PATH})

set(ExecutionGraph_CLIENT_DIR ${CMAKE_CURRENT_SOURCE_DIR})
set(ExecutionGraph_CLIENT_BINARY_DIR ${CMAKE_CURRENT_BINARY_DIR})
set(ExecutionGraph_CLIENT_OUT_DIR
    "${CMAKE_CURRENT_BINARY_DIR}/build"
    CACHE INTERNAL "" FORCE)

message(STATUS "Configuring target: ExecutionGraphApp ...")

find_package(Angular REQUIRED)

if(UNIX)
    set(buildFor "linux")
elseif(APPLE)
    set(buildFor "macos")
elseif(WIN32)
    set(buildFor "win")
endif()

configure_file(${ExecutionGraph_CLIENT_DIR}/cmake/package.json.cmake
               ${ExecutionGraph_CLIENT_BINARY_DIR}/package.json)

configure_file(${ExecutionGraph_CLIENT_DIR}/cmake/InstallPackage.cmake
               ${ExecutionGraph_CLIENT_BINARY_DIR}/InstallPackage.cmake)

add_custom_command(
    OUTPUT "${ExecutionGraph_CLIENT_OUT_DIR}/main.js"
    COMMAND "${NODE_PACKAGE_MANAGER_EXECUTABLE}" "run" "native:cmake-build-to" "--"
            "${ExecutionGraph_CLIENT_OUT_DIR}" "${ExecutionGraph_CLIENT_BINARY_DIR}" "${buildFor}"
    WORKING_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR})

add_custom_target("ExecutionGraphApp" ALL DEPENDS "${ExecutionGraph_CLIENT_OUT_DIR}/main.js")

add_dependencies("ExecutionGraphApp" "ExecutionGraphServer")

install(
    TARGETS ExecutionGraphServer ExecutionGraph-Core ExecutionGraph-Serialization
    RUNTIME
        DESTINATION ${ExecutionGraph_CLIENT_OUT_DIR}/server/${ExecutionGraph_RUNTIME_INSTALL_DIR}
    LIBRARY
        DESTINATION ${ExecutionGraph_CLIENT_OUT_DIR}/server/${ExecutionGraph_LIBRARY_INSTALL_DIR}
    ARCHIVE
        DESTINATION ${ExecutionGraph_CLIENT_OUT_DIR}/server/${ExecutionGraph_ARCHIVE_INSTALL_DIR}
    FRAMEWORK
        DESTINATION ${ExecutionGraph_CLIENT_OUT_DIR}/server/${ExecutionGraph_FRAMEWORK_INSTALL_DIR})

install(SCRIPT ${ExecutionGraph_CLIENT_BINARY_DIR}/InstallPackage.cmake)
