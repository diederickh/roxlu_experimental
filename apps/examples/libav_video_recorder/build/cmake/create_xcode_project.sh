#!/bin/sh

if [ -d build.xcodeproject ] ; then 
    rm -r build.xcodeproject
fi

if [ ! -d build.xcodeproject ] ; then 
  mkdir build.xcodeproject
fi

./cleanup_build_dir.sh

cd build.xcodeproject
cmake -G Xcode ../

