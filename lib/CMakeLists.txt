# Options ==============================================================
mark_as_advanced( ExecutionGraph_BUILD_LIBRARY_STATIC  )
set(ExecutionGraph_BUILD_LIBRARY_STATIC OFF CACHE BOOL "Build a static library")

################################
# Make Target: Library 
################################

macro(add_ExecutionGraph_lib STATICorSHARED TARGETNAME EXPORTNAME DOINSTALL)
    # Add the real library
    add_library(${TARGETNAME} ${STATICorSHARED} ${ExecutionGraph_SRC} ${ExecutionGraph_INC})
    setTargetCompileOptionsExecutionGraph(${TARGETNAME} ${ExecutionGraph_USE_ADDRESS_SANITIZER} ${ExecutionGraph_USE_LEAK_SANITIZER})

    target_include_directories(${TARGETNAME} PUBLIC ${ExecutionGraph_INC_DIRS} $<INSTALL_INTERFACE:include>)
    target_link_libraries(${TARGETNAME} PUBLIC ExecutionGraph::Core-Dependencies)
    add_library("ExecutionGraph::${EXPORTNAME}" ALIAS ${TARGETNAME})

    # Add library for headers only
    add_library("${TARGETNAME}-Headers" INTERFACE)
    add_library("ExecutionGraph::${EXPORTNAME}-Headers" ALIAS "${TARGETNAME}-Headers")
    target_include_directories("${TARGETNAME}-Headers" INTERFACE ${ExecutionGraph_INC_DIRS} $<INSTALL_INTERFACE:include>)


    set_target_properties(${TARGETNAME} 
        PROPERTIES
        VERSION ${ExecutionGraph_VERSION_STRING} SOVERSION ${ExecutionGraph_VERSION_STRING}
        DEBUG_POSTFIX ${CMAKE_DEBUG_POSTFIX}
        EXPORT_NAME ${EXPORTNAME})


    # set build flag, for import/export specifiers in platform.hpp
    # Building a library means we only export the symbols which have ExecutionGraph_EXPORT set in the Platform.hpp file
    # We define all other symbols to be hidden by default as suggested by https://gcc.gnu.org/wiki/Visibility
    if( ${CMAKE_CXX_COMPILER_ID} STREQUAL "GNU" OR 
        ${CMAKE_CXX_COMPILER_ID} STREQUAL "Clang" OR 
        ${CMAKE_CXX_COMPILER_ID} STREQUAL "AppleClang")
        target_compile_options(${TARGETNAME} PRIVATE -DExecutionGraph_BUILD_LIBRARY -fvisibility=hidden -fPIC)
    elseif(${CMAKE_CXX_COMPILER_ID} STREQUAL "MVCC")
        target_compile_options(${TARGETNAME} PRIVATE /DExecutionGraph_BUILD_LIBRARY)
    else()
        message(FATAL_ERROR "Could not set compile options for compiler id: '${CMAKE_CXX_COMPILER_ID}'")
    endif()

    if(${DOINSTALL})
        install(TARGETS ${TARGETNAME} EXPORT ExecutionGraphTargets
                RUNTIME       DESTINATION ${ExecutionGraph_RUNTIME_INSTALL_DIR}
                LIBRARY       DESTINATION ${ExecutionGraph_LIBRARY_INSTALL_DIR}
                ARCHIVE       DESTINATION ${ExecutionGraph_ARCHIVE_INSTALL_DIR}
                FRAMEWORK     DESTINATION ${ExecutionGraph_FRAMEWORK_INSTALL_DIR})
    endif()

endmacro()

message(STATUS "====================================================")
message(STATUS "Configuring target: ExecutionGraph::Core")
add_ExecutionGraph_lib("SHARED" "ExecutionGraph-Core" "Core" ON)
print_target_properties(ExecutionGraph::Core)

if(ExecutionGraph_BUILD_LIBRARY_STATIC)
    message(STATUS "====================================================")
    message(STATUS "Configuring target: ExecutionGraph::CoreS")
    add_ExecutionGraph_lib("STATIC" "ExecutionGraph-CoreS" "CoreS" ON)
    print_target_properties(ExecutionGraph::CoreS)
endif()

################################
# Make Target: Library for tests
################################

if(ExecutionGraph_BUILD_TESTS)
    
    message(STATUS "====================================================")
    message(STATUS "Configuring target: ExecutionGraph::CoreForTests")
    
    add_ExecutionGraph_lib("SHARED" "ExecutionGraph-CoreForTests" "CoreForTests" OFF)

    # set build flag, for import/export specifiers in platform.hpp
    # Building a library means we only export the symbols which have ExecutionGraph_EXPORT set in the Platform.hpp file
    # We define all other symbols to be hidden by default as suggested by https://gcc.gnu.org/wiki/Visibility
    if( ${CMAKE_CXX_COMPILER_ID} STREQUAL "GNU" OR 
        ${CMAKE_CXX_COMPILER_ID} STREQUAL "Clang" OR 
        ${CMAKE_CXX_COMPILER_ID} STREQUAL "AppleClang")
        target_compile_options(ExecutionGraph-CoreForTests PRIVATE -DExecutionGraph_BUILD_TESTS)
    elseif(${CMAKE_CXX_COMPILER_ID} STREQUAL "MVCC")
        target_compile_options(ExecutionGraph-CoreForTests PRIVATE /DExecutionGraph_BUILD_TESTS)
    else()
        message(FATAL_ERROR "Could not set compile options for compiler id: '${CMAKE_CXX_COMPILER_ID}'")
    endif()

    print_target_properties(ExecutionGraph::CoreForTests)
endif()

# Configuring Installstep
#########################

# Make cmake config files
install(EXPORT  ExecutionGraphTargets
                DESTINATION ${ExecutionGraph_CMAKE_CONFIG_INSTALL_DIR}
                NAMESPACE ExecutionGraph::
                FILE executiongraph-config.cmake)

# Write/install version file
include(CMakePackageConfigHelpers)
write_basic_package_version_file("${CMAKE_CURRENT_BINARY_DIR}/cmake/executiongraph-config-version.cmake"
                                 VERSION ${ExecutionGraph_VERSION_STRING}
                                 COMPATIBILITY AnyNewerVersion)

install(FILES "${CMAKE_CURRENT_BINARY_DIR}/cmake/executiongraph-config-version.cmake"
        DESTINATION ${ExecutionGraph_CMAKE_CONFIG_INSTALL_DIR}
        COMPONENT Devel)

# Install readme and license
install(FILES "${ExecutionGraph_LICENSE_FILE}" "${ExecutionGraph_README_FILE}"
        DESTINATION ${ExecutionGraph_ADDITIONAL_FILES_INSTALL_DIR}
        PERMISSIONS OWNER_READ)

