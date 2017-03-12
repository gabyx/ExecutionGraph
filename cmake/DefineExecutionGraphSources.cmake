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
        ${ExecutionGraph_ROOT_DIR}/src/LogicSocket.cpp
        ${ExecutionGraph_ROOT_DIR}/src/LogicNode.cpp
    )

    SET(${INC}$
        
        ${ExecutionGraph_ROOT_DIR}/include/ExecutionGraph/common/Asserts.hpp
        ${ExecutionGraph_ROOT_DIR}/include/ExecutionGraph/common/Delegates.hpp
        ${ExecutionGraph_ROOT_DIR}/include/ExecutionGraph/common/EnumClassHelper.hpp
        ${ExecutionGraph_ROOT_DIR}/include/ExecutionGraph/common/Exception.hpp
        ${ExecutionGraph_ROOT_DIR}/include/ExecutionGraph/common/MyContainerTypeDefs.hpp
        ${ExecutionGraph_ROOT_DIR}/include/ExecutionGraph/common/MyMatrixTypeDefs.hpp
        ${ExecutionGraph_ROOT_DIR}/include/ExecutionGraph/common/Platform.hpp
        ${ExecutionGraph_ROOT_DIR}/include/ExecutionGraph/common/SfinaeMacros.hpp
        ${ExecutionGraph_ROOT_DIR}/include/ExecutionGraph/common/StaticAssert.hpp
        ${ExecutionGraph_ROOT_DIR}/include/ExecutionGraph/common/TypeDefs.hpp
    
        ${ExecutionGraph_ROOT_DIR}/include/ExecutionGraph/nodes/LogicCommon.hpp
        ${ExecutionGraph_ROOT_DIR}/include/ExecutionGraph/nodes/LogicSocket.hpp
        ${ExecutionGraph_ROOT_DIR}/include/ExecutionGraph/nodes/LogicSocketDefaultTypes.hpp
        ${ExecutionGraph_ROOT_DIR}/include/ExecutionGraph/nodes/LogicNode.hpp
    )

    SET(${INCLUDE_DIRS}
        ${ExecutionGraph_ROOT_DIR}/include
        ${ExecutionGraph_BINARY_DIR}/include
    )


    # WRITE CONFIGURATION FILE
    
    INCLUDE(${ExecutionGraph_ROOT_DIR}/cmake/WriteConfigFile.cmake)
    SET(ExecutionGraph_CONFIG_FILE ${ExecutionGraph_BINARY_DIR}/include/ExecutionGraph/config/Config.hpp)
    MESSAGE(STATUS "ExecutionGraph: Write config file ${ExecutionGraph_CONFIG_FILE}, ${ExecutionGraph_OPENMP_NTHREADS}")
    ExecutionGraph_WRITE_CONFIG_FILE( ${ExecutionGraph_CONFIG_FILE} ${ExecutionGraph_ROOT_DIR})
    #=========================
    
    
    
ENDMACRO()
