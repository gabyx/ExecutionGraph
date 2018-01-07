#!/bin/bash
# format all TypeScript .ts files in the repository
# to use this tool install: npm install -g typescript-formatter

export TOOLS_DIR=$( cd "$( dirname "${BASH_SOURCE[0]}" )" && pwd )
export REPO_DIR="$TOOLS_DIR/../"

function formatTS() {
    find "$1" -type f \( -name "*.ts" \)  | xargs tsfmt --replace --baseDir "$REPO_DIR"
}

formatTS "$REPO_DIR/gui" 