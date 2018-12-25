#!/bin/bash
# Build angular only if the directory is not yet created

set -e

npmCmd="$1"
folder="$2"
target="$3"

echo "Building Angular Application into '$folder' ... "
if [[ -d "$folder" ]]; then
    echo "Angular Build '$folder' already done!"
else
    echo "Launching npm install ..."
    $npmCmd install
    echo "Launching npm run ... "
    $npmCmd run "$target"
fi
