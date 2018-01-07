#!/bin/bash
# format all JSON files in the repository
# to use this tool install: npm install -g json-fmt

export TOOLS_DIR=$( cd "$( dirname "${BASH_SOURCE[0]}" )" && pwd )
export REPO_DIR="$TOOLS_DIR/../"

function formatJSON() {
    find "$1" -type f \( -name "*.json" \)  | xargs json-fmt "{}" -p -o "{}"  
}

formatJSON "$REPO_DIR/gui"