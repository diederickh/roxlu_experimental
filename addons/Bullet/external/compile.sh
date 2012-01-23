#!/bin/sh
if [ ! -d compiled ] ; then 
	mkdir compiled
	mkdir installed
fi

id=${PWD}/installed
bd=${PWD}/compiled
cd compiled
cmake -DCMAKE_INSTALL_PREFIX=${id} -DCMAKE_OSX_ARCHITECTURES='i386'  ../
make
