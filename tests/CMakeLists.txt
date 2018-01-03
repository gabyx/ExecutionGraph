
# Add GoogleTests ===========================================================================
if (CMAKE_VERSION VERSION_LESS 3.2)
    set(UPDATE_DISCONNECTED_IF_AVAILABLE "")
else()
    set(UPDATE_DISCONNECTED_IF_AVAILABLE "UPDATE_DISCONNECTED 1")
endif()

include(DownloadProject)
download_project(PROJ                googletest
                 PREFIX              ${CMAKE_BINARY_DIR}/external
                 GIT_REPOSITORY      https://github.com/google/googletest.git
                 GIT_TAG             master
                 ${UPDATE_DISCONNECTED_IF_AVAILABLE}
)

# Prevent GoogleTest from overriding our compiler/linker options
# when building with Visual Studio
set(gtest_force_shared_crt ON CACHE BOOL "" FORCE)


add_subdirectory(${googletest_SOURCE_DIR} ${googletest_BINARY_DIR} EXCLUDE_FROM_ALL)

# When using CMake 2.8.11 or later, header path dependencies
# are automatically added to the gtest and gmock targets.
# For earlier CMake versions, we have to explicitly add the
# required directories to the header search path ourselves.
#if (CMAKE_VERSION VERSION_LESS 2.8.11)
    #include_directories("${gtest_SOURCE_DIR}/include"
                        #"${gmock_SOURCE_DIR}/include")
#endif()
# ==============================================================================================

set(ExecutionGraph_TESTS_OUTPUT_FILES_DIR "${PROJECT_BINARY_DIR}")
set(ExecutionGraph_TESTS_INPUT_FILES_DIR "${CMAKE_CURRENT_SOURCE_DIR}/files")
set(ExecutionGraph_TESTS_INPUT_FILES_ADD_DIR "${ExecutionGraph_ROOT_DIR}/additional/tests/files")
set(ExecutionGraph_TESTS_VALIDATION_FILES_DIR "${ExecutionGraph_ROOT_DIR}/additional/tests/files/validation")

# WRITE CONFIGURATION FILE
configure_file (
  "${CMAKE_CURRENT_SOURCE_DIR}/include/TestConfig.hpp.in.cmake"
  ${PROJECT_BINARY_DIR}/include/TestConfig.hpp
)
#=========================


set(SOURCE_FILES
    ${ExecutionGraph_SRC}
    ${CMAKE_CURRENT_SOURCE_DIR}/include/TestFunctions.hpp
)

set(INCLUDE_FILES
    ${ExecutionGraph_INC}
    ${CMAKE_CURRENT_SOURCE_DIR}/src/TestFunctions.cpp
)


#include directories
set(INCLUDE_DIRS
    ${ExecutionGraph_INC_DIRS}
    ${CMAKE_CURRENT_SOURCE_DIR}/include
    ${PROJECT_BINARY_DIR}/include

    ${googletest_SOURCE_DIR}/googletest/include 
    ${googletest_SOURCE_DIR}/googlemock/include 
)

# Define function to define compile definitions for all tests
function(defineCompileDefs target)

    message(STATUS "====================================================")
    message(STATUS "Configuring target: ${target}")

    target_include_directories(${target} PRIVATE ${INCLUDE_DIRS})
    target_link_libraries(${target} ExecutionGraph-LibForTestsOnly gtest gmock_main)

    if(${CMAKE_CXX_COMPILER_ID} STREQUAL "GNU" OR ${CMAKE_CXX_COMPILER_ID} STREQUAL "Clang" OR ${CMAKE_CXX_COMPILER_ID} STREQUAL "Intel")
        target_compile_options(${target} PRIVATE -DExecutionGraph_BUILD_TESTS )
    else()
        target_compile_options(${target} PRIVATE /DExecutionGraph_BUILD_TESTS )
    endif()

    cotire(${target})

    print_target_properties(${target})

    add_test(NAME ${target} COMMAND ${target})

endfunction()

# NodeTests
set(EXEC_NAME1 ${PROJECT_NAME}Test-LogicNodes)
add_executable(${EXEC_NAME1}  ${CMAKE_CURRENT_SOURCE_DIR}/src/main_LogicNode.cpp ${SOURCE_FILES} ${INCLUDE_FILES}   )
defineCompileDefs(${EXEC_NAME1})

#ExecutionTree Test
set(EXEC_NAME2 ${PROJECT_NAME}Test-ExecutionTree)
add_executable(${EXEC_NAME2}  ${CMAKE_CURRENT_SOURCE_DIR}/src/main_ExecutionTree.cpp ${SOURCE_FILES} ${INCLUDE_FILES}   )
defineCompileDefs(${EXEC_NAME2})

#ExecutionTree Test
set(EXEC_NAME3 ${PROJECT_NAME}Test-CustomSocketTypes)
add_executable(${EXEC_NAME3}  ${CMAKE_CURRENT_SOURCE_DIR}/src/main_CustomSocketTypes.cpp ${SOURCE_FILES} ${INCLUDE_FILES}   )
defineCompileDefs(${EXEC_NAME3})