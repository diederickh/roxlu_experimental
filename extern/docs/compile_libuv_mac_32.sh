#!/bin/sh
d=${PWD}
bd=${d}/build

export LDFLAGS="-L${bd}/lib -arch i386 -m32"
export CFLAGS="-Os -I${bd}/include -m32" 
export CPPFLAGS="-Os -I${bd}/include -m32" 

if [ ! -d libuv ] ; then 
    mkdir libuv 
    cd libuv 
    git clone https://github.com/joyent/libuv.git .
    git checkout 92fb84b751e18f032c02609467f44bfe927b80c5 
fi


if [ ! -d ${bd} ] ; then 
    mkdir ${bd}
fi

if [ ! -d ${bd}/lib ] ; then 
    mkdir ${bd}/lib
fi

export 
cd ${d}
cd libuv
make clean
make
cp -r include ../build/
cp  libuv.a ../build/lib/

