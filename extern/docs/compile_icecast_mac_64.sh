#!/bin/bash

# Compile script for icecast, using these libraries:
# --------------------------------------------------
# lame-3.99.5.tar.gz
# libogg-1.3.0.tar.gz 
# libsamplerate-0.1.8.tar.gz
# libtheora-1.1.1.tar.bz2
# libtool-2.2.1.10.tar.gz
# libvorbis-1.3.3.tar.gz
# libxml2-2.8.0.tar.gz
# pkg-config-0.18.1.tar.gz
# x264.bz2 (latest)
# yasm-1.2.0.tar.gz
# libshout-2.3.1.tar.gz
#
#
# You need to run this script a couple of times, every time using
# another argument to compile the next library.
# 1. First create a directory "sources" and download all the files you 
#    see above.
# 2. Then run this script once to extract everything 
#
# 3. Then execute this script a couple of times in this order:
# 
# 
#
d=${PWD}
bd=${d}/build
sd=${d}/sources/
found_all_sources=1

check_source() {
    p="${sd}/${1}"
    if [ ! -f ${p} ] ; then 
        echo "Download ${1}"
        found_all_sources=0
    else 
        echo "Found: ${p}"
    fi
}

check_source "lame-3.99.5.tar.gz"
check_source "pkg-config-0.18.1.tar.gz"
check_source "libtool-2.2.10.tar.gz"
check_source "libogg-1.3.0.tar.gz"
check_source "libvorbis-1.3.3.tar.gz"
check_source "libsamplerate-0.1.8.tar.gz"
check_source "libxml2-2.8.0.tar.gz"
check_source "libtheora-1.1.1.tar.bz2"
check_source "x264.bz2"
check_source "libshout-2.3.1.tar.gz"

if [ ${found_all_sources} == 0 ] ; then
    echo "Download missing sources."
    exit
fi

if [ ! -d "libvpx" ] ; then 
    git clone http://git.chromium.org/webm/libvpx.git
fi

if [ ! -d "opus" ] ; then 
    git clone http://git.xiph.org/opus.git
fi

if [ ! -d "pkgconfig" ] ; then 
    cp sources/pkg-config-0.18.1.tar.gz . && tar -zxvf pkg-config-0.18.1.tar.gz && mv pkg-config-0.18.1 pkgconfig && rm pkg-config-0.18.1.tar.gz
fi

if [ ! -d "lame" ] ; then 
    cp sources/lame-3.99.5.tar.gz . && tar -zxvf lame-3.99.5.tar.gz && mv lame-3.99.5 lame && rrm lame-3.99.5.tar.gz
fi

if [ ! -d "libtool" ] ; then
    cp sources/libtool-2.2.10.tar.gz . && tar -zxvf libtool-2.2.10.tar.gz && mv libtool-2.2.10 libtool && rm libtool-2.2.10.tar.gz
fi

if [ ! -d "libogg" ] ; then 
    cp sources/libogg-1.3.0.tar.gz . && tar -zxvf libogg-1.3.0.tar.gz && mv libogg-1.3.0 libogg && rm libogg-1.3.0.tar.gz
fi

if [ ! -d "libvorbis" ] ; then 
    cp sources/libvorbis-1.3.3.tar.gz . && tar -zxvf libvorbis-1.3.3.tar.gz && mv libvorbis-1.3.3 libvorbis && rm libvorbis-1.3.3.tar.gz
fi

if [ ! -d "libsamplerate" ] ; then
    cp sources/libsamplerate-0.1.8.tar.gz . && tar -zxvf libsamplerate-0.1.8.tar.gz && mv libsamplerate-0.1.8 libsamplerate && rm libsamplerate-0.1.8.tar.gz
fi

if [ ! -d "icecast" ] ; then 
    svn co http://svn.xiph.org/icecast/trunk/icecast icecast
fi

if [ ! -d "libshout" ] ; then
    cp sources/libshout-2.3.1.tar.gz . && tar -zxvf libshout-2.3.1.tar.gz && mv libshout-2.3.1 libshout && rm libshout-2.3.1.tar.gz
fi

if [ -d "yasm" ] ; then
    if [ ! -f yasm/INSTALL ] ; then
        rm -rf yasm
    fi
fi

if [ ! -d "xml" ] ; then
    cp sources/libxml2-2.8.0.tar.gz . && tar -zxvf libxml2-2.8.0.tar.gz && mv libxml2-2.8.0 xml && rm libxml2-2.8.0.tar.gz
fi

if [ ! -d "theora" ] ; then 
    cp sources/libtheora-1.1.1.tar.bz2 . && bunzip2 libtheora-1.1.1.tar.bz2 && tar -xvf libtheora-1.1.1.tar && mv libtheora-1.1.1 theora && rm libtheora-1.1.1.tar.bz2
fi

if [ ! -d "yasm" ] ; then 
    cp sources/yasm-1.2.0.tar.gz . && tar -zxvf yasm-1.2.0.tar.gz && mv yasm-1.2.0 yasm && rm yasm-1.2.0.tar.gz
fi

if [ -d "x264" ] ; then
    if [ ! -f x264/x264.c ] ; then
        rm -rf x264
    fi
fi

if [ ! -d "x264" ] ; then
    cp sources/x264.bz2 . && bunzip2 x264.bz2 && mv x264 x264.tar && tar -xvf x264.tar && mv x264-* x264
fi

export PKG_CONFIG=${bd}/bin/pkg-config
export PKG_CONFIG_PATH=${bd}/lib/pkgconfig/
export CFLAGS="-Os -I${bd}/include/ -m64"
export CPPFLAGS="-Os -I${bd}/include/ -arch x86_64"
export LDFLAGS="-L${bd}/lib/ -arch x86_64"
export PATH="${bd}/bin/:${PATH}"
export CXX="i686-apple-darwin10-llvm-g++-4.2"
export CC="gcc"

if [ ! -d ${bd} ] ; then 
    mkdir ${bd}
fi

if [ ! -d libvpx ] ; then 
    cd ${d} && git clone http://git.chromium.org/webm/libvpx.git
fi

# yasm
if [ "$1" = "yasm" ] ; then 
  cd ${d}/yasm
    ./configure --enable-static --enable-shared=no --prefix=${bd}
    make clean
    make 
    make install
fi

# opus 
if [ "$1" = "opus" ] ; then 
    cd ${d}/opus
    ./autogen.sh
    ./configure --enable-static --disabled-shared --prefix=${bd}
    make clean
    make 
    make install
fi

# vpx
if [ "$1" = "vpx" ] ; then 
    cd ${d}/libvpx
    ./configure --enable-static --prefix=${bd} --enable-vp8 --as=yasm
    make clean
    make 
    make install
fi

# mp3
if [ "$1" = "mp3" ] ; then 
    cd ${d}/lame
    ./configure --enable-static --enable-shared=no --prefix=${bd}
    make clean 
    make
    make install
fi

# xml
if [ "$1" = "xml" ] ; then 
    cd ${d}/xml
    ./configure --enable-static --enable-shared=no --prefix=${bd}
    make clean
    make
    make install
fi


# libogg
if [ "$1" = "ogg" ] ; then 
    cd ${d}/libogg
    ./configure --enable-static --enable-shared=no --prefix=${bd}
    make clean 
    make
    make install
fi

# theora
if [ "$1" = "theora" ] ; then 
    cd ${d}/theora
    ./configure --enable-static --enable-shared=no --prefix=${bd}
    make clean
    make
    make install
fi

# vorbis
if [ "$1" = "vorbis" ] ; then
    cd ${d}/libvorbis
    ./configure --enable-static --enable-shared=no --prefix=${bd}
    make clean
    make
    make install
fi

# libtool
if [ "$1" = "libtool" ] ; then 
    cd ${d}/libtool
    ./configure --enable-static --enable-shared=no --prefix=${bd}
    make clean
    make 
    make install
fi

# icecast
if [ "$1" = "icecast" ] ; then 
    cd ${d}/icecast
    ./autogen.sh
    ./configure --enable-static --enable-shared=no --prefix=${bd} --with-vorbis=${bd}/lib/libvorbis.a
    make clean
    make 
    make install
fi

# libshout (client library for icecast)
if [ "$1" = "libshout" ] ; then 
    cd ${d}/libshout
    ./configure --enable-shared=no --enable-static=yes --prefix=${bd} --with-ogg=${bd}/include/ --with-vorbis=${bd}/lib/ --with-speex
    make
    make clean
    make install
fi

