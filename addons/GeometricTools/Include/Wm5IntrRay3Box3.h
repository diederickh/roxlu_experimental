// Geometric Tools, LLC
// Copyright (c) 1998-2011
// Distributed under the Boost Software License, Version 1.0.
// http://www.boost.org/LICENSE_1_0.txt
// http://www.geometrictools.com/License/Boost/LICENSE_1_0.txt
//
// File Version: 5.0.1 (2010/10/01)

#ifndef WM5INTRRAY3BOX3_H
#define WM5INTRRAY3BOX3_H

#include "Wm5MathematicsLIB.h"
#include "Wm5Intersector.h"
#include "Wm5Ray3.h"
#include "Wm5IntrLine3Box3.h"

namespace Wm5
{

template <typename Real>
class WM5_MATHEMATICS_ITEM IntrRay3Box3
    : public Intersector<Real,Vector3<Real> >
{
public:
    IntrRay3Box3 (const Ray3<Real>& ray, const Box3<Real>& box);

    // Object access.
    const Ray3<Real>& GetRay () const;
    const Box3<Real>& GetBox () const;

    // Static intersection queries.
    virtual bool Test ();
    virtual bool Find ();

    // The intersection set.
    int GetQuantity () const;
    const Vector3<Real>& GetPoint (int i) const;

private:
    using Intersector<Real,Vector3<Real> >::mIntersectionType;

    // The objects to intersect.
    const Ray3<Real>* mRay;
    const Box3<Real>* mBox;

    // Information about the intersection set.
    int mQuantity;
    Vector3<Real> mPoint[2];
};

typedef IntrRay3Box3<float> IntrRay3Box3f;
typedef IntrRay3Box3<double> IntrRay3Box3d;

}

#endif
