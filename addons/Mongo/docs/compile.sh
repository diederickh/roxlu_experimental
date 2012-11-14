#!/bin/sh
d=${PWD}
bd=${d}/build
if [ ! -d source ] ; then 
    mkdir source 
    cd source 
    git clone https://github.com/mongodb/mongo-c-driver .
    #git checkout aee9ff2778d39a86fcca70e09752ea45d44c1e68
    git checkout v0.6
    scons
    mkdir compiled
    mkdir compiled/lib
    mkdir compiled/include
    mv *.a compiled/lib
    mv src/*.h compiled/include
fi
