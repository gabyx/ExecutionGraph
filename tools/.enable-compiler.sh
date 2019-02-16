#!/bin/bash
# Source this file to switch compiler with
#   enableCompiler "clang" 
# to enable clang or 
#   enableCompiler
# to reset all set flags.

export OLD_PATH="$PATH"
function enableCompiler(){
  comp="$1"
  if [[ ${comp} == "clang" ]] ; then
    echo "enabling clang"
    export PATH="/usr/local/opt/llvm-latest/bin:$OLD_PATH"
    export CC="/usr/local/opt/llvm-latest/bin/clang"
    export CXX="${CC}++"
    export CPPFLAGS="--config ${HOME}/.clang-flags-llvm-latest.cfg"
    export CXXFLAGS="$CPPFLAGS"
  else
    echo "enabling no compiler"
    export PATH="$OLD_PATH"
    export CC=
    export CXX=
    export LDFLAGS=
    export CPPFLAGS=
    export CXXFLAGS=
  fi
}
export -f enableCompiler