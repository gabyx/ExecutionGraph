#!/bin/bash
owndir="$(cd "$(dirname "$0")"; pwd -P)"

cd ${owndir}

rm -r build
mkdir -p build
mkdir -p buildExternal
cd build
cmake .. -DExecutionGraph_BUILD_TESTS=true \
         -DExecutionGraph_BUILD_LIBRARY=true \
         -DExecutionGraph_BUILD_GUI=true \
         -DExecutionGraph_EXTERNAL_BUILD_DIR="$(pwd)/../buildExternal" \
         -DExecutionGraph_USE_ADDRESS_SANITIZER=true \
         -DCMAKE_EXPORT_COMPILE_COMMANDS=true