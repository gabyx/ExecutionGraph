#!/bin/bash
# Build angular only if the directory is not yet created

set -e

cmd="$1"
folder="$2"
target="$3"

echo "Building Angular Application into '$folder' ... "
if [[ -d "$folder" ]]; then
    echo "Angular Build '$folder' already done!"
else
    echo "Launching npm run ... "
    $cmd run "$target" -- "--output-path" "$folder"
fi