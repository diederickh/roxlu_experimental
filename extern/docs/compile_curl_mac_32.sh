#!/bin/sh
# Used libcurl 7.28
d=${PWD}
bd=${d}/build
if [ ! -d ${bd} ] ; then 
    mkdir ${bd}
fi

cd curl
./configure --prefix=${bd} \
    --with-ssl=${bd} \
    --enable-static=yes \
    --enable-shared=no \
    --disable-ftp \
    --disable-ldaps \
    --disable-ldap \
    --disable-rtsp \
    --disable-dict \
    --disable-telnet \
    --disable-tftp \
    --disable-pop3 \
    --disable-imap \
    --disable-smtp \
    --disable-gopher \
    --without-axtls \
    --disable-ares \
    CFLAGS="-arch i386 -m32" 
make
make install
