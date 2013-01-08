#!/bin/sh
d=${PWD}
./compile.sh
cd ${d}/build
make install
${d}/bin/portaudio
