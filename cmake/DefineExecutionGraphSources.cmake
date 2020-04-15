function(include_all_source_ExecutionGraph SRC INC INC_DIRS
         ExecutionGraph_ROOT_DIR ExecutionGraph_BINARY_DIR)

    # Write Config files
    include(${ExecutionGraph_ROOT_DIR}/cmake/WriteConfigFile.cmake)
    set(ExecutionGraph_CONFIG_FILE
        ${ExecutionGraph_BINARY_DIR}/include/executionGraph/config/Config.hpp)
    message(
        STATUS "ExecutionGraph: Write config file ${ExecutionGraph_CONFIG_FILE}"
    )
    executiongraph_write_config_file(${ExecutionGraph_CONFIG_FILE}
                                     ${ExecutionGraph_ROOT_DIR})

    # Add all external sources/headers include(${ExecutionGraph_ROOT_DIR}/cmake/
    # DefineExecutionGraphExternalSources.cmake) no external sources up to now

    set(SOURCES
        ${ExecutionGraph_ROOT_DIR}/src/DemangleTypes.cpp
        ${ExecutionGraph_ROOT_DIR}/src/LogicSocket.cpp
        ${ExecutionGraph_ROOT_DIR}/src/LogicSocketBase.cpp
        ${ExecutionGraph_ROOT_DIR}/src/LogicSocketData.cpp
        # ${ExecutionGraph_ROOT_DIR}/src/LogicNode.cpp
        ${ExecutionGraph_ROOT_DIR}/src/FileSystem.cpp
        ${ExecutionGraph_ROOT_DIR}/src/FileMapper.cpp)

    set(INCLUDES
        ${ExecutionGraph_ROOT_DIR}/include/executionGraph/common/AccessMacros.hpp
        ${ExecutionGraph_ROOT_DIR}/include/executionGraph/common/Assert.hpp
        ${ExecutionGraph_ROOT_DIR}/include/executionGraph/common/BinaryBufferView.hpp
        ${ExecutionGraph_ROOT_DIR}/include/executionGraph/common/Deferred.hpp
        ${ExecutionGraph_ROOT_DIR}/include/executionGraph/common/Delegates.hpp
        ${ExecutionGraph_ROOT_DIR}/include/executionGraph/common/DemangleTypes.hpp
        ${ExecutionGraph_ROOT_DIR}/include/executionGraph/common/EnumClassHelper.hpp
        ${ExecutionGraph_ROOT_DIR}/include/executionGraph/common/Exception.hpp
        ${ExecutionGraph_ROOT_DIR}/include/executionGraph/common/Factory.hpp
        ${ExecutionGraph_ROOT_DIR}/include/executionGraph/common/FileSystem.hpp
        ${ExecutionGraph_ROOT_DIR}/include/executionGraph/common/Identifier.hpp
        ${ExecutionGraph_ROOT_DIR}/include/executionGraph/common/ITask.hpp
        ${ExecutionGraph_ROOT_DIR}/include/executionGraph/common/Log.hpp
        ${ExecutionGraph_ROOT_DIR}/include/executionGraph/common/MetaVisit.hpp
        ${ExecutionGraph_ROOT_DIR}/include/executionGraph/common/Platform.hpp
        ${ExecutionGraph_ROOT_DIR}/include/executionGraph/common/SfinaeMacros.hpp
        ${ExecutionGraph_ROOT_DIR}/include/executionGraph/common/Singleton.hpp
        ${ExecutionGraph_ROOT_DIR}/include/executionGraph/common/StringFormat.hpp
        ${ExecutionGraph_ROOT_DIR}/include/executionGraph/common/Synchronized.hpp
        ${ExecutionGraph_ROOT_DIR}/include/executionGraph/common/TaskConsumer.hpp
        ${ExecutionGraph_ROOT_DIR}/include/executionGraph/common/TaskQueue.hpp
        ${ExecutionGraph_ROOT_DIR}/include/executionGraph/common/ThreadPool.hpp
        ${ExecutionGraph_ROOT_DIR}/include/executionGraph/common/TypeDefs.hpp
        # ${ExecutionGraph_ROOT_DIR}/include/executionGraph/graphs/CycleDescript
        # ion.hpp ${ExecutionGraph_ROOT_DIR}/include/executionGraph/graphs/Execu
        # tionTree.hpp
        ${ExecutionGraph_ROOT_DIR}/include/executionGraph/nodes/LogicCommon.hpp
        # ${ExecutionGraph_ROOT_DIR}/include/executionGraph/nodes/LogicNodeDefau
        # ltPool.hpp
        # ${ExecutionGraph_ROOT_DIR}/include/executionGraph/nodes/LogicNode.hpp
        # ${ExecutionGraph_ROOT_DIR}/include/executionGraph/nodes/LogicSocketDef
        # aultTypes.hpp
        ${ExecutionGraph_ROOT_DIR}/include/executionGraph/nodes/LogicSocket.hpp
        # ${ExecutionGraph_ROOT_DIR}/include/executionGraph/nodes/SocketLinkDesc
        # ription.hpp ${ExecutionGraph_ROOT_DIR}/include/executionGraph/serializ
        # ation/Conversions.hpp ${ExecutionGraph_ROOT_DIR}/include/executionGrap
        # h/serialization/ExecutionGraphSerializer.hpp ${ExecutionGraph_ROOT_DIR
        # }/include/executionGraph/serialization/FileMapper.hpp ${ExecutionGraph
        # _ROOT_DIR}/include/executionGraph/serialization/GraphTypeDescription.h
        # pp ${ExecutionGraph_ROOT_DIR}/include/executionGraph/serialization/Gra
        # phTypeDescriptionSerializer.hpp ${ExecutionGraph_ROOT_DIR}/include/exe
        # cutionGraph/serialization/LogicNodeSerializer.hpp ${ExecutionGraph_ROO
        # T_DIR}/include/executionGraph/serialization/NodeTypeDescription.hpp ${
        # ExecutionGraph_ROOT_DIR}/include/executionGraph/serialization/SocketTy
        # peDescription.hpp
        ${ExecutionGraph_CONFIG_FILE})

    set(INCLUDE_DIRS $<BUILD_INTERFACE:${ExecutionGraph_ROOT_DIR}/include>
                     $<BUILD_INTERFACE:${ExecutionGraph_BINARY_DIR}/include>)

    set(${SRC}
        ${SOURCES}
        PARENT_SCOPE)
    set(${INC}
        ${INCLUDES}
        PARENT_SCOPE)
    set(${INC_DIRS}
        ${INCLUDE_DIRS}
        PARENT_SCOPE)

    include(InstallMacros)
    foreach(file ${INCLUDES})
        getincludeinstallfolderpostfix(${file} postfix)
        if("${postfix}" STREQUAL "")
            message(FATAL_ERROR "wrong path ${PATH}")
        endif()
        install(FILES ${file} DESTINATION "include/${postfix}")
    endforeach()

endfunction()

function(include_all_source_ExecutionGraphSerialization SRC INC INC_DIRS
         ExecutionGraph_ROOT_DIR ExecutionGraph_BINARY_DIR)

    set(SOURCES
        ${ExecutionGraph_ROOT_DIR}/src/GraphTypeDescriptionSerializer.cpp
        ${ExecutionGraph_ROOT_DIR}/src/Conversions.cpp)

    set(INCLUDES
        ${ExecutionGraph_ROOT_DIR}/include/executionGraph/serialization/ExecutionGraphSerializer.hpp
        ${ExecutionGraph_ROOT_DIR}/include/executionGraph/serialization/GraphTypeDescriptionSerializer.hpp
        ${ExecutionGraph_ROOT_DIR}/include/executionGraph/serialization/NodeTypeDescription.hpp
        ${ExecutionGraph_ROOT_DIR}/include/executionGraph/serialization/GraphTypeDescription.hpp
        ${ExecutionGraph_ROOT_DIR}/include/executionGraph/serialization/SocketTypeDescription.hpp
        ${ExecutionGraph_ROOT_DIR}/include/executionGraph/serialization/LogicNodeSerializer.hpp
        ${ExecutionGraph_ROOT_DIR}/include/executionGraph/serialization/FileMapper.hpp
        ${ExecutionGraph_ROOT_DIR}/include/executionGraph/serialization/Conversions.hpp
    )

    set(SCHEMAS
        ${ExecutionGraph_ROOT_DIR}/include/executionGraph/serialization/schemas/LogicNode.fbs
        ${ExecutionGraph_ROOT_DIR}/include/executionGraph/serialization/schemas/LogicSocket.fbs
        ${ExecutionGraph_ROOT_DIR}/include/executionGraph/serialization/schemas/DataTypes.fbs
        ${ExecutionGraph_ROOT_DIR}/include/executionGraph/serialization/schemas/GraphTypeDescription.fbs
        ${ExecutionGraph_ROOT_DIR}/include/executionGraph/serialization/schemas/ExecutionGraph.fbs
        ${ExecutionGraph_ROOT_DIR}/include/executionGraph/serialization/schemas/SocketTypeDescription.fbs
        ${ExecutionGraph_ROOT_DIR}/include/executionGraph/serialization/schemas/SocketLinkDescription.fbs
        ${ExecutionGraph_ROOT_DIR}/include/executionGraph/serialization/schemas/NodeTypeDescription.fbs
        ${ExecutionGraph_ROOT_DIR}/include/executionGraph/serialization/schemas/cpp/LogicNode_generated.h
        ${ExecutionGraph_ROOT_DIR}/include/executionGraph/serialization/schemas/cpp/DataTypes_generated.h
        ${ExecutionGraph_ROOT_DIR}/include/executionGraph/serialization/schemas/cpp/LogicSocket_generated.h
        ${ExecutionGraph_ROOT_DIR}/include/executionGraph/serialization/schemas/cpp/GraphTypeDescription_generated.h
        ${ExecutionGraph_ROOT_DIR}/include/executionGraph/serialization/schemas/cpp/ExecutionGraph_generated.h
        ${ExecutionGraph_ROOT_DIR}/include/executionGraph/serialization/schemas/cpp/SocketLinkDescription_generated.h
        ${ExecutionGraph_ROOT_DIR}/include/executionGraph/serialization/schemas/cpp/SocketLink_generated.h
        ${ExecutionGraph_ROOT_DIR}/include/executionGraph/serialization/schemas/cpp/NodeTypeDescription_generated.h
        ${ExecutionGraph_ROOT_DIR}/include/executionGraph/serialization/schemas/cpp/SocketTypeDescription_generated.h
    )

    set(INCLUDE_DIRS $<BUILD_INTERFACE:${ExecutionGraph_ROOT_DIR}/include>
                     $<BUILD_INTERFACE:${ExecutionGraph_BINARY_DIR}/include>)

    set(${SRC}
        ${SOURCES}
        PARENT_SCOPE)
    set(${INC}
        ${INCLUDES}
        PARENT_SCOPE)
    set(${INC_DIRS}
        ${INCLUDE_DIRS}
        PARENT_SCOPE)

    include(InstallMacros)
    foreach(file ${INCLUDES})
        getincludeinstallfolderpostfix(${file} postfix)
        if("${postfix}" STREQUAL "")
            message(FATAL_ERROR "wrong path ${PATH}")
        endif()
        install(FILES ${file} DESTINATION "include/${postfix}")

    endforeach()

endfunction()

function(setTargetCompileOptionsExecutionGraph target use_address_san
         use_leak_san)

    if(CMAKE_CXX_COMPILER_ID MATCHES "Clang|AppleClang")
        message(STATUS "Setting Compile/Linker Options for Clang")
        list(
            APPEND
            CXX_FLAGS
            "-fno-omit-frame-pointer"
            "-Wall"
            "-Werror"
            "-Wpedantic"
            "-Wno-documentation"
            "-Wno-unused-local-typedef"
            "-ftemplate-backtrace-limit=0")

    elseif(CMAKE_CXX_COMPILER_ID STREQUAL "GNU")
        list(
            APPEND
            CXX_FLAGS
            "-fno-omit-frame-pointer"
            "-Wall"
            "-Werror"
            "-Wpedantic"
            "-Wno-unused-local-typedef"
            "-Wno-documentation"
            "-Wno-unused-variable")
    elseif(CMAKE_CXX_COMPILER_ID STREQUAL "MSVC")
        message(ERROR "MSVC is not yet supported!")
    else()
        message(ERROR
                "Compiler '${CMAKE_CXX_COMPILER_ID}' is not yet supported!")
    endif()

    if(${use_address_san})
        if(CMAKE_CXX_COMPILER_ID MATCHES "Clang|GNU")
            # with clang 5.0.1: -fsanitize=address produces weird output in lldb
            # for std::string ...
            list(APPEND CXX_FLAGS_DEBUG "-fsanitize=address")
            set(LINKER_FLAGS "${LINKER_FLAGS} -fsanitize=address")
        elseif(CMAKE_CXX_COMPILER_ID STREQUAL "AppleClang")
            list(APPEND CXX_FLAGS_DEBUG "-fsanitize=address")
            set(LINKER_FLAGS "${LINKER_FLAGS} -fsanitize=address")
        elseif(CMAKE_CXX_COMPILER_ID STREQUAL "MSVC")
            message(FATAL_ERROR "MSVC is not yet supported!")
        else()
            message(ERROR
                    "Compiler '${CMAKE_CXX_COMPILER_ID}' is not yet supported!")
        endif()
    endif()

    if(${use_leak_san})
        if(CMAKE_CXX_COMPILER_ID MATCHES "Clang|GNU")
            list(APPEND CXX_FLAGS_DEBUG "-fsanitize=leak")
            set(LINKER_FLAGS "${LINKER_FLAGS} -fsanitize=leak")
        elseif(CMAKE_CXX_COMPILER_ID STREQUAL "AppleClang")
            message(
                FATAL_ERROR
                    "AppleClang does not support -fsanitize=leak (please check)"
            )
        elseif(CMAKE_CXX_COMPILER_ID STREQUAL "MSVC")
            message(FATAL_ERROR "MSVC is not yet supported!")
        else()
            message(ERROR
                    "Compiler '${CMAKE_CXX_COMPILER_ID}' is not yet supported!")
        endif()
    endif()

    target_compile_features(${target} PUBLIC cxx_std_20)

    # Compile flags.
    target_compile_options(
        ${target} PRIVATE ${CXX_FLAGS} $<$<CONFIG:Debug>:${CXX_FLAGS_DEBUG}>)

    # Linker flags.
    set_property(TARGET ${target} PROPERTY LINK_FLAGS ${LINKER_FLAGS})

    # Linking std-libraries target_link_libraries(${target}  PUBLIC "c++fs"
    # PUBLIC "c++experimental")

    if(OS_MACOSX)
        set_target_properties(
            ${target}
            PROPERTIES OSX_ARCHITECTURES_DEBUG "${CMAKE_OSX_ARCHITECTURES}"
                       OSX_ARCHITECTURES_RELEASE "${CMAKE_OSX_ARCHITECTURES}")
    endif()

endfunction()
