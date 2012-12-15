#!/bin/sh

d=${PWD}
app=${PWD##*/}

xcodebuild -target simulation -configuration Debug 
