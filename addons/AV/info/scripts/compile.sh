#!/bin/sh

# Author roxlu (www.roxlu.com
# Compiles gstreamer from source on Mac
# First compile all dependencies before the gstreamer plugins.
#
# Dependencies:
# -----------
# - gettext-0.18.1 ( http://ftp.gnu.org/pub/gnu/gettext/gettext-0.18.1.1.tar.gz )
# - glib-2.32.1 (ftp://ftp.gtk.org/pub/glib/2.32/glib-2.32.1.tar.xz )
# - libffi-3.0.11 ( ftp://sourceware.org/pub/libffi/libffi-3.0.11.tar.gz )
# - libxml2-2.8.0 ( ftp://xmlsoft.org/libxml2/libxml2-2.8.0.tar.gz )
# - pkg-config-0.18.1 ( http://pkgconfig.freedesktop.org/releases/pkg-config-0.18.1.tar.gz )
# - gstreamer-0.11.92 
# - gst-plugins-base-0.11.92
# - gst-plugins-good-0.11.92
# - gst-plugins-bad-0.11.92
# - gst-plugin-ugly-0.11.92
# - x264-snapshot-20120719-2245 ( http://www.videolan.org/developers/x264.html , download last )
# - yasm-1.2.0 ( http://www.tortall.net/projects/yasm/releases/yasm-1.2.0.tar.gz )
# - libtheora-1.1.1 ( http://downloads.xiph.org/releases/theora/libtheora-1.1.1.tar.bz2 )
# - libvorbis-1.3.3 ( http://downloads.xiph.org/releases/vorbis/libvorbis-1.3.3.tar.gz ) 
# - libogg-1.3.0 ( http://downloads.xiph.org/releases/ogg/libogg-1.3.0.tar.gz ) 
# - iconv ( http://ftp.gnu.org/pub/gnu/libiconv/libiconv-1.14.tar.gz )
# - libav ( git clone git://git.libav.org/libav.git && git reset --hard 65d94f63ca38b46a9d3719cb7d986d2daef90416 )
# - crtmpserver ( svn co --username anonymous --password "" https://svn.rtmpd.com/crtmpserver/branches/1.0 crtmpserver )

# Usage (how to compile for libav)
# --------------------------------
# - Download all sources, extract and create these directories
#	- libav
#	- libiconv
#	- gettext
#	- pkgconfig
#	- libffi
#	- libxml
#	- glib
#	- yasm
#	- libogg
#	- libvorbis
#	- libtheora
#	- x264
#
# - Compile with the following commands and in this order:
# 	./compile.sh iconv
#	./compile.sh gettext
#	./compile.sh pkgconfig
#	./compile.sh ffi
#	./compile.sh xml
#	./compile.sh glib	
#	./compile.sh yasm
#	./compile.sh ogg
#	./compile.sh vorbis
#	./compile.sh theora
#	./compile.sh x264
#	./compile.sh libav

# RTMP DUMP:
# ---------
# - Compiled manually, just calling "make" but had to change the "soname" in 
# the Makefile or librtmp. "Edit Makefile in ./librtmp and replace -soname 
# with -dylib_install_name"
# - Then I copied the directory to the "build" dir and created a 
# pkg-config file myself.
# - Had to compile rtmpdump w/o ssl/crypto
# 
# - rtmpdump 2.3, 
b=${PWD}
bd=${PWD}/build

export PKG_CONFIG=${bd}/bin/pkg-config
export PKG_CONFIG_PATH=${bd}/lib/pkgconfig
export PATH=${bd}/bin:${PATH}
export CXX="i686-apple-darwin10-llvm-g++-4.2"
export CC="i686-apple-darwin10-gcc-4.2.1"
export LDFLAGS="-L${bd}/lib/"
export CFLAGS="-Os -m32 -I${bd}/include/"
export CXXFLAGS=${CFLAGS}
export LIBFFI_LIBS="-lffi"
export LIBFFI_CFLAGS="-I${bd}/lib/libffi-3.0.11/include/"

echo "LDFLAGS: ${LDFLAGS}"
echo "CXXFLAGS: ${CXXFLAGS}"
echo "CFLAGS: ${CFLAGS}"
echo "PKGCONFIG: ${PKG_CONFIG}"
echo "PKG_CONFIG_PATH: ${PKG_CONFIG_PATH}"


# Compile a standard lib. 
# ${1} = directory
compilelib() 
{
	cd ${1}
	./configure --prefix=${bd}
	make
	make install
};


if [ "$1" = "iconv" ] ; then 
	cd libiconv
	./configure --prefix=${bd} \
		--disable-dtrace \
		--enable-static=yes \
		--with-gettext=${bd} \
		--without-iconv 
	make
	make install
fi

if [ "$1" = "gettext" ] ; then 
	cd gettext
	./configure --prefix=${bd} --enable-static=yes
	make
	make install
fi

if [ "$1" = "pkgconfig" ] ; then 
	cd pkgconfig
	./configure --prefix=${bd} --with-internal-glib 
	make 
	make install
fi

if [ "$1" = "ffi" ] ; then 
	cd libffi
	./configure --prefix=${bd} --enable-static=yes
	cd i386-apple-darwin10.8.0 
	make clean
	make
	make install
fi

if [ "$1" = "xml" ] ; then
	cd libxml
	./configure 	--prefix=${bd} --enable-static=yes 
	make
	make install
fi

if [ "$1" = "glib" ] ; then
	set -x
	cd glib
	./configure  --prefix=${bd} --enable-static=yes --includedir=${bd}/include
	make 
	make install
fi


if [ "$1" = "gstreamer" ] ; then 
	cd gstreamer
	./configure --prefix=${bd} --enable-static=yes
	make
	make install	
fi

if [ "$1" = "gstbase" ] ; then
	# see https://bugzilla.gnome.org/show_bug.cgi?id=660302 
	export CFLAGS="-O0 -m32 -I${bd}/include/" 
	cd gst-plugins-base
	./configure --prefix=${bd} --enable-static=yes --disable-debug
	make 
	make install
fi

if [ "$1" = "gstgood" ] ; then
	cd gst-plugins-good
	./configure --prefix=${bd} --enable-static=yes 
	make 
	make install
fi


if [ "$1" = "gstbad" ] ; then
	export CFLAGS="-Os -m32 -I${bd}/include/ -I${bd}/include/rtmpdump/"
	export LDFLAGS="-lpthread -lssl"
	cd gst-plugins-bad
	./configure --prefix=${bd} --enable-static=yes 
	make 
	make install
fi


if [ "$1" = "gstugly" ] ; then
	cd gst-plugins-ugly
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


if [ "$1" = "ogg" ] ; then
	compilelib libogg
fi


if [ "$1" = "vorbis" ] ; then
	compilelib libvorbis
fi

if [ "$1" = "theora" ] ; then
	compilelib libtheora
fi

if [ "$1" = "png" ] ; then
	compilelib libpng
fi


# I got a common.mak error with lib-av which means it's not configured. 
# I went into the libav directory and ran ./configure --disable-yasm, by hand
# then ran this script again and it compiled.
if [ "$1" = "gstav" ] ; then
	
	
	cd gst-libav
	cd gst-libs/ext/libav
	./configure --prefix=${bd} --enable-cross-compile \
			--arch=x86_32 \
			--disable-hwaccels \
			--disable-avserver target-os=darwin cc='/Developer/usr/bin/cc -m32 '  
	make
	#VideoDecodeAcceleration
	#compilelib gst-libav
fi

if [ "$1" = "mpeg" ] ; then
	cd gst-plugins-bad/gst/mpegtsmux
	./configure 
	make
	
fi

if [ "$1" = "libav" ] ; then
	cd libav
	./configure --prefix=${bd} --enable-cross-compile \
			--arch=x86_32 \
			--disable-hwaccels \
			--enable-libx264 \
			--enable-libtheora \
			--enable-gpl \
			--enable-shared \
			--disable-avserver target-os=darwin cc='/Developer/usr/bin/cc -m32 '
	make clean
	make
	make install
fi



# Edit Makefile in ./librtmp and replace -soname with -dylib_install_name
#if [ "$1" = "rtmp" ] ; then
#	compilelib rtmpdump
# compiled by just executing make
#fi

# First compile yasm
if [ "$1" = "x264" ] ; then
	cd x264
	./configure --prefix=${bd} --exec-prefix=${bd} --enable-static --enable-shared  
	make 
	make install
fi

# not gstreamer related!!
if [ "$1" = "irssi" ] ; then
	cd irssi
	./configure --prefix=${bd}
	make
	make install
fi

