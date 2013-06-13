#!/bin/sh

rm -r build.xcodeproject

if [ ! -d build.xcodeproject ] ; then 
  mkdir build.xcodeproject
fi

./cleanup_build_dir.sh

cd build.xcodeproject
cmake -G Xcode ../

