
# Add GoogleTests ===========================================================================
if (CMAKE_VERSION VERSION_LESS 3.2)
    set(UPDATE_DISCONNECTED_IF_AVAILABLE "")
else()
    set(UPDATE_DISCONNECTED_IF_AVAILABLE "UPDATE_DISCONNECTED 1")
endif()

include(DownloadProject)
download_project(PROJ                googlebenchmark
                 GIT_REPOSITORY      https://github.com/google/benchmark.git
                 GIT_TAG             master
                 ${UPDATE_DISCONNECTED_IF_AVAILABLE}
)

add_subdirectory(${googlebenchmark_SOURCE_DIR} ${googlebenchmark_BINARY_DIR} EXCLUDE_FROM_ALL)

# ==============================================================================================


# WRITE CONFIGURATION FILE
configure_file (
  "${CMAKE_CURRENT_SOURCE_DIR}/include/BenchmarkConfig.hpp.in.cmake"
  ${PROJECT_BINARY_DIR}/include/BenchmarkConfig.hpp
)
#=========================


SET(SOURCE_FILES
    ${ExecutionGraph_SRC}   
    ${CMAKE_CURRENT_SOURCE_DIR}/include/BenchmarkFunctions.hpp
)

SET(INCLUDE_FILES
    ${ExecutionGraph_INC}
    ${CMAKE_CURRENT_SOURCE_DIR}/src/BenchmarkFunctions.cpp
)


#include directories
set(INCLUDE_DIRS
    ${ExecutionGraph_INC_DIRS}
    ${CMAKE_CURRENT_SOURCE_DIR}/include/
    ${PROJECT_BINARY_DIR}/include/
)

include_directories(${INCLUDE_DIRS})

# Define function to define compile definitions for all tests
function(defineCompileDefs Target)
    IF(${CMAKE_CXX_COMPILER_ID} STREQUAL "GNU" OR ${CMAKE_CXX_COMPILER_ID} STREQUAL "Clang" OR ${CMAKE_CXX_COMPILER_ID} STREQUAL "Intel")
        target_compile_options(${Target} PRIVATE -DExecutionGraph_BUILD_TESTS )
    ELSE()
        target_compile_options(${Target} PRIVATE /DExecutionGraph_BUILD_TESTS )
    ENDIF()
endfunction()

# Delegates
SET(EXEC_NAME1 ${PROJECT_NAME}-BenchmarkDelegates)
ADD_EXECUTABLE(${EXEC_NAME1}  ${CMAKE_CURRENT_SOURCE_DIR}/src/main_delegates.cpp ${SOURCE_FILES} ${INCLUDE_FILES}   )
TARGET_LINK_LIBRARIES(${EXEC_NAME1} ExecutionGraph benchmark)
ADD_DEPENDENCIES(build_and_benchmark ${EXEC_NAME1})
if( NOT "${ExecutionGraph_DEPENDING_TARGETS}" STREQUAL "")
    ADD_DEPENDENCIES(${EXEC_NAME1} ${ExecutionGraph_DEPENDING_TARGETS})
endif()
defineCompileDefs(${EXEC_NAME1})


add_custom_command(TARGET ${EXEC_NAME1} POST_BUILD
    COMMAND 
          ${CMAKE_COMMAND}
          -E
          copy_directory ${CMAKE_CURRENT_SOURCE_DIR}/python/ ${PROJECT_BINARY_DIR}
)

add_test(NAME ${EXEC_NAME1} COMMAND ${EXEC_NAME1})