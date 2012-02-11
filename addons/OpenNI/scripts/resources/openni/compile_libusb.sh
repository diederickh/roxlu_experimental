#!/bin/sh
d=${PWD}/build
if [ ! -d ${d} ] ; then 
	mkdir ${d}
fi
./autogen.sh
./configure LDFLAGS='-framework IOKit -framework CoreFoundation -arch i386 -arch x86_64 ' CFLAGS='-arch i386 -arch x86_64' --disable-dependency-tracking --prefix=${d}
make 
make install


