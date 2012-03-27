// Geometric Tools, LLC
// Copyright (c) 1998-2011
// Distributed under the Boost Software License, Version 1.0.
// http://www.boost.org/LICENSE_1_0.txt
// http://www.geometrictools.com/License/Boost/LICENSE_1_0.txt
//
// File Version: 5.0.2 (2011/03/27)

#ifndef WM5TUPLE_H
#define WM5TUPLE_H

#include "Wm5CoreLIB.h"

// The class TYPE is either native data or is class data that has the
// following member functions:
//   TYPE::TYPE ()
//   TYPE::TYPE (const TYPE&);
//   TYPE& TYPE::operator= (const TYPE&)

namespace Wm5
{

template <int DIMENSION, typename TYPE>
class Tuple
{
public:
    // Construction and destruction.  The default constructor does not
    // initialize the tuple elements for native elements.  The tuple elements
    // are initialized for class data whenever TYPE initializes during its
    // default construction.
    Tuple ();
    Tuple (const Tuple& tuple);
    ~Tuple ();

    // Coordinate access.
    inline operator const TYPE* () const;
    inline operator TYPE* ();
    inline const TYPE& operator[] (int i) const;
    inline TYPE& operator[] (int i);

    // Assignment.
    Tuple& operator= (const Tuple& tuple);

    // Comparison.  The inequalities make the comparisons using memcmp, thus
    // treating the tuple as an array of unsigned bytes.
    bool operator== (const Tuple& tuple) const;
    bool operator!= (const Tuple& tuple) const;
    bool operator<  (const Tuple& tuple) const;
    bool operator<= (const Tuple& tuple) const;
    bool operator>  (const Tuple& tuple) const;
    bool operator>= (const Tuple& tuple) const;

protected:
    TYPE mTuple[DIMENSION];
};

#include "Wm5Tuple.inl"

}

#endif
