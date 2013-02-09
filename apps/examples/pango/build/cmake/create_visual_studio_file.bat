@echo off
if not exist %cd%\build (
   mkdir %cd%\build
)
cd %cd%\build
cmake -G "Visual Studio 10" ..\
cd ..