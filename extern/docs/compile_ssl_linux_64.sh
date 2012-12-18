#!/bin/sh
# Used openssl-1.0.1c
d=${PWD}
if [ ! -d build ] ; then 
   mkdir build
fi

if [ "$1" = "ssl" ] ; then 
    cd openssl
    ./Configure no-rc5 no-shared --prefix=${d}/build linux-x86_64
    make clean
    make
    make install
fi
