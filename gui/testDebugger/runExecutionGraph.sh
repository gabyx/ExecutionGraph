#!/bin/bash
root="$(git rev-parse --show-toplevel)"
echo -ne "breakpoint set --file FileSchemeHandlerFactory.cpp --line 37\nrun\nfr v temp" > runFile
lldb -s runFile "$root/build/gui/executionGraphGui/Debug/ExecutionGraphGUI.app"