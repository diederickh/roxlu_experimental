#!/bin/sh
d=${PWD}
if [ ! -d ${d}/build ] ; then 
    mkdir build 
fi

#export CC=/usr/bin/clang
#export CXX=/usr/bin/clang++

cd ${d}/build
cmake ../
make
