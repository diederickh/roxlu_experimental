#!/bin/sh
d=${PWD}
bd=${d}/../../bin
app=${PWD##*/}

if [ ! -d ${b}/build.release ] ; then
   mkdir ${d}/build.release
fi

if [ ! -d ${bd} ] ; then 
   mkdir ${bd}
   mkdir ${bd}/data
fi

cd build.release
cmake -DCMAKE_BUILD_TYPE=Release ../
make -j4
make install
