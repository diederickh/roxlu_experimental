#!/bin/sh
builddir=${PWD}/build
installdir=${builddir}/install

if [ ! -d ${builddir} ] ; then 
    mkdir ${builddir}
fi

if [ ! -d ${installdir} ] ; then 
    mkdir ${installdir} 
fi

function check_file {
    if [ ! -f $1 ] ; then 
	echo "File not found: ${1}"
	exit
    fi
}

pwd=${PWD}
if [ "$1" = "reset" ]; then
    if [ ! -d download ] ; then
	mkdir download
    fi
    if [ ! -d build ] ; then 
	mkdir build
    fi
    cp download/*.tgz ${builddir}/
    
	# get sources
    if [ ! -f ${pwd}/download/pkgconfig.tgz ] ; then
	curl http://pkgconfig.freedesktop.org/releases/pkg-config-0.25.tar.gz -o download/pkgconfig.tgz
    fi
    
    if [ ! -f ${pwd}/download/libpng.tgz ] ; then
        curl -L -O "http://download.sourceforge.net/libpng/libpng-1.5.13.tar.gz"
        mv libpng-* ${pwd}/libpng.tgz
        mv libpng.tgz ${pwd}/download/
        cd ${pwd}
    fi
    
    if [ ! -f ${pwd}/download/pixman.tgz ] ; then
	curl http://www.cairographics.org/releases/pixman-0.24.4.tar.gz -o ${pwd}/download/pixman.tgz
    fi
    
    if [ ! -f ${pwd}/download/cairo.tgz ] ; then
	curl http://www.cairographics.org/releases/cairo-1.12.0.tar.gz -o ${pwd}/download/cairo.tgz
    fi
    
    cp download/*.tgz ${builddir}/
    cd ${builddir}
    
    # check if all files were downloaded
    check_file pkgconfig.tgz
    check_file libpng.tgz
    check_file pixman.tgz
    check_file cairo.tgz

    # extract 
    tar -zxvf pkgconfig.tgz
    tar -zxvf libpng.tgz
    tar -zxvf pixman.tgz
    tar -zxvf cairo.tgz
    
    mv pkg-config-* pkgconfig
    mv libpng-* libpng
    mv pixman-* pixman
    mv cairo-* cairo
fi

if [ "$1" = "compile" ] ; then
    
	# compile pkgconfig
    if [ "$2" = "pkgconfig" ] ; then 
	set -x
        cd ${builddir}/pkgconfig
	./configure --prefix=${installdir}
	make
        make install
    fi
	# export system vars
    export PKG_CONFIG=${installdir}/bin/pkg-config
    export PKG_CONFIG_PATH=${installdir}/lib/pkgconfig
    export CC="gcc-4.2"
    export LDFLAGS="-arch i386 -isysroot /Developer/SDKs/MacOSX10.6.sdk"
    export CFLAGS="-Os -arch i386 -isysroot /Developer/SDKs/MacOSX10.6.sdk"
    export PATH=${buildir}/bin:${PATH}
    
	# compile libpng
    if [ "$2" = "libpng" ] ; then
	cd ${builddir}/libpng
	./configure --prefix=${installdir} \
	    --disable-dependency-tracking \
	    --enable-static=yes
	make clean
	make 	
	make install
    fi
    
	#build pixman
    if [ "$2" = "pixman" ] ; then
        cd ${builddir}/pixman
        ./configure --prefix=${installdir} --disable-dependency-tracking --host=i386-apple-darwin10
        make clean
        make
        make install
    fi
    
	#build cairo
    if [ "$2" = "cairo" ] ; then 
	cd ${builddir}/cairo
	./configure --prefix=${installdir} \
	    --disable-xlib --disable-ft \
	    --disable-dependency-tracking \
	    --enable-png=yes \
	    --enable-svg=yes \
	    --enable-static=yes \
	    --build=i386-pc-darwin
	
	make clean
	make
	make install
    fi
fi


