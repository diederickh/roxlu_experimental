// Geometric Tools, LLC
// Copyright (c) 1998-2011
// Distributed under the Boost Software License, Version 1.0.
// http://www.boost.org/LICENSE_1_0.txt
// http://www.geometrictools.com/License/Boost/LICENSE_1_0.txt
//
// File Version: 5.0.1 (2010/10/01)

#ifndef WM5FLOAT3_H
#define WM5FLOAT3_H

#include "Wm5MathematicsLIB.h"
#include "Wm5Tuple.h"

namespace Wm5
{

class WM5_MATHEMATICS_ITEM Float3 : public Tuple<3,float>
{
public:
    // Construction and destruction.
    Float3 ();   // uninitialized
    ~Float3 ();  // hides ~Tuple<3,float>
    Float3 (float f0, float f1, float f2);
    Float3 (const Float3& tuple);

    // Assignment.
    Float3& operator= (const Float3& tuple);
};

}

#endif
