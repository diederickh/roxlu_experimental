#!/bin/sh

# Compiling port audio on linux
# ------------------------------
#
# 
#

d=${PWD}
bd=${d}/build
audio_src="pa_stable_v19_20111121.tgz"

if [ ! -d ${bd} ] ; then 
    mkdir ${bd} 
fi

if [ ! -f ${audio_src} ] ; then
    echo "Download portaudio: ${audio_src}" 
    exit
fi

if [ ! -d "portaudio" ] ; then 
    tar -zxvf ${audio_src}
fi

cd portaudio
./configure --prefix=${bd} --enable-cxx
make 
make install
