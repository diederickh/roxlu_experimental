#!/usr/bin/env python
import os
import commands
import re

# extract zips
if not os.path.exists("backup"):
	zips = commands.getoutput("ls *.zip")
	for zipfile in zips.split("\n"):
		os.system("unzip -a " +zipfile)

# create backups
if not os.path.exists("backup"):
	os.system("mkdir backup")
	os.system("mv *.zip backup/")

# cleanup dirs
dirs = commands.getoutput("ls -d */")
for dir in dirs.split("\n"):
	if re.search("openni-", dir, re.IGNORECASE):
		os.system("mv "+dir +" openni ")
	elif re.search("avin2-", dir, re.IGNORECASE):
		os.system("mv "+dir +" avin ")

# extract and compile libusb
if not os.path.exists("openni/Platform/Linux/Build/Prerequisites/libusb"):
	os.system("cd openni/Platform/Linux/Build/Prerequisites/ && tar -xvjf libusb-1.0.8-osx.tar.bz2")
	os.system("cp resources/openni/compile_libusb.sh openni/Platform/Linux/Build/Prerequisites/libusb/")
	os.system("cd openni/Platform/Linux/Build/Prerequisites/libusb/ && ./compile_libusb.sh")

# copy our edited files
os.system("cp resources/openni/Makefile openni/Platform/Linux/Build/OpenNI/")
os.system("cp resources/openni/XnOpenNI.cpp openni/Source/OpenNI/")
os.system("cp resources/openni/Redist_OpenNi.py openni/Platform/Linux/CreateRedist/")

# compile openni 
# tmp off...
os.system("cd openni/Platform/Linux/CreateRedist && chmod 777 ./RedistMaker && ./RedistMaker")

# rename the compiled dir
os.system("mv openni/Platform/Linux/Redist/OpenNI-* openni/Platform/Linux/Redist/OpenNI")
