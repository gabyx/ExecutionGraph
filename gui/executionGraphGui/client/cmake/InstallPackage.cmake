execute_process(COMMAND "${NODE_PACKAGE_MANAGER_EXECUTABLE}"
                           "run" "native:cmake-package-to" "--"
                           "${ExecutionGraph_CLIENT_OUT_DIR}"
                           "${ExecutionGraph_CLIENT_BINARY_DIR}"
                           "${buildFor}"
                WORKING_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR})
