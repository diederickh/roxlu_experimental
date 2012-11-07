#!/bin/sh
d=${PWD}
app=${PWD##*/}

if [ ! -d build ] ; then 
    mkdir build 
fi

cd build
cmake ../
make
make install
cd ${d}
cd bin
./${app}
