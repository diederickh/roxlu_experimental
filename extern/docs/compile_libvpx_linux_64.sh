#!/bin/sh
d=${PWD}
bd=${d}/build

if [ ! -d ${bd} ] ; then
    mkdir ${bd}
fi

if [ ! -f yasm-1.2.0.tar.gz ] ; then
    echo "Download: yasm-1.2.0.tar.gz, http://www.tortall.net/projects/yasm/releases/yasm-1.2.0.tar.gz"
    exit
fi

if [ ! -d vpx ] ; then
    mkdir vpx 
    cd vpx
    git clone http://git.chromium.org/webm/libvpx.git .
fi

cd ${d}
if [ ! -d yasm ] ; then
    tar -zxvf yasm-1.2.0.tar.gz
    mv yasm-1.2.0 yasm
fi

cd yasm
./configure --prefix=${bd}
make 
make install


export PATH=${bd}/bin:${PATH}
cd ${d}
cd vpx
./configure --prefix=${bd} \
    --enable-pic \
    --enable-vp8 \
    --enable-vp9 \
    --enable-static \
    --as=yasm
make 
make install
