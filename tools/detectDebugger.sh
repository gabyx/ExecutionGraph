#!/bin/bash

REPO_DIR="$(git rev-parse --show-toplevel)"

LINE=$(grep -E "^CMAKE_CXX_COMPILER" "$REPO_DIR/build/CMakeCache.txt")

if [ $? -eq 0 ]; then
    if echo "$LINE" | grep -q -E "g++|gcc"; then
        echo "gdb"
    elif echo "$LINE" | grep -q -E "clang"; then
        echo "lldb"
    fi
else
    echo "lldb"
fi
