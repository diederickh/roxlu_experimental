#!/bin/bash

# COMPILING PANGO WITH CAIRO
# --------------------------
# This script compiles pango + cairo on Mac OSX 10.8.2 with all 
# dependencies. This script has been tested only on this mac, but should
# work on 10.6.x too.
#
# HOW TO COMPILE
# --------------
# - Create a directores "sources" and download the sources (see list below)
# - Run pangocairo.sh once to extract all sources and set everything in place
# - Run pangocairo.sh several times in this order:
#    $ ./pangocairo.sh ffi
#    $ ./pangocairo.sh iconv
#    $ ./pangocairo.sh libtool
#    $ ./pangocairo.sh autoconf
#    $ ./pangocairo.sh automake
#    $ ./pangocairo.sh gettext
#    $ ./pangocairo.sh glib
#    $ ./pangocairo.sh pkgconfig
#    $ ./pangocairo.sh png
#    $ ./pangocairo.sh jpg
#    $ ./pangocairo.sh pixman
#    $ ./pangocairo.sh freetype
#    $ ./pangocairo.sh fontconfig
#    $ ./pangocairo.sh poppler
#    $ ./pangocairo.sh harfbuzz
#    $ ./pangocairo.sh cairo
#    $ ./pangocairo.sh pango
#

bd=${PWD}/build
sd=${PWD}/sources
pwd=${PWD}

# SOURCES:
pkgconfig_src="pkg-config-0.27.tar.gz"
png_src="libpng-1.5.13.tar.gz"
pixman_src="pixman-0.28.2.tar.gz"
cairo_src="cairo-1.12.8.tar.xz"
gettext_src="gettext-0.18.2.tar.gz"
iconv_src="libiconv-1.14.tar.gz"
glib_src="glib-2.34.3.tar.xz"
ffi_src="libffi-3.0.11.tar.gz"
libtool_src="libtool-2.4.2.tar.gz"
jpg_src="jpegsrc.v8d.tar.gz"
svg_src="librsvg-2.36.4.tar.xz"
fontconfig_src="fontconfig-2.10.2.tar.gz"
poppler_src="poppler-0.20.5.tar.gz"
freetype_src="freetype-2.4.11.tar.gz"
pango_src="pango-1.32.5.tar.xz"
harfbuzz_src="harfbuzz-0.9.9.tar.bz2"
autoconf_src="autoconf-2.69.tar.gz"
automake_src="automake-1.12.6.tar.gz"

if [ ! -d ${bd} ] ; then 
    mkdir ${bd}
fi

found_sources=1
function check_source {
    if [ ! -f ${sd}/${1} ] ; then 
	echo "File not found: ${1}"
        found_sources=0
    else 
        echo "Found: ${1}"
    fi
}

# CHECK IF WE CAN FIND ALL DEPENDENCIES
check_source ${pkgconfig_src}
check_source ${png_src}
check_source ${pixman_src}
check_source ${cairo_src}
check_source ${gettext_src}
check_source ${iconv_src}
check_source ${glib_src}
check_source ${ffi_src}
check_source ${libtool_src}
check_source ${jpg_src}
check_source ${svg_src}
check_source ${fontconfig_src}
check_source ${poppler_src}
check_source ${freetype_src}
check_source ${pango_src}
check_source ${harfbuzz_src}
check_source ${autoconf_src}
check_source ${automake_src}

if [ ${found_sources} = 0 ] ; then 
    echo "Downlaod all dependencies into sources/" 
    exit
fi

# EXTRACT ALL SOURCES
if [ ! -d "pkgconfig" ] ; then
    cp ${sd}/${pkgconfig_src} . && tar -zxvf ${pkgconfig_src} && mv ${pkgconfig_src%.tar.gz} pkgconfig && rm ${pkgconfig_src}
fi
if [ ! -d "png" ] ; then
    cp ${sd}/${png_src} . && tar -zxvf ${png_src} && mv ${png_src%.tar.gz} png && rm ${png_src}
fi
if [ ! -d "pixman" ] ; then
    cp ${sd}/${pixman_src} . && tar -zxvf ${pixman_src} && mv ${pixman_src%.tar.gz} pixman && rm ${pixman_src}
fi
if [ ! -d "cairo" ] ; then
    cp ${sd}/${cairo_src} . && xz -d ${cairo_src} && tar -xvf ${cairo_src%.xz} && mv ${cairo_src%.tar.xz} cairo && rm ${cairo_src%.tar.xz}.tar
fi
if [ ! -d "gettext" ] ; then
    cp ${sd}/${gettext_src} . && tar -zxvf ${gettext_src} && mv ${gettext_src%.tar.gz} gettext && rm ${gettext_src}
fi
if [ ! -d "iconv" ] ; then 
    cp ${sd}/${iconv_src} . && tar -zxvf ${iconv_src} && mv ${iconv_src%.tar.gz} iconv && rm ${iconv_src}
fi
if [ ! -d "glib" ] ; then 
    cp ${sd}/${glib_src} . && xz -d ${glib_src} && tar -xvf ${glib_src%.xz} && mv ${glib_src%.tar.xz} glib && rm ${glib_src%.tar.xz}.tar 
fi
if [ ! -d "ffi" ] ; then 
    cp ${sd}/${ffi_src} . && tar -zxvf ${ffi_src} && mv ${ffi_src%.tar.gz} ffi && rm ${ffi_src}
fi
if [ ! -d "libtool" ] ; then
    cp ${sd}/${libtool_src} . && tar -zxvf ${libtool_src} && mv ${libtool_src%.tar.gz} libtool && rm ${libtool_src}
fi
if [ ! -d "jpg" ] ; then
    cp ${sd}/${jpg_src} . && tar -zxvf ${jpg_src} && mv jpeg-8d jpg && rm ${jpg_src}
fi
if [ ! -d "svg" ] ; then 
    cp ${sd}/${svg_src} . && xz -d ${svg_src} && tar -xvf ${svg_src%.xz} && mv ${svg_src%.tar.xz} svg && rm ${svg_src%.tar.xz}.tar 
fi
if [ ! -d "poppler" ] ; then
    cp ${sd}/${poppler_src} . && tar -zxvf ${poppler_src} && mv ${poppler_src%.tar.gz} poppler  && rm ${poppler_src}
fi 
if [ ! -d "fontconfig" ] ; then
    cp ${sd}/${fontconfig_src} . && tar -zxvf ${fontconfig_src} && mv ${fontconfig_src%.tar.gz} fontconfig  && rm ${fontconfig_src}
fi 
if [ ! -d "freetype" ] ; then
    cp ${sd}/${freetype_src} . && tar -zxvf ${freetype_src} && mv ${freetype_src%.tar.gz} freetype  && rm ${freetype_src}
fi 
if [ ! -d "pango" ] ; then 
    cp ${sd}/${pango_src} . && xz -d ${pango_src} && tar -xvf ${pango_src%.xz} && mv ${pango_src%.tar.xz} pango && rm ${pango_src%.tar.xz}.tar 
fi
if [ ! -d "harfbuzz" ] ; then 
    cp ${sd}/${harfbuzz_src} . && bunzip2 ${harfbuzz_src} && tar -xvf ${harfbuzz_src%.tar.bz2}.tar && mv ${harfbuzz_src%.tar.bz2} harfbuzz && rm ${harfbuzz_src%.tar.bz2}.tar
fi
if [ ! -d "autoconf" ] ; then
    cp ${sd}/${autoconf_src} . && tar -zxvf ${autoconf_src} && mv ${autoconf_src%.tar.gz} autoconf  && rm ${autoconf_src}
fi 
if [ ! -d "automake" ] ; then
    cp ${sd}/${automake_src} . && tar -zxvf ${automake_src} && mv ${automake_src%.tar.gz} automake  && rm ${automake_src}
fi 

# MAKE SURE WE'RE USING OUR LOCAL BUILD DIRECTORIES
export CFLAGS="-I/${bd}/include/"
export CPPFLAGS=${CFLAGS}
export LDFLAGS="-L${bd}/lib/"
curr_path=${PATH}
export PATH=${bd}/bin:${PATH}
new_path=${PATH}
export LIBFFI_LIBS="-L${bd}/lib/ -lffi"
export LIBFFI_CFLAGS="-I${bd}/include/ -I${bd}/lib/libffi-3.0.11/include/"

# ffi 
if [ "$1" = "ffi" ] ; then 
    set -x
    cd ${pwd}/ffi
    ./configure --prefix=${bd} \
        --enable-static=yes
    make
    make install
fi

# iconv
if [ "$1" = "iconv" ] ; then 
    set -x
    cd ${pwd}/iconv
    ./configure --prefix=${bd} \
        --enable-static=yes \
        --enable-shared=yes
    make
    make install
fi

# libtool 
if [ "$1" = "libtool" ] ; then 
    set -x
    cd ${pwd}/libtool
    ./configure --prefix=${bd} \
        --enable-static=yes
    make
    make install
fi

# autoconf
if [ "$1" = "autoconf" ] ; then 
    set -x
    cd ${pwd}/autoconf
    ./configure --prefix=${bd} \
	--enable-static=yes 
    make clean
    make 	
    make install
fi

# automake
if [ "$1" = "automake" ] ; then 
    set -x
    cd ${pwd}/automake
    ./configure --prefix=${bd} \
	--enable-static=yes 
    make clean
    make 	
    make install
fi

# gettext
if [ "$1" = "gettext" ] ; then 
    set -x
    cd ${pwd}/gettext
    ./configure --prefix=${bd}
    make
    make install
fi

# glib
if [ "$1" = "glib" ] ; then 
    set -x
    cd ${pwd}/glib
    ./configure --prefix=${bd} \
        --enable-static=yes \
        --disable-selinux \
        --disable-modular-tests 
    make clean
    make V=1
    make install
fi

# pkgconfig
if [ "$1" = "pkgconfig" ] ; then 
    export LDFLAGS="${LDFLAGS} -framework Carbon"
    export GLIB_CFLAGS="-I${bd}/include/ -I${bd}/include/glib-2.0/ -I${bd}/lib/glib-2.0/include/"
    export GLIB_LIBS="-lglib-2.0 -lintl -liconv"
    set -x
    cd ${pwd}/pkgconfig
    ./configure --prefix=${bd} 
    make clean
    make V=1
    make install
fi

# png
if [ "$1" = "png" ] ; then 
    export PATH=${curr_path}
    cd ${pwd}/png
    ./autogen.sh
    ./configure --prefix=${bd} \
	--enable-static=yes 
    make clean
    make 	
    make install
fi

# jpg
if [ "$1" = "jpg" ] ; then 
    cd ${pwd}/jpg
    ./configure --prefix=${bd} \
	--enable-static=yes 
    make clean
    make 	
    make install
fi

# pixman
if [ "$1" = "pixman" ] ; then 
    export PKG_CONFIG=${bd}/bin/pkg-config
    export PKG_CONFIG_PATH=${bd}/lib/pkgconfig
    cd ${pwd}/pixman
    ./autogen.sh
    ./configure --prefix=${bd} \
	--enable-static=yes 
    make clean
    make 	
    make install
fi

# freetype
if [ "$1" = "freetype" ] ; then 
    export PKG_CONFIG=${bd}/bin/pkg-config
    export PKG_CONFIG_PATH=${bd}/lib/pkgconfig
    cd ${pwd}/freetype
    ./configure --prefix=${bd} \
	--enable-static=yes
    make clean
    make 	
    make install
fi

# fontconfig
if [ "$1" = "fontconfig" ] ; then 
    export PKG_CONFIG=${bd}/bin/pkg-config
    export PKG_CONFIG_PATH=${bd}/lib/pkgconfig
    cd ${pwd}/fontconfig
    ./configure --prefix=${bd} \
	--enable-static=yes 
    make clean
    make 	
    make install
fi

# poppler
if [ "$1" = "poppler" ] ; then 
    export FONTCONFIG_CFLAGS="-I${bd}/include/"
    export FONTCONFIG_LIBS="-lfontconfig"
    cd ${pwd}/poppler
    ./configure --prefix=${bd} \
	--enable-static=yes 
    make clean
    make 	
    make install
fi

# harfbuzz
if [ "$1" = "harfbuzz" ] ; then 
    cd ${pwd}/harfbuzz
    ./configure --prefix=${bd} \
	--enable-static=yes 
    make clean
    make 	
    make install
fi

# cairo
if [ "$1" = "cairo" ] ; then
    # we must disable-ft else pango can't load fonts somehow
    cd ${pwd}/cairo
    export PKG_CONFIG=${bd}/bin/pkg-config
    export PKG_CONFIG_PATH=${bd}/lib/pkgconfig
    export pixman_CFLAGS="-I/${bd}/include/pixman-1/"
    export pixman_LIBS="-lpixman-1"
    export POPPLER_CFLAGS="-I${bd}/include/"
    export POPPLER_LIBS="-lpoppler"
    export FREETYPE_CFLAGS="-I${bd}/include/freetype2/"
    export FREETYPE_LIBS="-lfreetype"
    export LIBS=" -framework CoreFoundation -framework ApplicationServices"
    ./configure --prefix=${bd} \
	--disable-xlib \
        --disable-ft \
	--disable-dependency-tracking \
	--enable-png=yes \
	--enable-svg=no \
	--enable-static=yes 
    make clean
    make
    make install
fi

# pango
if [ "$1" = "pango" ] ; then 
    export PKG_CONFIG=${bd}/bin/pkg-config
    export PKG_CONFIG_PATH=${bd}/lib/pkgconfig
    export LIBS=" -framework CoreFoundation -framework ApplicationServices"
    export pixman_CFLAGS="-I/${bd}/include/pixman-1/"
    export pixman_LIBS="-lpixman-1"

    cd ${pwd}/pango
    ./configure --prefix=${bd} \
	--enable-static=yes
    make clean
    make 	
    make install
fi
