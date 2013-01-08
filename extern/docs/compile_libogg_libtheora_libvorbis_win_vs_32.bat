@echo off
:: Buid script for libogg, libtheora and libvorbis on windows using 
:: Microsoft Visual Studio 2010 and 2012 Express on Windows 7.
:: 
:: HOW TO:
:: ------
:: 1) Create some directories: 
::    - Downloads\compile_ogg
::    - Downloads\compile_ogg\sources\
::   
:: 2) Download these sources and put them in the 'sources' dir:
::   - libogg-1.3.0.zip
::   - libtheora-1.1.1.zip
::   - libvorbis-1.3.3.zip
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
::    libogg-1.3.0\win32\VS2010\libogg_static.sln
::    libtheora-1.1.1\win32\VS2008\libtheora_static.sln
::    libvorbis-1.3.3\win32\VS2010\vorbis_static.sln
::
::    For each solution right click the static lib project, and select "Properties"
::    to open the project properties, go to:
::    
::    Configuration Properties > C/C++ > Code Generation
::
::    And change "Runtime Library" to "Multi-Threaded (/MT)"
::
::    Next, open a "Microsoft Visual Studio [2010,2012] console and execute this script, 
::    all libs will be stored in:
:: 
::    - Downloads\[your_dir]\build\
::
::
:: VERSION:
:: --------
:: 2012.01.07 - Initial release

set d=%CD%
set sd=%d%\sources
set bd=%d%\build

if not exist %bd% (
   mkdir %bd%
)

set ogg_src=libogg-1.3.0
set theora_src=libtheora-1.1.1
set vorbis_src=libvorbis-1.3.3

set found_sources=1
call :check_source %ogg_src%
call :check_source %theora_src%
call :check_source %vorbis_src%

if %found_sources% == 0 (
   echo ERROR: First download all sources and extract in sources/
   goto:eof
)
 
:: COMPILE LIBOGG
cd %sd%\%ogg_src%\%ogg_src%\win32\VS2010
msbuild libogg_static.sln /p:Configuration=Release /p:OutDir=%bd% 

:: COMPILE LIBTHEORA
cd %d%
set INCLUDE=%sd%\%ogg_src%\%ogg_src%\include\;%bd%;%INCLUDE%
set LIB=%bd%;%LIB%;

cd %sd%\%theora_src%\%theora_src%\win32\VS2008\
msbuild libtheora_static.sln /p:useenv=true /p:Configuration=Release /p:OutDir=%bd% /t:libtheora_static

:: COMPILE VORBIS
cd %sd%\%vorbis_src%\%vorbis_src%\win32\VS2010\
msbuild vorbis_static.sln /p:useenv=true /p:Configuration=Release /p:Outdir=%bd% /t:libvorbis_static
msbuild vorbis_static.sln /p:useenv=true /p:Configuration=Release /p:Outdir=%bd% /t:libvorbisfile
msbuild vorbis_static.sln /p:useenv=true /p:Configuration=Release /p:Outdir=%bd% /t:vorbisdec
msbuild vorbis_static.sln /p:useenv=true /p:Configuration=Release /p:Outdir=%bd% /t:vorbisenc

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