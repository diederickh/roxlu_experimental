#!/bin/sh
# 
# Compile script for video recorder lib
# -------------------------------------
# Creates statis libraries for video recorder addon
# 
# Dependencies
# ------------
# - libav: git clone git://git.libav.org/libav.git && git reset --hard 65d94f63ca38b46a9d3719cb7d986d2daef90416
# - speex 1.2 rc:  ( http://www.speex.org/downloads/ )
# - pkg-config-0.18.1 (  http://pkgconfig.freedesktop.org/releases/pkg-config-0.18.1.tar.gz )
# - yasm-1.2.0 ( http://www.tortall.net/projects/yasm/releases/yasm-1.2.0.tar.gz )
# - x264-snapshot-20120719-2245 ( http://www.videolan.org/developers/x264.html , download last 
# 
# This scripts compiles 64bit libs for mac, for 32bit libs 
# set the -m32/-arch x86_32 c/cppflags
# For example: libav_options_base = "--arch=x86_32 --disable-libmp3lame --enable-libspeex --enable-cross-compile --enable-libx264 --enable-gpl --disable-doc --disable-network --disable-avserver --target-os=darwin cc='/Developer/usr/bin/cc -m32' "
#
#
# IMPORTANT: I HAD TO DISABLE SSE ON MAC AS IT GAVE AN ERROR WITH SWSCALE
#


d=${PWD}
bd=${d}/build

export PKG_CONFIG=${bd}/bin/pkg-config
export PKG_CONFIG_PATH=${bd}/lib/pkgconfig
export LDFLAGS="-L${bd}/lib"
export CFLAGS="-Os -I${bd}/include" 
export CXXFLAGS=${CFLAGS}
export PATH=${bd}/bin:${PATH}

if [ ! -d ${bd} ] ; then 
    mkdir ${bd}
fi

if [ ! -d libav ] ; then
    mkdir libav 
    cd libav 
    git clone git://git.libav.org/libav.git .
    #&& git reset --hard 65d94f63ca38b46a9d3719cb7d986d2daef90416
fi

if [ ! -f speex.tar.gz ] ; then 
    curl http://downloads.xiph.org/releases/speex/speex-1.2rc1.tar.gz -o speex.tar.gz
fi

if [ ! -d speex ] ; then 
    tar -zxvf speex.tar.gz
    mv speex-* speex
fi

if [ ! -f pkgconfig.tar.gz ] ; then 
    curl http://pkgconfig.freedesktop.org/releases/pkg-config-0.18.1.tar.gz -o pkgconfig.tar.gz
fi

if [ ! -d pkgconfig ] ; then 
    tar -zxvf pkgconfig.tar.gz
    mv pkg-config-* pkgconfig
fi

if [ ! -f yasm.tar.gz ] ; then 
    curl http://www.tortall.net/projects/yasm/releases/yasm-1.2.0.tar.gz -o yasm.tar.gz 
fi

if [ ! -d yasm ] ; then 
    tar -zxvf yasm.tar.gz
    mv yasm-* yasm
fi

if [ ! -d x264 ] ; then 
    mkdir x264 
    cd x264
    git clone git://git.videolan.org/x264.git .
fi

if [ "$1" = "pkg" ] ; then 
    cd pkgconfig 
    ./configure --prefix=${bd}
    make 
    make install
fi

if [ "$1" = "yasm" ] ; then 
    cd yasm
    ./configure --prefix=${bd} 
    make 
    make install
fi

if [ "$1" = "x264" ] ; then 
    cd x264
    ./configure --prefix=${bd} --enable-static 
    make
    make install
fi

if [ "$1" = "speex" ] ; then 
    cd speex
    ./configure --prefix=${bd} --enable-sse --enable-static 
    make 
    make install
fi


if [ "$1" = "av" ] ; then 
    cd libav 
    ./configure --prefix=${bd} --enable-static --enable-libx264 --enable-libspeex --target-os=darwin --enable-gpl --disable-sse
    make 
    make install
fi

    



