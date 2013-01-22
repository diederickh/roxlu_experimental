Roxlu lib 0.0.0.0.1
===================
This is a experimental, personal library used for creative simulations. As 
you can see from the version it's not yet meant to be a stable version and it's
still changing.


openGL
======
Roxlu lib is build around opengl applications using GLFW as a wrapper to setup 
the GL context. Though, it's possible to use this library withouth opengl and
just as e.g. console application or library.   

We're using CMake aas build system which set a define called ROXLU_GL_WRAPPER
(see include/roxlu/opengl/OpenGLInit.h). When we do not set the ROXLU_GL_WRAPPER
define all openGL related code will be excluded from compilation.

Install Ubuntu/linux
=====================
You might need to install some additional developer packages:
- apt-get install libudev-dev
