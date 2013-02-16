#!/bin/sh
set -x
d=${PWD}
bd=${d}/../../bin
appdir=${bd}/../

# get app name
cd ${appdir}
app=${PWD##*/}

# make sure we have the build + data dirs
cd ${d}
if [ ! -d ${b}/build ] ; then
   mkdir ${d}/build
fi

if [ ! -d ${bd} ] ; then 
   mkdir ${bd}
   mkdir ${bd}/data
fi

cd build
make install

cd ${bd}

lldb ./${app}


