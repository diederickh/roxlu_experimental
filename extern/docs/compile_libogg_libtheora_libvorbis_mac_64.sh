#!/bin/bash
#  
# Compiles libogg, libvorbis, libtheora.
# Download the following file and put them in sources:
#
# - libogg-1.3.0.tar.gz
# - libtheora from svn, revision: 18763
# - libvorbis-1.3.3.tar.gz
# - liboggz-1.1.1.tar.gz (used to test encoding)
#
# VERSIONS:
# ---------
# 0.0.0 // 2013.01.04 - First version
#
# Compile:
# --------
# ./[this_file]
# ./[this_file] "libtool"
# ./[this_file] "autoconf"
# ./[this_file] "automake"
# ./[this_file] "ogg"
# ./[this_file] "vorbis"
# ./[this_file] "theora"

d=${PWD}
bd=${d}/build
sd=${d}/sources


libtool_src="libtool-2.4.2.tar.gz"
autoconf_src="autoconf-2.69.tar.gz"
automake_src="automake-1.12.6.tar.gz"
ogg_src="libogg-1.3.0.tar.gz"
oggz_src="liboggz-1.1.1.tar.gz"
vorbis_src="libvorbis-1.3.3.tar.gz"


export PATH=${bd}:${bd}/bin:${PATH}
export CFLAGS="-I${bd}/include"
export CXXFLAGS=${CFLAGS}
export LDFLAGS="-L${bd}/lib"
export PKG_CONFIG_PATH="${bd}/lib/pkgconfig/:${PKG_CONFIG_PATH}"

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
check_source ${autoconf_src}
check_source ${libtool_src}
check_source ${automake_src}
check_source ${ogg_src}
check_source ${vorbis_src}
check_source ${oggz_src}

if [ ${found_sources} = 0 ] ; then
    echo "First download all sources" 
    exit
fi

if [ ! -d "ogg" ] ; then 
    cp ${sd}/${ogg_src} . && tar -zxvf ${ogg_src} && mv ${ogg_src%.tar.gz} ogg && rm ${ogg_src}
fi
if [ ! -d "oggz" ] ; then 
    cp ${sd}/${oggz_src} . && tar -zxvf ${oggz_src} && mv ${oggz_src%.tar.gz} oggz && rm ${oggz_src}
fi
if [ ! -d "theora" ] ; then 
    mkdir theora
    cd theora
    svn co -r 18763 http://svn.xiph.org/trunk/theora .
fi
if [ ! -d "vorbis" ] ; then
    cp ${sd}/${vorbis_src} . && tar -zxvf ${vorbis_src} && mv ${vorbis_src%.tar.gz} vorbis && rm ${vorbis_src}
fi
if [ ! -d "libtool" ] ; then
    cp ${sd}/${libtool_src} . && tar -zxvf ${libtool_src} && mv ${libtool_src%.tar.gz} libtool && rm ${libtool_src}
fi
if [ ! -d "autoconf" ] ; then
    cp ${sd}/${autoconf_src} . && tar -zxvf ${autoconf_src} && mv ${autoconf_src%.tar.gz} autoconf  && rm ${autoconf_src}
fi 
if [ ! -d "automake" ] ; then
    cp ${sd}/${automake_src} . && tar -zxvf ${automake_src} && mv ${automake_src%.tar.gz} automake  && rm ${automake_src}
fi 

# libtool 
if [ "$1" = "libtool" ] ; then 
    set -x
    cd ${d}/libtool
    ./configure --prefix=${bd} \
        --enable-static=yes
    make
    make install
fi

# autoconf
if [ "$1" = "autoconf" ] ; then 
    set -x
    cd ${d}/autoconf
    ./configure --prefix=${bd} \
        --enable-static=yes 
    make clean
    make 
    make install
fi

# automake
if [ "$1" = "automake" ] ; then 
    set -x
    cd ${d}/automake
    ./configure --prefix=${bd} \
        --enable-static=yes 
    make clean
    make 
    make install
fi

if [ "$1" = "ogg" ] ; then 
    cd ${d}/ogg
    ./configure --prefix=${bd} --enable-static
    make
    make install
fi

if [ "$1" = "oggz" ] ; then 
    cd ${d}/oggz
    ./configure --prefix=${bd} --enable-static
    make
    make install
fi

if [ "$1" = "vorbis" ] ; then 
    cd ${d}/vorbis
    ./configure --prefix=${bd} --enable-static
    make
    make install
fi

if [ "$1" = "theora" ] ; then 
    # I had to disable-asm as there seems to be a bug with clang and asm.
    # when this is fixed we should enable it again.
    cd ${d}/theora
    ./autogen.sh
    ./configure --prefix=${bd} --enable-static --disable-asm
    make
    make install
fi

