#!/bin/sh
if [ -d build ] ; then 
    cd build 
    rm -rf *
    cd ..
fi

if [ -d build.release ] ; then 
  cd build.release
  rm -r *
  cd ..
fi

if [ -d build.debug ] ; then 
  cd build.debug
  rm -r *
  cd ..
fi


