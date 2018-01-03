#!/bin/bash
# format all .cpp|.hpp files in the repository

export TOOLS_DIR=$( cd "$( dirname "${BASH_SOURCE[0]}" )" && pwd )
export REPO_DIR="$TOOLS_DIR/../"

find $REPO_DIR/include -type f \( -name "*.hpp" -or  -name "*.cpp" \) | xargs clang-format -i 
find $REPO_DIR/src -type f \( -name "*.hpp" -or  -name "*.cpp" \) | xargs clang-format -i 
find $REPO_DIR/examples -type f \( -name "*.hpp" -or  -name "*.cpp" \)  | xargs clang-format -i 
find $REPO_DIR/tests -type f \( -name "*.hpp" -or  -name "*.cpp" \)  | xargs clang-format -i 
find $REPO_DIR/gui -type f \( -name "*.hpp" -or  -name "*.cpp" \)  | xargs clang-format -i 
find $REPO_DIR/benchmarks -type f \( -name "*.hpp" -or  -name "*.cpp" \)  | xargs clang-format -i 