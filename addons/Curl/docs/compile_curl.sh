#!/bin/sh
d=${PWD}
if [ ! -d build ] ; then 
    mkdir build
    mkdir build/compiled
fi

cd build
cmake -DCMAKE_INSTALL_PREFIX=${d}/build/compiled -DCURL_STATICLIB=ON ../curl/ 
make
make install
