macro(INCLUDE_ALL_ExecutionGraph_SOURCE 
      SRC 
      INC 
      INCLUDE_DIRS 
      DEPENDING_TARGETS # Input variable names
      ExecutionGraph_ROOT_DIR 
      ExecutionGraph_BINARY_DIR)  


    # Add all external sources/headers
    # include(${ExecutionGraph_ROOT_DIR}/cmake/DefineExecutionGraphExternalSources.cmake)
    # no external sources up to now
    
    set(${SRC}
                
        ${ExecutionGraph_ROOT_DIR}/src/DemangleTypes.cpp
    
        ${ExecutionGraph_ROOT_DIR}/src/LogicSocket.cpp
        ${ExecutionGraph_ROOT_DIR}/src/LogicNode.cpp
    )

    set(${INC}
        ${ExecutionGraph_ROOT_DIR}/include/ExecutionGraph/common/Asserts.hpp
        ${ExecutionGraph_ROOT_DIR}/include/ExecutionGraph/common/Delegates.hpp
        ${ExecutionGraph_ROOT_DIR}/include/ExecutionGraph/common/DemangleTypes.hpp
        ${ExecutionGraph_ROOT_DIR}/include/ExecutionGraph/common/EnumClassHelper.hpp
        ${ExecutionGraph_ROOT_DIR}/include/ExecutionGraph/common/Exception.hpp
        ${ExecutionGraph_ROOT_DIR}/include/ExecutionGraph/common/MyContainerTypeDefs.hpp
        ${ExecutionGraph_ROOT_DIR}/include/ExecutionGraph/common/MyMatrixTypeDefs.hpp
        ${ExecutionGraph_ROOT_DIR}/include/ExecutionGraph/common/Platform.hpp
        ${ExecutionGraph_ROOT_DIR}/include/ExecutionGraph/common/SfinaeMacros.hpp
        ${ExecutionGraph_ROOT_DIR}/include/ExecutionGraph/common/TypeDefs.hpp
    
        ${ExecutionGraph_ROOT_DIR}/include/ExecutionGraph/nodes/LogicCommon.hpp
        ${ExecutionGraph_ROOT_DIR}/include/ExecutionGraph/nodes/LogicSocket.hpp
        ${ExecutionGraph_ROOT_DIR}/include/ExecutionGraph/nodes/LogicSocketDefaultTypes.hpp
        ${ExecutionGraph_ROOT_DIR}/include/ExecutionGraph/nodes/LogicNode.hpp
        
        ${ExecutionGraph_ROOT_DIR}/include/ExecutionGraph/graphs/ExecutionTreeInOut.hpp
    )

    set(${INCLUDE_DIRS}
        ${ExecutionGraph_ROOT_DIR}/include
        ${ExecutionGraph_BINARY_DIR}/include
    )


    # WRITE CONFIGURATION FILE
    
    include(${ExecutionGraph_ROOT_DIR}/cmake/WriteConfigFile.cmake)
    set(ExecutionGraph_CONFIG_FILE ${ExecutionGraph_BINARY_DIR}/include/ExecutionGraph/config/Config.hpp)
    message(STATUS "ExecutionGraph: Write config file ${ExecutionGraph_CONFIG_FILE}, ${ExecutionGraph_OPENMP_NTHREADS}")
    ExecutionGraph_write_config_file( ${ExecutionGraph_CONFIG_FILE} ${ExecutionGraph_ROOT_DIR})
    #=========================
    
    
    
endmacro()
