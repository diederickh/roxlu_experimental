@echo off

:: Compiling glfw using the latest github version
:: -----------------------------------------------
:: 1) Open the MSVC++ 2010 or 2012 CMD window.
::    Create a directory to work in: 
::    $ cd Downloads
::    $ mkdir glfw
::   
:: 2) Copy this script into that dir. 
::    - Comment/Uncomment the appropriate line with "call :compile_msvc .. etc.." 
::      to switch between a MSVC 2010 or 2012 .. build.
::
:: 3) Execute this script from your MSVC CMD window. After running you'll find yourself
::    a build directory that contains the static libraries. By default we use the 
::    "MultiThreaded" runtime (/MT)


set d=%CD%

if not exist %d%\glfw (
   git clone https://github.com/elmindreda/glfw.git 
)

:: call :compile_msvc compile_vs2010,build_vs2010,"Visual Studio 10"
call :compile_msvc compile_vs2012,build_vs2012,"Visual Studio 11"

:compile_msvc
        cd %d%
        if exist %~1 ( rd /s/q %~1 )
        if exist %~2 ( rd /s/q %~2 )
        mkdir %~1
        mkdir %~2
        cd %d%/%~1
        echo. %d%\glfw
        cmake -DCMAKE_INSTALL_PREFIX="%d%\%~2" -DCMAKE_C_FLAGS_DEBUG="/MT" -DCMAKE_C_FLAGS_RELEASE="/MT" -DCMAKE_CXX_FLAGS_DEBUG="/MT" -DCMAKE_CXX_FLAGS_RELEASE="/MT" -G "%~3" %d%\glfw 
        cmake --build . --target install -- /p:Configuration=Release   
goto:eof

:eof