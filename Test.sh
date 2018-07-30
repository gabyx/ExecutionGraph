#!/bin/bash
# comp="gcc"
# export PATH="/usr/local/opt/$comp/bin:$PATH"
# export CC="/usr/local/opt/$comp/bin/gcc-7"
# export CXX="/usr/local/opt/$comp/bin/g++-7"
# export LDFLAGS="-L/usr/local/opt/$comp/lib/gcc/7 -Wl,-rpath,/usr/local/opt/$comp/lib/gcc/7"
# export CPPFLAGS="-I/usr/local/opt/$comp/include -I/usr/local/opt/$comp/include/c++/7.2.0"
# export CXXFLAGS="$CPPFLAGS"

# cd /Users/gabrielnuetzi/Desktop/ExecutionGraph/build/gui/executionGraphGUI && ${CXX}  -DRTTR_DLL -isystem /usr/local/opt/rttr/include $CPPFLAGS -g -std=c++17 -o Test.cpp.o -c /Users/gabrielnuetzi/Desktop/ExecutionGraph/gui/executionGraphGUI/backend/Test.cpp

comp="gcc"

cd /Users/gabrielnuetzi/Desktop/ExecutionGraph/build/gui/executionGraphGUI && clang++  -DRTTR_DLL -isystem /usr/local/opt/rttr/include -g -std=c++17 -o Test.cpp.o -c /Users/gabrielnuetzi/Desktop/ExecutionGraph/gui/executionGraphGUI/backend/Test.cpp