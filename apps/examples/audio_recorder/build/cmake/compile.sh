#!/bin/sh
#set -x
d=${PWD}
bd=${d}/../../bin
app=${PWD##*/}

if [ ! -d ${d}/build ] ; then
   mkdir ${d}/build
fi

if [ ! -d ${bd}/data ] ; then 
   mkdir ${bd}
   mkdir ${bd}/data
fi

cd build
cmake ../
make VERBOSE=1
make install
