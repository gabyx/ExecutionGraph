MACRO(ExecutionGraph_WRITE_CONFIG_FILE ExecutionGraph_CONFIG_FILE ExecutionGraph_ROOT_DIR )

  
    configure_file(
      ${ExecutionGraph_ROOT_DIR}/include/executionGraph/config/Config.hpp.in.cmake
      ${ExecutionGraph_CONFIG_FILE}
    )

ENDMACRO()

