function(include_all_source_ExecutionGraph 
        SRC 
        INC 
        INCLUDE_DIRS 
        DEPENDING_TARGETS # Input variable names
        ExecutionGraph_ROOT_DIR 
        ExecutionGraph_BINARY_DIR)

    # Write Config files
    include(${ExecutionGraph_ROOT_DIR}/cmake/WriteConfigFile.cmake)
    set(ExecutionGraph_CONFIG_FILE ${ExecutionGraph_BINARY_DIR}/include/executionGraph/config/Config.hpp)
    message(STATUS "ExecutionGraph: Write config file ${ExecutionGraph_CONFIG_FILE}")
    ExecutionGraph_write_config_file( ${ExecutionGraph_CONFIG_FILE} ${ExecutionGraph_ROOT_DIR})

    # Add all external sources/headers
    # include(${ExecutionGraph_ROOT_DIR}/cmake/DefineExecutionGraphExternalSources.cmake)
    # no external sources up to now
    
    set(${SRC}
        ${ExecutionGraph_ROOT_DIR}/src/DemangleTypes.cpp
        ${ExecutionGraph_ROOT_DIR}/src/CommandLineArguments.cpp

        ${ExecutionGraph_ROOT_DIR}/src/LogicSocket.cpp
        ${ExecutionGraph_ROOT_DIR}/src/LogicNode.cpp

        ${ExecutionGraph_ROOT_DIR}/src/FileSystem.cpp

        ${ExecutionGraph_ROOT_DIR}/src/FileMapper.cpp

        # Serialization
        ${ExecutionGraph_ROOT_DIR}/src/GraphTypeDescriptionSerializer.cpp

        PARENT_SCOPE
    )

    set(${INC}
        ${ExecutionGraph_ROOT_DIR}/include/executionGraph/common/Log.hpp
        ${ExecutionGraph_ROOT_DIR}/include/executionGraph/common/Assert.hpp
        ${ExecutionGraph_ROOT_DIR}/include/executionGraph/common/Delegates.hpp
        ${ExecutionGraph_ROOT_DIR}/include/executionGraph/common/DemangleTypes.hpp
        ${ExecutionGraph_ROOT_DIR}/include/executionGraph/common/EnumClassHelper.hpp
        ${ExecutionGraph_ROOT_DIR}/include/executionGraph/common/Exception.hpp
        ${ExecutionGraph_ROOT_DIR}/include/executionGraph/common/MyContainerTypeDefs.hpp
        ${ExecutionGraph_ROOT_DIR}/include/executionGraph/common/MyMatrixTypeDefs.hpp
        ${ExecutionGraph_ROOT_DIR}/include/executionGraph/common/Platform.hpp
        ${ExecutionGraph_ROOT_DIR}/include/executionGraph/common/SfinaeMacros.hpp
        ${ExecutionGraph_ROOT_DIR}/include/executionGraph/common/TypeDefs.hpp
        ${ExecutionGraph_ROOT_DIR}/include/executionGraph/common/CommandLineArguments.hpp
        ${ExecutionGraph_ROOT_DIR}/include/executionGraph/common/Identifier.hpp
        ${ExecutionGraph_ROOT_DIR}/include/executionGraph/common/IObjectID.hpp
        ${ExecutionGraph_ROOT_DIR}/include/executionGraph/common/Factory.hpp
        ${ExecutionGraph_ROOT_DIR}/include/executionGraph/common/FileSystem.hpp

        ${ExecutionGraph_ROOT_DIR}/include/executionGraph/nodes/LogicCommon.hpp
        ${ExecutionGraph_ROOT_DIR}/include/executionGraph/nodes/LogicSocket.hpp
        ${ExecutionGraph_ROOT_DIR}/include/executionGraph/nodes/LogicSocketDefaultTypes.hpp
        ${ExecutionGraph_ROOT_DIR}/include/executionGraph/nodes/LogicNode.hpp
        
        ${ExecutionGraph_ROOT_DIR}/include/executionGraph/graphs/ExecutionTree.hpp

        ${ExecutionGraph_ROOT_DIR}/include/executionGraph/serialization/ExecutionGraphSerializer.hpp
        ${ExecutionGraph_ROOT_DIR}/include/executionGraph/serialization/FileMapper.hpp

        ${ExecutionGraph_CONFIG_FILE}
        PARENT_SCOPE
    )

    set(${INCLUDE_DIRS}
        $<BUILD_INTERFACE:${ExecutionGraph_ROOT_DIR}/include>
        $<BUILD_INTERFACE:${ExecutionGraph_BINARY_DIR}/include>
        PARENT_SCOPE
    )
    
    include(InstallMacros)
    foreach(file ${${INC}})
        getIncludeInstallFolderPostfix(${file} postfix )
        if("${postfix}" STREQUAL "")
            message(FATAL_ERROR "wrong path ${PATH}")
        endif()
        install(FILES ${file} DESTINATION "include/${postfix}" )
    endforeach()   

endfunction()


function(setTargetCompileOptionsExecutionGraph target use_address_san use_leak_san)

    if(CMAKE_CXX_COMPILER_ID STREQUAL "Clang" OR
       CMAKE_CXX_COMPILER_ID STREQUAL "AppleClang")
        message(STATUS "Setting Compile/Linker Options for Clang")
        list(APPEND CXX_FLAGS       "-fno-omit-frame-pointer"
                                    "-Wall"
                                    "-Werror"
                                    "-Wpedantic"
                                    "-Wno-documentation"
                                    "-Wno-unused-local-typedef"
                                    "-ftemplate-backtrace-limit=0")

    elseif(CMAKE_CXX_COMPILER_ID STREQUAL "GNU")
        list(APPEND CXX_FLAGS       "-fno-omit-frame-pointer"
                                    "-Wall"
                                    "-Werror"
                                    "-Wpedantic"
                                    "-Wno-unused-local-typedef"
                                    "-Wno-documentation")
    elseif(${CMAKE_CXX_COMPILER_ID} STREQUAL "MSVC")
        message(ERROR "MSVC is not yet supported!")
    endif()


    if(${use_address_san})
        if(CMAKE_CXX_COMPILER_ID STREQUAL "Clang" OR CMAKE_CXX_COMPILER_ID STREQUAL "GNU")
            # with clang 5.0.1: -fsanitize=address produces weird output in lldb for std::string ...
            list(APPEND CXX_FLAGS_DEBUG  "-fsanitize=address")
            set(LINKER_FLAGS "${LINKER_FLAGS} -fsanitize=address")
        elseif(CMAKE_CXX_COMPILER_ID STREQUAL "AppleClang")
            list(APPEND CXX_FLAGS_DEBUG "-fsanitize=address")
            set(LINKER_FLAGS "${LINKER_FLAGS} -fsanitize=address")
        elseif(${CMAKE_CXX_COMPILER_ID} STREQUAL "MSVC")
            message(FATAL_ERROR "MSVC is not yet supported!")
        endif()
    endif()

    if(${use_leak_san})
        if(CMAKE_CXX_COMPILER_ID STREQUAL "Clang" OR CMAKE_CXX_COMPILER_ID STREQUAL "GNU")
            list(APPEND CXX_FLAGS_DEBUG  "-fsanitize=leak")
            set(LINKER_FLAGS "${LINKER_FLAGS} -fsanitize=leak")
        elseif(CMAKE_CXX_COMPILER_ID STREQUAL "AppleClang")
            message(FATAL_ERROR "AppleClang does not support -fsanitize=leak (please check)")
        elseif(${CMAKE_CXX_COMPILER_ID} STREQUAL "MSVC")
            message(FATAL_ERROR "MSVC is not yet supported!")
        endif()
    endif()

    target_compile_features(${target} PUBLIC cxx_std_17)

    # Compile flags.
    target_compile_options(${target} PRIVATE ${CXX_FLAGS} $<$<CONFIG:Debug>:${CXX_FLAGS_DEBUG}>)

    # Linker flags.
    set_property(TARGET ${target} PROPERTY LINK_FLAGS ${LINKER_FLAGS})

    # Linking std-libraries
    target_link_libraries(${target}  PUBLIC "stdc++fs" PUBLIC "c++experimental")

    if(OS_MACOSX)
        set_target_properties(${target} PROPERTIES
            OSX_ARCHITECTURES_DEBUG      "${CMAKE_OSX_ARCHITECTURES}"
            OSX_ARCHITECTURES_RELEASE    "${CMAKE_OSX_ARCHITECTURES}"
        )
    endif()

endfunction()