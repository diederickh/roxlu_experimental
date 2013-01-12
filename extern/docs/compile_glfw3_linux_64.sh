#!/bin/sh
d=${PWD}
bd=${PWD}/build

if [ ! -d ${bd} ] ; then 
    mkdir ${bd}
fi

if [ ! -d ${d}/glfw ] ; then 
    git clone https://github.com/elmindreda/glfw.git 
fi

if [ ! -d ${d}/compile ] ; then 
    mkdir ${d}/compile
fi

cd ${d}/compile
cmake -DCMAKE_INSTALL_PREFIX=${bd} ../glfw/
cmake --build . --target install
