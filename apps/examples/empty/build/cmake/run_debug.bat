@echo off

set d=%CD%

cd %d%\..\..\
for %%* in (.) do set appname=%%~n*
set app_exe=%appname%_debug.exe

cd %d%
call build_release.bat

cd %d%\..\..\bin\
%app_exe%

cd %d%