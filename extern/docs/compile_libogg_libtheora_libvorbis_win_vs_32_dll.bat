@echo off
:: Buid script for libogg, libtheora and libvorbis on windows using 
:: Microsoft Visual Studio 2010 and 2012 Express on Windows 7.
:: 
:: HOW TO:
:: ------
:: 0) Prerequisites
::    - Make sure that you've installed the "Microsoft Windows SDK for Windows 7
::     and .NET Framework 4" See: http://www.microsoft.com/en-us/download/details.aspx?id=8279
:: 
:: 1) Create some directories: 
::    - Downloads\compile_ogg
::    - Downloads\compile_ogg\sources\
::   
:: 2) Download these sources and put them in the 'sources' dir:
::   - libogg-1.3.0.zip
::   - libvorbis-1.3.3.zip
:: 
::   - We're using theora from SVN, build with revision: 18763
:: 
:: 3) Create a directory "sources" and extract the zip file 
::    using explorer > right mouse click > "Extract all..." option
::    for the files above you get "sources\libogg-1.3.0\libogg-1.3.0\"
::    you get the same for theora and vorbix.
::
:: 4) Before we run this script to compile the libraries you need to set the 
::    Runtime Library that is the MSVC compiled uses to create the libraries. 
::    For this you need to open all of the solution files:
:: 
::    libogg-1.3.0\win32\VS2010\libogg_dynamic.sln
::    libtheora-1.1.1\win32\VS2008\libtheora_dynamic.sln
::    libvorbis-1.3.3\win32\VS2010\vorbis_dynamic.sln
::
::    For each solution right click the static lib project, and select "Properties"
::    to open the project properties, go to:
::    
::    Configuration Properties > C/C++ > Code Generation: "Runtime Library" to "Multi-Threaded DLL (/MD)"
::    Configuration Properties > C/C++ > Optimization: "Whole Program Optimization" to "No"
::
::    Next, open a "Microsoft Visual Studio [2010,2012] console and execute this script, 
::    all libs will be stored in:
:: 
::    - Downloads\[your_dir]\build\
::
::
:: VERSION:
:: --------
:: 2012.01.10 - Updated the script to create dynamic libs
:: 2012.01.08 - Using latest theora version from svn, 
:: 2012.01.08 - Can't convert the 2008 solution for libtheora to MSVC 2010 :( (the svn version has a 2010 project though)
:: 2012.01.07 - Initial release

set d=%CD%
set sd=%d%\sources
set bd=%d%\build\

if not exist %bd% (
   mkdir %bd%
)

set ogg_src=libogg-1.3.0
set vorbis_src=libvorbis-1.3.3

set found_sources=1
call :check_source %ogg_src%
call :check_source %vorbis_src%

:: CHECKOUT LIBTHEORA
if not exist %sd%\theora (
   cd %sd% 
   mkdir theora
   cd theora
   svn co -r 18763 http://svn.xiph.org/trunk/theora .
   cd %d%
   goto :eof
)


if %found_sources% == 0 (
   echo ERROR: First download all sources and extract in sources/
   goto:eof
)
 
:: COMPILE LIBOGG
cd %sd%\%ogg_src%\%ogg_src%\win32\VS2010
msbuild libogg_dynamic.sln /p:Configuration=Release /p:OutDir=%bd% 

:: COMPILE LIBTHEORA
cd %d%
set INCLUDE=%sd%\%ogg_src%\%ogg_src%\include\;%bd%;%INCLUDE%
set LIB=%bd%;%LIB%;

cd %sd%\theora\win32\VS2010\
msbuild libtheora_dynamic.sln /p:useenv=true /p:Configuration=Release /p:OutDir=%bd% 

:: COMPILE VORBIS
cd %sd%\%vorbis_src%\%vorbis_src%\win32\VS2010\
msbuild vorbis_dynamic.sln /p:useenv=true /p:Configuration=Release /p:Outdir=%bd% 
msbuild vorbis_dynamic.sln /p:useenv=true /p:Configuration=Release /p:Outdir=%bd% 
msbuild vorbis_dynamic.sln /p:useenv=true /p:Configuration=Release /p:Outdir=%bd% 
msbuild vorbis_dynamic.sln /p:useenv=true /p:Configuration=Release /p:Outdir=%bd% 

cd %d%

:check_source
  if exist %sd%\%~1 (
   echo FOUND: %~1
  ) else (
   echo NOT FOUND: %~1
   set "found_sources=0"  
  ) 
goto:eof

:eof