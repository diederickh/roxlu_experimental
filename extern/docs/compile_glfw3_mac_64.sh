#!/bin/sh
d=${PWD}
bd=${PWD}/build

if [ ! -d ${bd} ] ; then 
    mkdir ${bd}
fi

if [ ! -d ${d}/glfw ] ; then 
    git clone https://github.com/elmindreda/glfw.git 
    git reset --hard 747b6d880563c0461df028d9e287652f16de7b17
fi

if [ ! -d ${d}/compile ] ; then 
    mkdir ${d}/compile
fi

cd ${d}/compile
cmake -DCMAKE_INSTALL_PREFIX=${bd} ../glfw/
cmake --build . --target install
