// Geometric Tools, LLC
// Copyright (c) 1998-2011
// Distributed under the Boost Software License, Version 1.0.
// http://www.boost.org/LICENSE_1_0.txt
// http://www.geometrictools.com/License/Boost/LICENSE_1_0.txt
//
// File Version: 5.0.1 (2010/10/01)

#ifndef WM5FLOAT1_H
#define WM5FLOAT1_H

#include "Wm5MathematicsLIB.h"
#include "Wm5Tuple.h"

namespace Wm5
{

class WM5_MATHEMATICS_ITEM Float1 : public Tuple<1,float>
{
public:
    // Construction and destruction.
    Float1 ();   // uninitialized
    ~Float1 ();  // hides ~Tuple<1,float>
    Float1 (float f0);
    Float1 (const Float1& tuple);

    // Assignment.
    Float1& operator= (const Float1& tuple);
};

}

#endif
