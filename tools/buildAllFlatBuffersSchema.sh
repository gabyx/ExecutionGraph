#!/bin/bash
# build all flatbuffers schemas in this project
owndir="$(cd "$(dirname "$0")"; pwd -P)"
cd "${owndir}"
REPO_DIR="$(git rev-parse --show-toplevel)"

flatcCompiler=$(find "$REPO_DIR/buildExternal/install" -type f -executable -path "*/bin/flatc")

if [[ "${flatcCompiler}" != "" ]]; then
    echo "Using flatc compiler: ${flatcCompiler}"
else
    echo "No flatc compiler found in the project!"
fi

set -e # all errors are fatal

$flatcCompiler --version

# Tests
folder="${REPO_DIR}/tests/files"
echo "Building schemas in ${folder} ..."
cd "$folder"
"$flatcCompiler" -I "${REPO_DIR}/include/" --cpp *.fbs

# executionGraph
folder="${REPO_DIR}/include/executionGraph/serialization/schemas"
echo "Building schemas in ${folder} ..."
cd "$folder"
"$flatcCompiler" -I "${REPO_DIR}/include/" -o "cpp"  --keep-prefix --cpp *.fbs 
"$flatcCompiler" -I "${REPO_DIR}/include/" -I "${REPO_DIR}/gui/" --keep-prefix -o "ts" --ts --no-fb-import *.fbs

python3 "$REPO_DIR/tools/correctFlatBufferSchemaIncludes.py" \
    --input="${folder}/cpp/*.h" \
    --includeRegex="(#include\s+"'\")'"(.*exec.*)"'(\"'".*)" \
    --includePath="${folder}/cpp"  \
    --regex='(.*executionGraph/.*/)(.*)' --substitution='\1cpp/\2'

python3 "$REPO_DIR/tools/correctFlatBufferSchemaIncludes.py" \
    --input="${folder}/ts/*.ts" \
    --includeRegex="(from\s+"'\")'"(.*exec.*)"'(\"'".*)" \
    --includePath="${folder}/ts"  \
    --regex='.*(executionGraph/.*/)(.*)' --substitution='./cpp/\1'

# executionGraphGUI
folder="${REPO_DIR}/gui/executionGraphGUI/messages/schemas"
echo "Building schemas in ${folder} ..."
cd "$folder"
"$flatcCompiler" -I "${REPO_DIR}/include/" -I "${REPO_DIR}/gui/" --keep-prefix -o "cpp"  --cpp *.fbs
"$flatcCompiler" -I "${REPO_DIR}/include/" -I "${REPO_DIR}/gui/" --keep-prefix -o "ts" --ts --no-fb-import *.fbs

python3 "$REPO_DIR/tools/correctFlatBufferSchemaIncludes.py" \
    --input="${folder}/cpp/*.h" \
    --includeRegex="(#include\s+"'\")'"(.*exec.*)"'(\"'".*)" \
    --includePath="${folder}/cpp"  \
    --regex='(.*executionGraph(?:GUI)?/.*/)(.*)' --substitution='\1cpp/\2'

python3 "$REPO_DIR/tools/correctFlatBufferSchemaIncludes.py" \
    --input="${folder}/ts/*.ts" \
    --includePath="${folder}/ts"  \
    --includeRegex="(from\s+"'\")'"(.*exec.*)"'(\"'".*)" \
    --regex='.*executionGraphGUI/.*/(.*)' --substitution='./\1' \
    --regex='.*executionGraph/.*/(\w+)' --substitution='@eg/serialization/\1' 

