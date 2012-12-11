#!/bin/sh
# Used libcurl 7.28
d=${PWD}
bd=${d}/build
if [ ! -d build_curl ] ; then 
    mkdir build_curl
fi

#cd build
#cmake -DCMAKE_INSTALL_PREFIX=${d}/build/ \
#    -DCURL_STATICLIB=ON \
#    ../curl/ 
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
    --disable-ares 
make
make install
