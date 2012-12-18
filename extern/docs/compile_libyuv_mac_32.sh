#!/bin/sh
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
export LDFLAGS="-L${bd}/lib -arch i386 -m32"
export CFLAGS="-Os -I${bd}/include -m32" 
export CPPFLAGS="-Os -I${bd}/include -m32" 

if [ ! -d ${d}/libyuv ] ; then 
    mkdir ${d}/libyuv
    cd ${d}/libyuv
    gclient config http://libyuv.googlecode.com/svn/trunk
    gclient sync
    ${d}/libyuv/trunk/tools/clang/scripts/update.sh

    if [ ${system} == "Darwin" ] ; then 
        cd ${d}/libyuv/trunk/
        xcodebuild -project libyuv.xcodeproj -configuration Release 
        cp ${d}/libyuv/trunk/xcodebuild/Release/libyuv.a ${bd}/lib/
        cp -r ${d}/libyuv/trunk/include ${bd}/
    else
        cd ${d}/libyuv/trunk/ && make -j6 V=1 BUILDTYPE=Release
        cp ${d}/libyuv/trunk/out/Release/obj.target/libyuv.a ${bd}/lib/
        cp -r ${d}/libyuv/trunk/include ${bd}/
    fi

fi




