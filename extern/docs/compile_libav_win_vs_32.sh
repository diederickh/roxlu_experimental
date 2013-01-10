#!/bin/sh

# Compiling libav on windows
# --------------------------
# Compiling libav takes quite a bit of time and you need to setup a build 
# environment. I've followed these instructions for ffmpeg: 
# http://blogs.gnome.org/rbultje/2012/09/27/microsoft-visual-studio-support-in-ffmpeg-and-libav/
#
# How to
# ------
# 1) Install mingw http://sourceforge.net/projects/mingw/files/latest/download?source=files
#    [x] C compiler
#    [x] C++ compiler
#    [x] MSYS Basic system
#    [x] MingW Developer Toolkit
#
# 2) Download yasm.exe (Win32): http://yasm.tortall.net/Download.html 
#    Copy yasm-1.2.0-win32.exe to: C:\MinGW\msys\1.0\bin\yasm.exe  (note: rename!)
#
# 3) Download c99-tc89 converter:  ftp://ftp.videolan.org/pub/contrib/c99-to-c89/1.0.1/
#    - Copy: - c99conv.exe 
#            - c99wrap.exe
#            - makedef
#   To: C:\MinGW\msys\1.0\bin
#
# 4) Download the msinttypes: http://code.google.com/p/msinttypes/downloads/list
#    - Extract to C:\msinttypes\
#  
# 5) Make sure the mingw/msys shell is aware of all Microsoft Visual Studio 
#    environment variables:
#    - Open C:\MinGW\msys\1.0\bin\msys.bat 
#    - Add this CALL around line 17 (just before the 'if "%1" ...')
#
#       For Microsft Visual Studio 2010: 
#       CALL "%%VS100COMNTOOLS%%vsvars32.bat"  
#
#       For Microsft Visual Studio 2012:
#       CALL "%%VS110COMNTOOLS%%vsvars32.bat"  
# 
# 6) Next you can execute this script, or just follow these steps:
#    - Open a MinGW shell: Start > MinGW > MinGW Shell
#
#    - Test to see if the CALL (step 5) worked by execute: cl (enter)
#      if you do not see something like "Microsoft (R) 32 bit ... " then you need to 
#      check if you're using the correct %%VS???COMNTOOLS%% environment variable. 
#      Go to your configuration screen > system > env setting and look for something 
#      similiar.
#   
#    - Make sure we use the correct link.exe. 
#      
#       When you type: 
#         $ which link
#    
#      ..you probably see something like: /bin/link, which we need to fix by doing:
#     
#      mv /bin/link.exe /bin/msys-link.exe
#      
#    - Next we need to make sure our newlines are handled correctly, so type:
#      $ git config --global core.autocrlf false
#      
#    - Assuming your compiler is found, continue by downloading the git version of 
#      lib av into a tmp dir:
#      $ cd 
#      $ cd Downloads
#      $ mkdir libav_compile && cd libav_compile
#      $ git clone git://git.libav.org/libav.git
#      $ cd libav 
#      $ ./configure --toolchain=msvc --arch=i686 --extra-cflags="-I/c/msinttypes/"
#      $ make
#      
#      NOTE: if you want to create DLL files, use:
#      $ ./configure --toolchain=msvc --arch=i686 --enable-shared --extra-cflags="-I/c/msinttypes/"
# 
#    - Once everything is compiled you can get the static libraries, linked against
#      the /MT (MultiThreaded) runtime lib from e.g libutil/libavutil.a, libswscale/libswscale.a
#      

d=${PWD}
sd=${d}/sources/
libav_dir=${sd}/libav/
PATH=${d}/sources:${PATH}

if [ ! -d build ] ; then 
   mkdir build
fi

cd ${libav_dir}
./configure --toolchain=msvc --arch=i686 --extra-cflags="-I/c/msinttypes/" --prefix=${d}/build
make
make install
