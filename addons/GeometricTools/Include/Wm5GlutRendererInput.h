// Geometric Tools, LLC
// Copyright (c) 1998-2011
// Distributed under the Boost Software License, Version 1.0.
// http://www.boost.org/LICENSE_1_0.txt
// http://www.geometrictools.com/License/Boost/LICENSE_1_0.txt
//
// File Version: 5.6.0 (2010/01/01)

#ifndef WM5GLUTRENDERERINPUT_H
#define WM5GLUTRENDERERINPUT_H

#include "Wm5OpenGLRendererLIB.h"

#ifdef __APPLE__
#include <GLUT/glut.h>
#else
#include "glut.h"
#include "Wm5WglExtensions.h"
#endif

namespace Wm5
{

class RendererInput
{
public:
    int mWindowID;
    bool mDisableVerticalSync;
};

}

#endif
