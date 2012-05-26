#!/bin/sh
d=${PWD}
packagedir=${PWD}/packaged
if [ -d ${packagedir} ] ; then
	rm -r ${packagedir}
fi

installdir=${PWD}/build/install/
mkdir ${packagedir}

cd ${packagedir}
mkdir include
mkdir lib

cd ${d}
cp ${installdir}/include/*.h ${packagedir}/include/
cp -r ${installdir}/include/glib-2.0/ ${packagedir}/include/
cp -r ${installdir}/include/cairo ${packagedir}/include/
cp -r ${installdir}/include/gio-unix-2.0/ ${packagedir}/include/
cp -r ${installdir}/include/libart-2.0/ ${packagedir}/include/
cp -r ${installdir}/include/libpng15 ${packagedir}/include/
cp -r ${installdir}/include/libxml2/ ${packagedir}/include/
cp -r ${installdir}/include/pango-1.0/ ${packagedir}/include/
cp -r ${installdir}/include/pixman-1 ${packagedir}/include/

cp ${installdir}/lib/*.a ${packagedir}/lib/
cp ${installdir}/lib/glib-2.0/include/* ${packagedir}/include/
cp ${installdir}/lib/libffi-3.0.11-rc2/include/* ${packagedir}/include/
