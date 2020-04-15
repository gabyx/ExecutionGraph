set(SOURCE_FILES ${PROJECT_SOURCE_DIR}/tests/src/TestFunctions.cpp)

set(INCLUDE_FILES)

#include directories
set(INCLUDE_DIRS ${PROJECT_SOURCE_DIR}/tests/include/ ${CMAKE_CURRENT_SOURCE_DIR}/../
                 ${CMAKE_CURRENT_SOURCE_DIR}/include ${CMAKE_CURRENT_SOURCE_DIR}/files)

include(AddTestTargets)

# Flatbuffer Test
set(addLibs "flatbuffersLib" "rttrLib" "ExecutionGraph::Serialization")
addTest("GUI-Flatbuffer" ${addLibs} OFF 0)

# Http Server
set(addLibs "boostBeastLib" "ExecutionGraph::Serialization")
addTest("GUI-HttpServer" ${addLibs} OFF 0)

# Http Server Async
set(addLibs "boostBeastLib" "ExecutionGraph::Serialization")
addTest("GUI-HttpServerAsync" ${addLibs} OFF 0)
