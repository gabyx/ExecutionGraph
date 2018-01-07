#!/bin/bash
# format all .cpp|.hpp files in the repository

export TOOLS_DIR=$( cd "$( dirname "${BASH_SOURCE[0]}" )" && pwd )
export REPO_DIR="$TOOLS_DIR/../"

function formatCpp() {
    find "$1" -type f \( -name "*.hpp" -or  -name "*.cpp" \) | xargs clang-format -i 
}

formatCpp "$REPO_DIR/include"  
formatCpp "$REPO_DIR/src" 
formatCpp "$REPO_DIR/examples" 
formatCpp "$REPO_DIR/tests" 
formatCpp "$REPO_DIR/gui"
formatCpp "$REPO_DIR/benchmarks"