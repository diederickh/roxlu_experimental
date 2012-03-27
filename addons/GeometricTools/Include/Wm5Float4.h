// Geometric Tools, LLC
// Copyright (c) 1998-2011
// Distributed under the Boost Software License, Version 1.0.
// http://www.boost.org/LICENSE_1_0.txt
// http://www.geometrictools.com/License/Boost/LICENSE_1_0.txt
//
// File Version: 5.0.1 (2010/10/01)

#ifndef WM5FLOAT4_H
#define WM5FLOAT4_H

#include "Wm5MathematicsLIB.h"
#include "Wm5Tuple.h"

namespace Wm5
{

class WM5_MATHEMATICS_ITEM Float4 : public Tuple<4,float>
{
public:
    // Construction and destruction.
    Float4 ();   // uninitialized
    ~Float4 ();  // hides ~Tuple<4,float>
    Float4 (float f0, float f1, float f2, float f3);
    Float4 (const Float4& tuple);

    // Assignment.
    Float4& operator= (const Float4& tuple);
};

}

#endif
