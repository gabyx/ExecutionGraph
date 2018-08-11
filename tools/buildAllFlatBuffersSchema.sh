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

# Tests
folder="${REPO_DIR}/tests/files"
echo "Building schemas in ${folder} ..."
cd "$folder"
"$flatcCompiler" -I "${REPO_DIR}/include/" --cpp *.fbs

# executionGraph
folder="${REPO_DIR}/include/executionGraph/serialization/schemas"
echo "Building schemas in ${folder} ..."
cd "$folder"
"$flatcCompiler" -I "${REPO_DIR}/include/" -o "cpp"  --cpp *.fbs 
"$flatcCompiler" -I "${REPO_DIR}/include/" -I "${REPO_DIR}/gui/" -o "ts" --ts --no-fb-import *.fbs
"$flatcCompiler" -I "${REPO_DIR}/include/" -o "cpp"  -M --cpp *.fbs > flatbuffers.includes

# python3 "$REPO_DIR/tools/correctFlatBufferSchemaIncludes.py" \
#      --inputFile='./flatbuffers.includes' \
#      --includePaths="${REPO_DIR}/include/executionGraph/serialization/schemas/"  \
#      --generator='cpp' \
#      --regex='.*(executionGraph.*)' --substitution='\1' 

# python3 "$REPO_DIR/tools/correctFlatBufferSchemaIncludes.py" \
#      --inputFile='./flatbuffers.includes' \
#      --includePaths="${REPO_DIR}/include/executionGraph/serialization/schemas"  \
#      --generator='ts' \
#      --regex='.*executionGraph/.*/(\w+)' --substitution='@serialization/\1'

rm flatbuffers.includes

# executionGraphGUI
folder="${REPO_DIR}/gui/executionGraphGUI/messages/schemas"
echo "Building schemas in ${folder} ..."
cd "$folder"
"$flatcCompiler" -I "${REPO_DIR}/include/" -I "${REPO_DIR}/gui/" -o "cpp"  --cpp *.fbs
"$flatcCompiler" -I "${REPO_DIR}/include/" -I "${REPO_DIR}/gui/" -o "ts" --ts --include-prefix "schemas/ts" --no-fb-import *.fbs
"$flatcCompiler" -I "${REPO_DIR}/include/" -I "${REPO_DIR}/gui/" -o "cpp"  -M --cpp *.fbs > flatbuffers.includes

# python3 "$REPO_DIR/tools/correctFlatBufferSchemaIncludes.py" \
#      --inputFile='./flatbuffers.includes' \
#      --includePaths="${REPO_DIR}/include/executionGraph/serialization/schemas"  \
#      --includePaths="${REPO_DIR}/gui/executionGraphGUI/messages/schemas"  \
#      --generator='cpp' \
#      --regex='.*(executionGraph.*)' --substitution='\1' \
#      --regex='.*(executionGraphGUI.*)' --substitution='\1'

# python3 "$REPO_DIR/tools/correctFlatBufferSchemaIncludes.py" \
#      --inputFile='./flatbuffers.includes' \
#      --includePaths="${REPO_DIR}/include/executionGraph/serialization/schemas"  \
#      --includePaths="${REPO_DIR}/gui/executionGraphGUI/messages/schemas"  \
#      --generator='ts' \
#      --regex='.*executionGraph/.*/(\w+)' --substitution='@serialization/\1' \
#      --regex='.*executionGraphGUI/.*/(\w+)' --substitution='@messages/\1'

rm flatbuffers.includes

