#!/bin/bash

set -e # exit on errors

cd $ROOT_PATH

# Install eigen3 =======================================================
hg clone https://bitbucket.org/eigen/eigen/ ${ROOT_PATH}/eigen3
cd ${ROOT_PATH}/eigen3 && hg update default
mkdir ${ROOT_PATH}/eigen3Build
cd ${ROOT_PATH}/eigen3Build
cmake ../eigen3 -DCMAKE_INSTALL_PREFIX=$INSTALL_PREFIX
sudo make VERBOSE=1 install

# Install meta =========================================================
git clone https://github.com/ericniebler/meta.git ${ROOT_PATH}/meta
sudo cp -r ${ROOT_PATH}/meta/include/* $INSTALL_PREFIX/include/
#ls -a /usr/local/include/meta