#!/bin/sh
#
# Compile libyuv for linux 64bit.
#
# Compiling
# ---------
# - install subversion: apt-get install subversion
#
# - Run (twice): 
#   $ ./compile_libyuv_linux_64.sh 
#   $ ./compile_libyuv_linux_64.sh yuv
#
#
# Versions:
# ---------
# 0.0.1 // 2013.01.06  - initial release
#

d=${PWD}
bd=${d}/build
system=`uname`
if [ ! -d ${bd} ] ; then
    mkdir ${bd}
    mkdir ${bd}/lib
    mkdir ${bd}/include/
fi

if [ ! -d ${bd}/depot_tools ] ; then 
    mkdir ${bd}/depot_tools
    cd ${bd}/depot_tools
    git clone https://git.chromium.org/chromium/tools/depot_tools.git .
fi

export PATH=${PATH}:${bd}/depot_tools

if [ ! -d ${d}/libyuv ] ; then
  mkdir ${d}/libyuv
fi


if [ "$1" = "yuv" ] ; then 
    cd ${d}/libyuv
    gclient config http://libyuv.googlecode.com/svn/trunk
    gclient sync
    ${d}/libyuv/trunk/tools/clang/scripts/update.sh

    cd ${d}/libyuv/trunk/ && make -j6 V=1 BUILDTYPE=Release
    cp ${d}/libyuv/trunk/out/Release/obj.target/libyuv.a ${bd}/lib/
    cp ${d}/libyuv/trunk/out/Release/obj.target/third_party/libjpeg_turbo/libjpeg_turbo.a ${bd}/lib/
    cp -r ${d}/libyuv/trunk/include ${bd}/
fi




