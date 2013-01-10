@echo off

Compiling libvpx on Windows
=================================
1) We're using the git version and MinGW build system. I first compiled
   libav so I'm not 100% sure what dependencies you might need to setup. 
   But at least make sure you have:
   - Install mingw
   - Install yasm
   - Check your link.exe
   - Check your line endings 

   For all these steps see the "compile_libav_win_vs32.sh" file. 

2) Once you've installed all dependencies, open an "MinGW Shell"
   Start > MinGW > MinGW Shell

   Then execute:

   $ cd   # go to your home dir
   $ cd Downloads
   $ mkdir vpx && cd vpx
   $ mkdir vpx && vpx
   $ git clone http://git.chromium.org/webm/libvpx.git .
   $ git checkout 27825e9e3a70c1d71c2bdea5bd44bf7b4bbb1824
   $ cd 
   $ cd Downloads/vpx
   $ mkdir build
   $ cd build
   $ ../vpx/configure --target=x86-win32-vs9 --enable-static-msvcrt
   $ make

3) When make finished, you have a lot of files in you build/ dir. Open vpx.sln with
   Microsoft Visual Studio. 

   But first make sure to copy yasm.exe to:
   C:\Program Files (x86)\Microsoft Visual Studio 10.0\VC\bin
   
   Then you can compile the 'vpx' project which gives you a static lib.

   DON'T FORGET TO REMOTE yasm.exe AGAIN SO YOUR SYSTEM STAYS CLEAN.
