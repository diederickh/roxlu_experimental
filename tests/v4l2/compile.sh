#!/bin/sh
if [ ! -d build ] ; then
    mkdir build
fi

export CC=/usr/bin/clang
export CXX=/usr/bin/clang++


cd build
cmake ../ 
make 
