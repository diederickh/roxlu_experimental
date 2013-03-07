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
- apt-get install glu


Compiler flags
==============

- `ROXLU_GL_CORE3`: if you want to target openGL 3.2 core profile, forward compat.
- `ROXLU_DEBUG:` enable verbose output for e.g. opengl shaders
- `GLEW_STATIC`: when using the cmake files this set for you. when you target < 3.x, make sure to 
                 define this in you preprocessor settings.
        

Using roxlu lib with openFrameworks and XCode
=============================================
- Open you XCode project
- `File > Add Files to "emptyExample"...` and select `roxlu/lib/build/xcode/roxlu.xcodeproj`.
- `View > Navigators > Project Navigator` and click on the blue `emptyExample` project.  Click on the `emptyExample` target.
  - Select `Build Phases > Link Binary With Libraries`. Press the `+` button  and select `libroxlu.a`
  - Select `Target Dependencies`. Press the `+` button and select the `roxlu` target.
  - Select `Build Settings` (still while the emptyExample target is selected), search for `Header search paths`, and double click to add a new entry. Drag & drop the following directories:
      - `roxlu/lib/include/`
      - `roxlu/extern/include/`
  - Select `Build Settings`, search for `Preprocessor Macros` and add `ROXLU_GL_WRAPPER`

  
