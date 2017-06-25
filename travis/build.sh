#!/bin/bash
# "BUILD ========================================================================"

# "Go to $ROOT_PATH"
cd $ROOT_PATH

if [ -z "$BUILD_TYPE" ]; then export BUILD_TYPE=Release; fi

echo "Build ExecutionGraph:"
cd $CHECKOUT_PATH

if [ ! -d $ROOT_PATH/build ]; then mkdir $ROOT_PATH/build; fi
cd $ROOT_PATH/build
cmake $CHECKOUT_PATH -DCMAKE_BUILD_TYPE=$BUILD_TYPE
make VERBOSE=1
make install
cd $ROOT_PATH

# make install and library usage!
echo "Install and test if ExecutionGraph links:"
mkdir $ROOT_PATH/buildLibUsage
cd $ROOT_PATH/buildLibUsage
INSTALL=$(find $ROOT_PATH/build/install/lib/cmake/* -type d)
echo "Install dir= $INSTALL"
cmake $CHECKOUT_PATH/examples/libraryUsage -DCMAKE_BUILD_TYPE=$BUILD_TYPE -DExecutionGraph_DIR=$INSTALL
make VERBOSE=1
cd $ROOT_PATH

#"Run Unit Tests:"
cd $ROOT_PATH/build
make build_and_test

# "BUILD COMPLETE ================================================================"
