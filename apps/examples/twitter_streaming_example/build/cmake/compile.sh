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
cmake ../
make install
