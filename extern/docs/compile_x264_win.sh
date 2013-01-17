#!/bin/sh
/**
 * Build x264 on windows 7
 * 
 * HOWTO
 * -----
 * 1) Execute this file in a MinGW shell
 * 2) After compiling you should have a 'build' directory with 
 *    a static and shared (dll) library. We only need to copy 
 *    the import library .lib manually from the x264 source dir.
 * 
 *
 */

d=${PWD}
bd=${d}/build
if [ ! -d x264 ] ; then
    mkdir x264
    cd x264
    git clone git://git.videolan.org/x264.git .
    git reset --hard bc13772e21d0e61dea6ba81d0d387604b5b360df
fi

cd ${d}/x264
./configure --prefix=${bd} \
    --enable-shared \
    --enable-static \
    --disable-cli \
    --extra-ldflags=-Wl,--output-def=libx264.def
make
make install

cd ${d}/x264
LIB /DEF:libx264.def 
