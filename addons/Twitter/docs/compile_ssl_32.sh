#!/bin/sh
d=${PWD}
if [ ! -d build ] ; then 
   mkdir build
fi

if [ "$1" = "ssl" ] ; then 
    cd openssl
    ./Configure --prefix=${d}/build darwin-i386-cc
    make
    make install
fi
