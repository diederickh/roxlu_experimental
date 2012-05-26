#!/bin/sh

# interesting read: 
# http://x11.gp2x.de/personal/google/
# http://cocoawithlove.com/2009/09/building-for-earlier-os-versions-in.html
# make sure you compile against 10.5 in xcode
# example on using pango_layout: http://paps.sourceforge.net/doxygen-html/
# http://x11.gp2x.de/personal/google/ # examples!!	
# http://paps.sourceforge.net/doxygen-html/
# reset from top to bottom then
# compile from top to bottom.

builddir=${PWD}/build
installdir=${builddir}/install
downloaddir=${PWD}/download

if [ ! -d ${downloaddir} ] ; then 
	mkdir ${downloaddir}
fi
if [ ! -d ${builddir} ] ; then
	mkdir ${builddir}
fi

if [ ! -d ${installdir} ] ; then 
	mkdir ${installdir}
fi

if [ "$1" = "reset" ]; then
	#libiconv
	if [ "$2" = "iconv" ]; then 
		rm -r ${builddir}/gettext
		if [ ! -f download/iconv.tgz ] ; then 
			curl http://ftp.gnu.org/pub/gnu/libiconv/libiconv-1.14.tar.gz -o iconv.tgz
			cp iconv.tgz download/
		fi
		
		cp download/iconv.tgz ${builddir}
		
		# extract and rename dirs.
		cd ${builddir}
		tar -zxvf iconv.tgz
		mv libiconv-* iconv
	fi
	
	#gettext
	if [ "$2" = "gettext" ]; then 
		rm -r ${builddir}/gettext
		if [ ! -f download/gettext.tgz ] ; then 
			curl http://ftp.gnu.org/pub/gnu/gettext/gettext-0.18.1.1.tar.gz -o gettext.tgz
			cp gettext.tgz download/
		fi
		
		cp download/gettext.tgz ${builddir}
		
		# extract and rename dirs.
		cd ${builddir}
		tar -zxvf gettext.tgz
		mv gettext-* gettext
	fi

	#glib
	if [ "$2" = "glib" ]; then
		if [ -d ${builddir}/glib ] ; then
			rm -r ${builddir}/glib
		fi
		if [ ! -f download/glib.tgz ] ; then
			curl ftp://ftp.gtk.org/pub/glib/2.31/glib-2.31.2.tar.bz2 -o glib.bz2
			cp glib.bz2 ${builddir}
		fi
		
		# extract and rename dirs.
		cd ${builddir}
		bunzip2 glib.bz2
		mv glib glib.tar.gz
		tar -zxvf glib.tar.gz
		mv glib-* glib
	fi
	
	#pango
	if [ "$2" = "pango" ]; then 
		if [ -d ${builddir}/pango ] ; then
			rm -r ${builddir}/pango
		fi
		
		if [ ! -f download/pango.tgz ] ; then
			curl ftp://ftp.gnome.org/pub/GNOME/sources/pango/1.29/pango-1.29.5.tar.bz2 -o download/pango.bz2
			cp download/pango.bz2 ${builddir}/pango.bz2
		fi
		
		# extract and rename dirs.
		cd ${builddir}
		bunzip2 pango.bz2
		mv pango pango.tar.gz
		tar -zxvf pango.tar.gz
		mv pango-* pango
	fi
	
	#ffi 
	if [ "$2" = "ffi" ] ; then
		echo "@todo - downlaod from github!"
		if [ ! -d ${builddir}/libffi ] ; then
			cd ${builddir}
			mkdir libffi
			cd libffi
			git clone https://github.com/atgreen/libffi.git .
			git reset --hard 8360bf1cd0aba8db5582266da70467de7e89a57a
		fi
	fi
	
	#xml
	if [ "$2" = "xml" ] ; then 
		if [ ! -f ${builddir}/libxml.tar.gz ] ; then 
			curl ftp://xmlsoft.org/libxml2/libxml2-2.7.8.tar.gz -o libxml.tar.gz
			cp libxml.tar.gz ${builddir}
		fi
		cd ${builddir}
		tar -zxvf libxml.tar.gz
		mv libxml2-2.7.8 libxml
	fi

	# art
	if [ "$2" = "art" ] ; then 
		if [ ! -f ${builddir}/libart.tar.gz ] ; then 
			curl http://ftp.gnome.org/pub/gnome/sources/libart_lgpl/2.3/libart_lgpl-2.3.20.tar.gz -o libart.tar.gz
			mv libart.tar.gz ${builddir}
		fi
		cd ${builddir}
		tar -zxvf libart.tar.gz
		mv libart_lgpl-2.3.20 libart
	fi

	# svg
	if [ "$2" = "svg" ] ; then
		if [ ! -d ${builddir}/librsvg ] ; then
			curl http://ftp.gnome.org/pub/GNOME/sources/librsvg/2.35/librsvg-2.35.0.tar.bz2 -o librsvg.tar.bz2
			mv librsvg.tar.bz2 ${builddir}
		fi
		cd ${builddir}
		bunzip2 librsvg.tar.bz2
		tar -xvf librsvg.tar
		mv librsvg-2.35.0 librsvg
	fi
fi

if [ "$1" = "compile" ] ; then
	export PKG_CONFIG=${installdir}/bin/pkg-config
	export PKG_CONFIG_PATH=${installdir}/lib/pkgconfig
#	if [ "$2" = "pango" ] ; then 
		export LDFLAGS="-arch i386 -isysroot /Developer/SDKs/MacOSX10.6.sdk"
		export CFLAGS="-Os -arch i386 -isysroot /Developer/SDKs/MacOSX10.6.sdk"
#	else
#		export LDFLAGS="-arch i386 -isysroot /Developer/SDKs/MacOSX10.6.sdk"
#		export CFLAGS="-Os -arch i386 -isysroot /Developer/SDKs/MacOSX10.6.sdk"
#	fi
	
	# note that I could not compile gettext with -arch ppc, after I compiled gettext
	# i removed the -arch ppc again, and continued
	export PATH=${builddir}/bin:${builddir}/lib:${PATH}
	
	# lib art
	if [ "$2" = "art" ] ; then
	   	export LDFLAGS="-arch i386 -L${builddir}/lib -isysroot /Developer/SDKs/MacOSX10.6.sdk"
                export CFLAGS="-m32 -Os -I${builddir}/include/ -L${builddir}/lib/ -isysroot /Developer/SDKs/MacOSX10.6.sdk"
                export CPPFLAGS="-arch i386 -Os -I${builddir}/include"
                export CC="gcc -m32     "
		cd ${builddir}/libart
		./configure --prefix=${installdir} --enable-static=yes --enable-shared=no
		make clean
		make 
		make install
	fi

	# libxml
	if [ "$2" = "xml" ] ; then 
		cd ${builddir}/libxml
		export LDFLAGS="-arch i386 -L${builddir}/lib -isysroot /Developer/SDKs/MacOSX10.6.sdk"
                export CFLAGS="-m32 -Os -I${builddir}/include/ -L${builddir}/lib/ -isysroot /Developer/SDKs/MacOSX10.6.sdk"
                export CPPFLAGS="-arch i386 -Os -I${builddir}/include"
                export CC="gcc -m32     "
		./configure --prefix=${installdir} --enable-static=yes --enable-shared=no
		make clean
		make		
		make install
	fi

	if [ "$2" = "svg" ] ; then 
	        cd ${builddir}/librsvg
		export LDFLAGS="-arch i386 -L${builddir}/lib -isysroot /Developer/SDKs/MacOSX10.6.sdk"
                export CFLAGS="-m32 -Os -I${builddir}/include/ -L${builddir}/lib/ -isysroot /Developer/SDKs/MacOSX10.6.sdk"
                export CPPFLAGS="-arch i386 -Os -I${builddir}/include"
                export CC="gcc -m32     "
                ./configure     --prefix=${installdir} \
				--libdir=${installdir}/lib \
				--includedir=${installdir}/include \
				--disable-pixbuf-loader \
				--disable-gtk-theme \
				--disable-tools \
				--enable-static=yes \
				--enable-shared=no

                make clean
                make            
                make install	
	fi

	#compile gettext
	if [ "$2" = "gettext" ]; then
		cd ${builddir}/gettext
		./configure --prefix=${installdir} \
					--disable-dependency-tracking \
					--disable-java \
					--enable-status=yes \
					--disable-rpah \
					--disable-libasprintf \
					--enable-static=yes \
					--enable-shared=no
		make clean
		make 	
		make install
	fi
	
	# compile iconv
	if [ "$2" = "iconv" ]; then
		cd ${builddir}/iconv
		./configure --prefix=${installdir} --disable-dependency-tracking  --enable-static=yes --enable-shared=no
		make clean
		make 	
		make install
	fi
	
	#compile ffi
	if [ "$2" = "ffi" ] ; then

		# The compilation of libffi is broken!!
		# I'm using the system libffi paths (see the glib compile part)
		# YOU NEED TO USE THE GIT VERSION	
		set -x
		export LDFLAGS="-arch i386 -L${installdir}/lib -isysroot /Developer/SDKs/MacOSX10.6.sdk"
		export CFLAGS="-m32 -Os -I${installdir}/include/ -L${installdir}/lib/ -isysroot /Developer/SDKs/MacOSX10.6.sdk"
		export CPPFLAGS="-arch i386 -Os -I${installdir}/include"
		export CC="gcc -m32 	"
		cd ${builddir}/libffi
		./configure --prefix=${installdir} --enable-static=yes --enable-shared=no
		make clean
		make
		make install
		
		# IMPORTANT
		# -----------
		# After compiling you need to copy the libffi/i386-apple-darwin10.8.0/libffi.pc 
		# file to lib/pkgconfig, also copy the include dirs. Also copy the files 
		# from libffi/src/x86/ to the builddir include dir.
	fi
	#compile glib
	if [ "$2" = "glib" ]; then
		set -x
		export PATH=${installdir}/include:${installdir}/bin:${installdir}/lib:${PATH}
		export LDFLAGS="-arch i386 -L${installdir}/lib "
		export CFLAGS="-m32 -Os -I${installdir}/include/ -L${installdir}/lib/ "
		export CPPFLAGS="-arch i386 -Os -I${installdir}/include "
		#export LIBFFI_LIBS="/usr/lib/"
		#export LIBFFI_CFLAGS="-I/usr/include/ffi/"
		export LIBFFI_LIBS="-L${installdir}/lib/"
		export LIBFFI_CFLAGS="-I${installdir}/include/ -I${installdir}/lib/libffi-3.0.11-rc2/include/"
		cd ${builddir}/glib
		./configure --prefix=${installdir} \
			--disable-dependency-tracking \
			--disable-dtrace  \
			--enable-static=yes \
			--enable-shared=yes
			#--enable-static=yes
			#--with-libiconv=${installdir}/lib \
		make clean
		make 	
		make install
	fi
	
	# compile pango
	if [ "$2" = "pango" ]; then
		set -x
		oflibs="/Users/diederickhuijbers/Documents/programming/c++/of/0071/libs"
		cairo_inc_path="${oflibs}/cairo/include/"
		cairo_lib_path="${oflibs}/cairo/lib/osx/"
	 	export PATH=${installdir}/bin:${installdir}/lib:${PATH}
                export LDFLAGS="-arch i386 -L${installdir}/lib "
                export CFLAGS="-m32 -Os -I${installdir}/include/ -L${installdir}/lib/"
                export CPPFLAGS="-arch i386 -Os -I${installdir}/include "
                export LIBFFI_LIBS="/usr/lib/"
                export LIBFFI_CFLAGS="-I/usr/include/ffi/"		
		#export CC="gcc-4.0"
		cd ${builddir}/pango
		./configure --prefix=${installdir} \
			--disable-dependency-tracking \
			--with-included-modules=yes \
			--enable-static=yes \
			--enable-shared=no \
			--build=i386-darwin
			GLIB_LIBS=${installdir}/lib/
		make clean
		make 	
		make install
	fi
	
fi


