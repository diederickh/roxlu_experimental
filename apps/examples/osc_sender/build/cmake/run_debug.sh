#!/bin/sh
set -x
d=${PWD}
bd=${d}/../../bin
appdir=${bd}/../

# get app name
cd ${appdir}
app=${PWD##*/}_debug

# make sure we have the build + data dirs
cd ${d}

if [ ! -d ${bd} ] ; then 
   mkdir ${bd}
   mkdir ${bd}/data
fi

./build_debug.sh

cd ${bd}

./${app}

