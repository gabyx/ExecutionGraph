#!/bin/bash
root="$(git rev-parse --show-toplevel)"
echo -ne "breakpoint set --file ClientSchemeHandlerFactory.cpp --line 37\nrun\nfr v temp" > runFile
lldb -s runFile "$root/build/gui/executionGraphGui/Debug/executionGraphGui.app"