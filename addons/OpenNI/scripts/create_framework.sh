#!/bin/sh

d=${PWD}
openni_dir=${d}/openni/Platform/Linux/CreateRedist/
openni_build=${d}/openni/Platform/Linux/Redist/OpenNI/
openni_libusb=${d}/openni/Platform/Linux/Build/Prerequisites/libusb/
avin_build=${d}/avin/Bin/sensor/
nite_build=${d}/nite/

function createDir {
	if [ ! -d $1 ] ; then 
		mkdir $1
	fi
}

# Creates a frameworks for OSX. 
createDir framework

# create basic structure
cd framework
createDir Versions
createDir Versions/A
createDir Versions/A/Headers
createDir Versions/A/Libraries
createDir Versions/A/Resources/

cd Versions
ln -s A Current

# Copy OpenNI libs
cd ${d}
cp ${openni_build}/Lib/*.dylib ./framework/Versions/A/Libraries/
cd framework/Versions

# Copy LibUSB (assuming you used our compile script)
cd ${d}
cp ${openni_libusb}/build/lib/libusb-1.0.0.dylib ./framework/Versions/A/Libraries
cp -r ${openni_libusb}/build/include/libusb-1.0 ./framework/Versions/A/Headers/

# Copy sensor kinect
cp ${avin_build}/Lib/*.dylib ./framework/Versions/A/Libraries/
cp -r ${d}/avin/Include/* ./framework/Versions/A/Headers/
cp ${avin_build}/Config/GlobalDefaultsKinect.ini ./framework/Versions/A/Resources/

cd ${d}/framework
ln -s Versions/A/Libraries/libOpenNI.dylib OpenNI
ln -s Versions/A/Headers Headers
ln -s Versions/A/Resources Resources

# Copy NITE files
cd ${d}
cp -r ${nite_build}/Features_1_5_2/Data/* ./framework/Resources/
cp ${nite_build}/Features_1_5_2/Bin/*.dylib ./framework/Versions/Current/Libraries/
cp ${nite_build}/Hands_1_5_2/Data/* ./framework/Resources/
cp ${nite_build}/Hands_1_5_2/Bin/libXnVHandGenerator_1_5_2.dylib ./framework/Versions/Current/Libraries/
cp ${nite_build}/Bin/libXnVCNITE_1_5_2.dylib ./framework/Versions/Current/Libraries/
cp ${nite_build}/Bin/libXnVFeatures_1_5_2.dylib ./framework/Versions/Current/Libraries/
cp ${nite_build}/Bin/libXnVHandGenerator_1_5_2.dylib ./framework/Versions/Current/Libraries/
cp ${nite_build}/Bin/libXnVNite_1_5_2.dylib ./framework/Versions/Current/Libraries/
cp ${nite_build}/Include/*.h ./framework/Versions/Current/Headers/

# Copy resources
cd ${d}
cp resources/openni/modules.xml ./framework/Resources/

# Fix ids
cd ${d}
cp fix_install_names.py framework/Versions/Current/Libraries
cd framework/Versions/Current/Libraries
./fix_install_names.py

# Copy headers
cd ${d}
cp fix_headers.py framework/Versions/Current/Headers/
cp -r ${openni_build}/Include/* framework/Versions/Current/Headers/
cd framework/Versions/Current/Headers/
./fix_headers.py
mv output/*.h ./
mv output/XnEE/*.h ./XnEE/
rm -r output
