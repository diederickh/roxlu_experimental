#!/bin/sh
pwd=${PWD}
dest=${pwd}/packaged_for_openframeworks
dest_of=${dest}/replace_openframeworks_libs

./package.sh

if [ -d ${dest} ] ; then
	rm -r ${dest}
fi

if [ ! -d ${dest} ] ; then
	mkdir ${dest}
fi

mkdir ${dest_of}
mkdir -p ${dest_of}/cairo/include
mkdir -p ${dest_of}/cairo/lib/osx/

mv packaged/lib/libcairo.a ${dest_of}/cairo/lib/osx/cairo.a
mv packaged/lib/libcairo-script-interpreter.a ${dest_of}/cairo/lib/osx/cairo-script-interpreter.a
mv packaged/lib/libpixman-1.a ${dest_of}/cairo/lib/osx/pixman-1.a

mv packaged/include/cairo ${dest_of}/cairo/include/
mv packaged/include/libpng15 ${dest_of}/cairo/include/
mv packaged/include/pixman-1 ${dest_of}/cairo/include/

cd ${dest_of}/cairo/include/
ln -s libpng15/png.h png.h
ln -s libpng15/pngconf.h pngconf.h
ln -s libpng15/pnglibconf.h pnglibconf.h

# remove libs which are already in OF
cd ${pwd}
rm -f packaged/lib/libpng.a
rm -f packaged/lib/libpng15.a
rm -f packaged/include/png.h
rm -f packaged/include/pngconf.h
rm -f packaged/include/pnglibconf.h

