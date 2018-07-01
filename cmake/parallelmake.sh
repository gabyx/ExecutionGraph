#!/bin/bash

case "$OSTYPE" in
  darwin*)  cores=$(sysctl -n hw.ncpu) ;; 
  linux*)   cores=$(grep -c ^processor /proc/cpuinfo) ;;
  bsd*)     cores=$(grep -c ^processor /proc/cpuinfo) ;;
  *)        cores=1 ;;
esac

echo "Building with $cores cores!"
make -j$cores "$@" 

exit