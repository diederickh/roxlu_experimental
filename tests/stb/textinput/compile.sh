#!/bin/sh
d=${PWD}
if [ ! -d ${d}/build ] ; then 
    mkdir build 
fi

cd build
cmake ../ 
make clean
make    
