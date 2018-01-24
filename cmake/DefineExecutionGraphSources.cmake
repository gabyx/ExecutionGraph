macro(include_all_source_ExecutionGraph 
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
        ${ExecutionGraph_ROOT_DIR}/src/CommandLineArguments.cpp

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
        ${ExecutionGraph_ROOT_DIR}/include/ExecutionGraph/common/CommandLineArguments.hpp
        ${ExecutionGraph_ROOT_DIR}/include/ExecutionGraph/common/Identifier.hpp
        ${ExecutionGraph_ROOT_DIR}/include/ExecutionGraph/common/IObjectID.hpp
        ${ExecutionGraph_ROOT_DIR}/include/ExecutionGraph/common/Factory.hpp

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
    set(ExecutionGraph_CONFIG_FILE ${ExecutionGraph_BINARY_DIR}/include/executionGraph/config/Config.hpp)
    message(STATUS "ExecutionGraph: Write config file ${ExecutionGraph_CONFIG_FILE}")
    ExecutionGraph_write_config_file( ${ExecutionGraph_CONFIG_FILE} ${ExecutionGraph_ROOT_DIR})
    #=========================
    
endmacro()


macro(set_target_compile_options_ExecutionGraph target)

    if(${CMAKE_CXX_COMPILER_ID} STREQUAL "Clang" OR
       ${CMAKE_CXX_COMPILER_ID} STREQUAL "AppleClang")

        message(STATUS "Setting Compile/Linker Options for Clang")
        list(APPEND CXX_FLAGS "-std=c++17" 
                              "-lc++"
                              "-lc++experimental" 
                              "-ferror-limit=50" 
                              "-Werror=return-type")
        list(APPEND CXX_FLAGS_DEBUG "-g3"
                                    "-fno-omit-frame-pointer"
                                    "-Weverything"
                                    "-Wpedantic" 
                                    "-Wno-deprecated-register" 
                                    "-Wno-documentation" 
                                    "-Wno-old-style-cast" 
                                    "-Wno-comment" 
                                    "-Wno-float-equal" 
                                    "-Wno-deprecated" 
                                    "-Wno-c++98-compat-pedantic" 
                                    "-Wno-undef" 
                                    "-Wno-unused-macros")
        set(CXX_FLAGS_MINSIZEREL ${CMAKE_CXX_FLAGS_MINSIZEREL})
        list(APPEND CXX_FLAGS_MINSIZEREL "-Os" 
                                         "-DNDEBUG")
        set(CXX_FLAGS_RELEASE ${CMAKE_CXX_FLAGS_RELEASE})
        list(APPEND CXX_FLAGS_RELEASE "-O3" 
                                      "-DNDEBUG")
        set(CXX_FLAGS_RELWITHDEBINFO ${CMAKE_CXX_FLAGS_RELWITHDEBINFO})
        list(APPEND CXX_FLAGS_RELWITHDEBINFO "-O2"
                                             "-g3")

        set(LINKER_FLAGS "${CMAKE_EXE_LINKER_FLAGS} -lc++experimental")
    elseif(${CMAKE_CXX_COMPILER_ID} STREQUAL "GNU")
        message(STATUS "Setting Compile/Linker Options for GCC")
        list(APPEND CXX_FLAGS "-std=c++17" 
                            "-lc++experimental" 
                            "-lc++")
        set(CXX_FLAGS_DEBUG ${CMAKE_CXX_FLAGS_DEBUG})
        list(APPEND CXX_FLAGS_DEBUG "-g3"
                                    "-fno-omit-frame-pointer")
        set(CXX_FLAGS_MINSIZEREL ${CMAKE_CXX_FLAGS_MINSIZEREL})
        list(APPEND CXX_FLAGS_MINSIZEREL "-Os" 
                                        "-DNDEBUG")
        set(CXX_FLAGS_RELEASE ${CMAKE_CXX_FLAGS_RELEASE})
        list(APPEND CXX_FLAGS_RELEASE "-O3" 
                                    "-DNDEBUG")
        set(CXX_FLAGS_RELWITHDEBINFO ${CMAKE_CXX_FLAGS_RELWITHDEBINFO})
        list(APPEND CXX_FLAGS_RELWITHDEBINFO "-O2"
                                            "-g3")

        set(LINKER_FLAGS "${CMAKE_EXE_LINKER_FLAGS} -lc++experimental")
    elseif(${CMAKE_CXX_COMPILER_ID} STREQUAL "MSVC")
        message(ERROR "MSVC is not yet supported!")
    endif()


    if(${CMAKE_CXX_COMPILER_ID} STREQUAL "Clang" OR ${CMAKE_CXX_COMPILER_ID} STREQUAL "GNU")
        # with clang 5.0.1: -fsanitize=address produces weird output in lldb for std::string ...
        list(APPEND CXX_FLAGS_DEBUG "-fsanitize=leak" "-fsanitize=address" "-fno-omit-frame-pointer")
        set(LINKER_FLAGS "${LINKER_FLAGS} -fsanitize=leak -fsanitize=address")
    endif()

    
    # Compile flags.
    target_compile_options(${target} PRIVATE ${CXX_FLAGS} )
    target_compile_options(${target} PRIVATE $<$<CONFIG:Debug>:${CXX_FLAGS_DEBUG}> )
    target_compile_options(${target} PRIVATE $<$<CONFIG:Release>:${CXX_FLAGS_RELEASE}> )
    target_compile_options(${target} PRIVATE $<$<CONFIG:MinSizeRel>:${CXX_FLAGS_MINSIZEREL}> )
    target_compile_options(${target} PRIVATE $<$<CONFIG:RelWithDebInfo>:${CXX_FLAGS_RELWITHDEBINFO}> )

    # Linker flags.
    set_property(TARGET ${target} PROPERTY LINK_FLAGS ${LINKER_FLAGS})

endmacro()