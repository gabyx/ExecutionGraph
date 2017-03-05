MACRO(INCLUDE_ALL_ExecutionGraph_SOURCE 
      SRC 
      INC 
      INCLUDE_DIRS 
      DEPENDING_TARGETS # Input variable names
      ExecutionGraph_ROOT_DIR 
      ExecutionGraph_BINARY_DIR)  


    # Add all external sources/headers
    # INCLUDE(${ExecutionGraph_ROOT_DIR}/cmake/DefineExecutionGraphExternalSources.cmake)
    # no external sources up to now
    
    SET(${SRC}
        ${ExecutionGraph_ROOT_DIR}/src/DummyNode.cpp
    )

    SET(${INC}
        ${ExecutionGraph_ROOT_DIR}/include/ExecutionGraph/Nodes/DummyNode.hpp
    )

    SET(${INCLUDE_DIRS}
        ${ExecutionGraph_ROOT_DIR}/include
        ${ExecutionGraph_BINARY_DIR}/include
    )


    # WRITE CONFIGURATION FILE
    
    INCLUDE(${ExecutionGraph_ROOT_DIR}/cmake/WriteConfigFile.cmake)
    SET(ExecutionGraph_CONFIG_FILE ${ExecutionGraph_BINARY_DIR}/include/ExecutionGraph/Config/Config.hpp)
    MESSAGE(STATUS "ExecutionGraph: Write config file ${ExecutionGraph_CONFIG_FILE}, ${ExecutionGraph_OPENMP_NTHREADS}")
    ExecutionGraph_WRITE_CONFIG_FILE( ${ExecutionGraph_CONFIG_FILE} ${ExecutionGraph_ROOT_DIR})
    #=========================
    
    
    
ENDMACRO()
