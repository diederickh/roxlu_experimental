@echo off
set d=%CD%

if not exist %d%\glfw (
   git clone https://github.com/elmindreda/glfw.git 
)

call :compile_msvc compile_vs2010,build_vs2010,"Visual Studio 10"
call :compile_msvc compile_vs2012,build_vs2012,"Visual Studio 11"

:compile_msvc
        cd %d%
        if exist %~1 ( rd /s/q %~1 )
        if exist %~2 ( rd /s/q %~2 )
        mkdir %~1
        mkdir %~2
        cd %d%/%~1
        echo. %d%\glfw
        cmake -DCMAKE_INSTALL_PREFIX="%d%\%~2" -G "%~3" %d%\glfw 
        cmake --build . --target install -- /p:Configuration=Release   
goto:eof

:eof