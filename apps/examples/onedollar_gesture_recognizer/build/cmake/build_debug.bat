@echo off

set d=%CD%

if not exist "%d%\build.debug" (
   mkdir %d%\build.debug
)

if not exist "%d%\..\..\bin\data" (
   mkdir %d%\..\..\bin\data
)

cd %d%\build.debug
cmake -DCMAKE_BUILD_TYPE=Debug -G "Visual Studio 10" ..\
cmake --build . --target install -- /p:Configuration=Debug

:: -- /p:Configuration=Release /v:q
:: %d%\bin\011_windows.exe
:: cmake --build . --target install -- /p:Configuration=Debug

:: msbuild Project.sln /v:m /p:useenv=true /p:Configuration=Release /t:rebuild /p:OutDir="../bin/"
:: msbuild Project.sln /v:m /p:useenv=true /p:Configuration=Release /t:011_windows /p:OutDir="../bin/"
:: msbuild Project.sln /v:m /p:useenv=true /p:Configuration=Release /p:OutDir="../bin/"

cd %d%