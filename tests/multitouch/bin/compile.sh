#!/bin/sh
make clean
cmake ../
make VERBOSE=1
cp Trackpad.a ../../../roxlu/experimental/

