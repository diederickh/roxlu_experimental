#!/bin/sh
# Used openssl-1.0.1c, mac 10.6.8
d=${PWD}
if [ ! -d build ] ; then 
   mkdir build
fi

if [ "$1" = "ssl" ] ; then 
    cd openssl
    ./Configure --prefix=${d}/build darwin-i386-cc
    make clean
    make
    make install
fi
