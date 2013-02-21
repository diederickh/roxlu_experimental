#!/bin/sh

d=${PWD}
bd=${d}/../../bin
app=${PWD##*/}

if [ ! -d ${b}/build ] ; then
   mkdir ${d}/build
fi

if [ ! -d ${bd} ] ; then 
   mkdir ${bd}
   mkdir ${bd}/data
fi

cd build
# cmake -DCMAKE_BUILD_TYPE=Debug ../
cmake -DCMAKE_BUILD_TYPE=Release ../
make install
