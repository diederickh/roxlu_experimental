#!/bin/sh
#
# Compile libyuv for mac 64bit.
#
# Versions:
# ---------
# 0.0.1 // 2013.01.03 - The current script builds a i386 library. To create a 64bit library do:
#                       1) run this script
#                       2) open libyuv/trunk/libyuv.xcodeproj
#                       3) select the target then select the settings tab and change architecture to Intel 64
#
#                       Compile libjpeg_turbo too:
#                       1) open third_party/libjpeg_turbo/libjpeg_turbo.xcodeproj
#                       2) select the libjpeg_turbo target
#                       3) change architecture to 64 bit 
#                       4) change the scheme 'Debug' type to 'Release'
#                       5) copy the file libjpeg_turbo.a from trunk/xcodebuild/Release/
#
# 0.0.0 // 2012.??.?? - initial release
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




