@echo off
:: Build script for libuv (https://github.com/joyent/libuv) using 
:: Microsoft Visual Studio 2010 Expres on Windows 7
::
:: HOW TO
:: ------
:: 
:: 
set d=%CD%
set bd=%d%\build\
set lvdir=%d%\libuv\

if not exist %lvdir% (

   mkdir %lvdir%
   cd %lvdir%
   git clone git@github.com:joyent/libuv.git .
)

cd %lvdir%
vcbuild.bat


