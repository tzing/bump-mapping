#!/bin/sh

git submodule update --init
cd glm
./autogen.sh
./configure --prefix=$(pwd)
make -j8
make install
