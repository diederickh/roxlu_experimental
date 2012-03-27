// Geometric Tools, LLC
// Copyright (c) 1998-2011
// Distributed under the Boost Software License, Version 1.0.
// http://www.boost.org/LICENSE_1_0.txt
// http://www.geometrictools.com/License/Boost/LICENSE_1_0.txt
//
// File Version: 5.0.1 (2010/10/01)

#ifndef WM5FLOAT2_H
#define WM5FLOAT2_H

#include "Wm5MathematicsLIB.h"
#include "Wm5Tuple.h"

namespace Wm5
{

class WM5_MATHEMATICS_ITEM Float2 : public Tuple<2,float>
{
public:
    // Construction and destruction.
    Float2 ();   // uninitialized
    ~Float2 ();  // hides ~Tuple<2,float>
    Float2 (float f0, float f1);
    Float2 (const Float2& tuple);

    // Assignment.
    Float2& operator= (const Float2& tuple);
};

}

#endif
