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

if [ "$1" = "libevent" ] ; then 
    cd libevent
    export CFLAGS="-Os -m32 -I${d}/build/include/"
    export LDFLAGS="-m32 -L${d}/build/lib/"
    ./configure --prefix=${d}/build
    make
    make install
fi
