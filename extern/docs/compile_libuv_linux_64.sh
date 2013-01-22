#!/bin/sh
d=${PWD}
bd=${d}/build

if [ ! -d libuv ] ; then 
    mkdir libuv 
    cd libuv 
    git clone https://github.com/joyent/libuv.git .
    git reset --hard 358957cdec34cc2c7c0116637e624800b0e5e36a
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

