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


macro(set_target_compile_options_ExecutionGraph target)

	if(${CMAKE_CXX_COMPILER_ID} STREQUAL "GNU")

		set(MYPROJECT_DONTSET_COMPILER_FLAGS_INTERNAL TRUE CACHE INTERNAL "x" FORCE)
		message(STATUS "Setting Compile/Linker Options for GNU")
		set(CXX_FLAGS         "${CMAKE_CXX_FLAGS} -std=c++17 -fmax-errors=50 -Werror=return-type" CACHE STRING "Flags for CXX Compiler" FORCE)
		set(CXX_FLAGS_DEBUG   "${CMAKE_CXX_FLAGS_DEBUG} -g -fno-omit-frame-pointer -fsanitize=leak -Wall -Wpedantic -Wno-char-subscripts" CACHE STRING "Flags for CXX Compiler for debug builds" FORCE)

	elseif( ${CMAKE_CXX_COMPILER_ID} STREQUAL "Clang" )

        message(STATUS "Setting Compile/Linker Options for Clang")
        set(MYPROJECT_DONTSET_COMPILER_FLAGS_INTERNAL TRUE CACHE INTERNAL "x" FORCE)
        
		set(CXX_FLAGS                "${CMAKE_CXX_FLAGS} -std=c++17 -lc++experimental -ferror-limit=50 -Werror=return-type " CACHE STRING "Flags for CXX Compiler" FORCE)
		set(CXX_FLAGS_DEBUG          "${CMAKE_CXX_FLAGS_DEBUG} -g3 -fno-omit-frame-pointer -fsanitize=leak -fsanitize=address -Weverything -Wpedantic -Wno-deprecated-register -Wno-documentation -Wno-old-style-cast -Wno-comment -Wno-float-equal -Wno-deprecated -Wno-c++98-compat-pedantic -Wno-undef -Wno-unused-macros" CACHE STRING "Flags for CXX Compiler for debug builds" FORCE)
		set(CXX_FLAGS_MINSIZEREL     "${CMAKE_CXX_FLAGS_MINSIZEREL} -Os -DNDEBUG" CACHE STRING "Flags for CXX Compiler for release minsize builds" FORCE)
		set(CXX_FLAGS_RELEASE        "${CMAKE_CXX_FLAGS_RELEASE} -O3 -DNDEBUG" CACHE STRING "Flags for CXX Compiler for release builds" FORCE)
		set(CXX_FLAGS_RELWITHDEBINFO "${CMAKE_CXX_FLAGS_RELWITHDEBINFO} -O2 -g3" CACHE STRING "Flags for CXX Compiler for release builds with debug info" FORCE)

        set(LINKER_FLAGS    "${CMAKE_EXE_LINKER_FLAGS} -lc++experimental")

    elseif ( ${CMAKE_CXX_COMPILER_ID} STREQUAL "MSVC" )
        message(ERROR "MSVC is not yet supported!")
    endif()

    # Compile flags.
    target_compile_options(${target} PRIVATE ${CXX_FLAGS})
    target_compile_options(${target} PRIVATE $<$<CONFIG:Debug>:${CXX_FLAGS_DEBUG}>)
    target_compile_options(${target} PRIVATE $<$<CONFIG:Release>:${CXX_FLAGS_RELEASE}>)
    target_compile_options(${target} PRIVATE $<$<CONFIG:MinSizeRel>:${CXX_FLAGS_MINSIZEREL}>)
    target_compile_options(${target} PRIVATE $<$<CONFIG:RelWithDebInfo>:${CXX_FLAGS_RELWITHDEBINFO}>)

    # Linker flags.
    set_property(TARGET ${target} PROPERTY LINK_FLAGS ${LINKER_FLAGS})

endmacro()