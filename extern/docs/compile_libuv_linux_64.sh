#!/bin/sh
d=${PWD}
bd=${d}/build

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

cd ${d}
cd libuv
make clean
make
cp -r include ../build/
cp  libuv.a ../build/lib/

