#!/bin/sh
d=${PWD}
bd=${PWD}/build

if [ ! -d ${bd} ] ; then 
    mkdir ${bd}
fi

if [ ! -d ${d}/glfw ] ; then 
    git clone https://github.com/elmindreda/glfw.git 
    git reset --hard d1dac4b9656886d2ad1ea21357058ec145581d44
fi

if [ ! -d ${d}/compile ] ; then 
    mkdir ${d}/compile
fi

cd ${d}/compile
cmake -DCMAKE_INSTALL_PREFIX=${bd} ../glfw/
cmake --build . --target install
