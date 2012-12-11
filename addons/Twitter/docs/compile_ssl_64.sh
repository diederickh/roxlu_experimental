#!/bin/sh
# Used openssl-1.0.1c
d=${PWD}
if [ ! -d build ] ; then 
   mkdir build
fi

if [ "$1" = "ssl" ] ; then 
    cd openssl
    ./Configure --prefix=${d}/build darwin64-x86_64-cc
    make clean
    make
    make install
fi
