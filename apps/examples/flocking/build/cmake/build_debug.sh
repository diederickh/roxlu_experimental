#!/bin/sh
d=${PWD}
bd=${d}/../../bin
app=${PWD##*/}

if [ ! -d ${b}/build.debug ] ; then
   mkdir ${d}/build.debug
fi

if [ ! -d ${bd} ] ; then 
   mkdir ${bd}
   mkdir ${bd}/data
fi

cd build.debug
cmake -DCMAKE_BUILD_TYPE=Debug ../
make 
make install
