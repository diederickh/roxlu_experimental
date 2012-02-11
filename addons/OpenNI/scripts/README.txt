OpenNI as MacOSX Framework Bundle
=================================

- Create a directory "openni_framework"

- Go to the OpenNI GitHub Page: https://github.com/OpenNI/OpenNI

- Click the Download as Zip button. 
I downloaded: OpenNI-OpenNI-Stable-1.5.2.23-0-g1516074.zip

- Go to the SensorKinect repository from Avin: https://github.com/avin2/SensorKinect
I downloaded: avin2-SensorKinect-faf4994.zip

- Click the "Download as Zip" button.

- Go to Go to http://www.openni.org/Downloads/OpenNIModules.aspx, 
Select OpenNI Compliant Middleware Binaries
Select Unstable
Select Primesense NITE Unstable for mac
click downlaod

I downloaded: nite-bin-macosx-v1.5.2.21.tar.bz2

- Put both zip files into the "openni_framework" dir

- Execute: ./compile_roxlu_openni.py

- Execute: ./compile_roxlu_sensorkinect.py

- Execute: ./compile_roxlu_nite.py

- Execute: ./create_framework.sh



FILES 
=====

compile_roxlu_openni.py:
------------------------
Copies some edited files from resources/openni into the openni/
directories. These files are edited so we can use OpenNI as a framework.
Most important thing is the way how the XnOpenNI uses the modules.xml file.

compile_roxlu_sensorkinect.py:
------------------------------
This file doesn't do much; it just extracts the zip with all the binaries.

compile_roxlu_nite.py
---------------------
Just extracts the downloaded nite file and renames the dir.

create_framework.sh:
---------------------
This script does most of the work (together with fix_headers.py and 
fix_install_names.py). It creates the directory structure for a MacOSX 
Framework Bundle  and copies all the necessary headers from OpenNI and
Sensor Kinect into the correct dirs.

It uses fix_header.py to adjust the .h files from OpenNI and SensorKinect
in such a way that is uses "local" includes instead of "glboal" includes
for local files.  This way the framework can find the headers itself and
you don't need to se a header path. I basically changes:
#include <somefile>  into #include "somefile"

The fix_install_names.py sets the correct -id values and paths to 
dylibs. It does this in such a way your application can find the dylibs
at runtime.


HOW TO USE XCode 4.2
======================

- Drag & Drop the Framework into your XCode project

- Click on the project name

- Click on the target

- Open the Build Phases tab

- Open the "Copy files" pane

- Drag the OpenNI framework you just added to your project
to the Copy Files pane.


