#!/bin/sh
# Used GNU PCRE 8.32

d=${PWD}
bd=${d}/build

if [ ! -d ${bd} ] ; then 
    mkdir ${bd}
fi

cd ${d}/pcre
./configure --enable-static --prefix=${bd}
make
make install
