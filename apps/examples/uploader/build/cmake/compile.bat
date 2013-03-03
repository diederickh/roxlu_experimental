@echo off

set d=%CD%

if not exist "%d%\build" (
   mkdir %d%\build
)

cd %d%\build
cmake -G "Visual Studio 10" ..\
cmake --build . --target install --config Release
:: -- /p:Configuration=Release /v:q
:: %d%\bin\011_windows.exe
:: cmake --build . --target install -- /p:Configuration=Debug

:: msbuild Project.sln /v:m /p:useenv=true /p:Configuration=Release /t:rebuild /p:OutDir="../bin/"
:: msbuild Project.sln /v:m /p:useenv=true /p:Configuration=Release /t:011_windows /p:OutDir="../bin/"
:: msbuild Project.sln /v:m /p:useenv=true /p:Configuration=Release /p:OutDir="../bin/"

cd %d%