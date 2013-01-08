#!/bin/bash
#  
# Compiles libogg, libvorbis, libtheora.




# Download the following file and put them in sources:
#
# - libogg-1.3.0.tar.gz
# - libtheora-1.1.1.tar.bz2
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
# ./[this_file] "ogg"
# ./[this_file] "vorbis"
# ./[this_file] "theora"

d=${PWD}
bd=${d}/build
sd=${d}/sources

ogg_src="libogg-1.3.0.tar.gz"
oggz_src="liboggz-1.1.1.tar.gz"
theora_src="libtheora-1.1.1.tar.bz2"
vorbis_src="libvorbis-1.3.3.tar.gz"

export PATH=${bd}:${PATH}
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
check_source ${ogg_src}
check_source ${theora_src}
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
    cp ${sd}/${theora_src} . && bunzip2 ${theora_src} && tar -xvf ${theora_src%.tar.bz2}.tar && rm ${theora_src%.tar.bz2}.tar && mv ${theora_src%.tar.bz2} theora
fi
if [ ! -d "vorbis" ] ; then
    cp ${sd}/${vorbis_src} . && tar -zxvf ${vorbis_src} && mv ${vorbis_src%.tar.gz} vorbis && rm ${vorbis_src}
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
    cd ${d}/theora
    ./configure --prefix=${bd} --enable-static
    make
    make install
fi

