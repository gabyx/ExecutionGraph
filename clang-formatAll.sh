#!/bin/bash
# format all .cpp|.hpp files in the repository

export REPO_DIR=$( cd "$( dirname "${BASH_SOURCE[0]}" )" && pwd )

find $REPO_DIR/include -type f \( -name "*.hpp" -or  -name "*.cpp" \) | xargs clang-format -i 
find $REPO_DIR/src -type f \( -name "*.hpp" -or  -name "*.cpp" \) | xargs clang-format -i 
find $REPO_DIR/examples -type f \( -name "*.hpp" -or  -name "*.cpp" \)  | xargs clang-format -i 
