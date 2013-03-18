#!/bin/bash
d=${PWD}
if [ ! -d ${d}/build ] ; then 
    mkdir -f ${d}/build
fi

cd ${d}/build
cmake -G "Xcode" ..\

