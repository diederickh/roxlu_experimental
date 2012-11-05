#!/bin/sh
d=${PWD}
bd=${d}/build

if [ ! -d libuv ] ; then 
   mkdir libuv 
   cd libuv 
   git clone https://github.com/joyent/libuv.git .
fi

if [ ! -d build ] ; then 
   mkdir build 
fi

if [ ! -d build/lib ] ; then 
   mkdir build/lib
fi

if [ "$1" = "libuv" ] ; then 
   cd libuv
   export CPPFLAGS="-m32"
   export CFLAGS="-m32"
   export LDFLAGS="-m32"
   make clean
   make
   cp -r include ../build/
   cp  libuv.a ../build/lib/
fi
