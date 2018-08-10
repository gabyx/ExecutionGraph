#!/bin/bash
# build all flatbuffers schemas in this project
REPO_DIR="$(git rev-parse --show-toplevel)"

flatcCompiler=$(find "$REPO_DIR/buildExternal/install" -type f -executable -path "*/bin/flatc")

if [[ "${flatcCompiler}" != "" ]]; then
    echo "Using flatc compiler: ${flatcCompiler}"
else
    echo "No flatc compiler found in the project!"
fi

set -e # all errors are fatal

folder="${REPO_DIR}/tests/files"
echo "Building schemas in ${folder} ..."
cd "$folder"
"$flatcCompiler" -I "${REPO_DIR}/include/" --cpp *.fbs

folder="${REPO_DIR}/include/executionGraph/serialization/schemas"
echo "Building schemas in ${folder} ..."
cd "$folder"
"$flatcCompiler" -I "${REPO_DIR}/include/" -o "cpp" --cpp *.fbs
"$flatcCompiler" -I "${REPO_DIR}/include/" -I "${REPO_DIR}/gui/" -o "ts" --ts --no-fb-import *.fbs

folder="${REPO_DIR}/gui/executionGraphGUI/messages/schemas"
echo "Building schemas in ${folder} ..."
cd "$folder"
"$flatcCompiler" -I "${REPO_DIR}/include/" -I "${REPO_DIR}/gui/" -o "cpp" --cpp *.fbs
"$flatcCompiler" -I "${REPO_DIR}/include/" -I "${REPO_DIR}/gui/" -o "ts" --ts --no-fb-import *.fbs
