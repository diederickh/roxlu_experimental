// Geometric Tools, LLC
// Copyright (c) 1998-2011
// Distributed under the Boost Software License, Version 1.0.
// http://www.boost.org/LICENSE_1_0.txt
// http://www.geometrictools.com/License/Boost/LICENSE_1_0.txt
//
// File Version: 5.0.1 (2010/10/01)

#ifndef WM5MASSSPRINGVOLUME_H
#define WM5MASSSPRINGVOLUME_H

#include "Wm5PhysicsLIB.h"
#include "Wm5ParticleSystem.h"

namespace Wm5
{

template <typename Real, typename TVector>
class WM5_PHYSICS_ITEM MassSpringVolume : public ParticleSystem<Real,TVector>
{
public:
    // Construction and destruction.  This class represents an SxRxC array of
    // masses lying on in a volume and connected by an array of springs.  The
    // masses are indexed by mass[s][r][c] for 0 <= s < S, 0 <= r < R, and
    // 0 <= c < C.  The mass at interior position X[s][r][c] is connected by
    // springs to the masses at positions X[s][r-1][c], X[s][r+1][c],
    // X[s][r][c-1], X[s][r][c+1], X[s-1][r][c], and X[s+1][r][c].  Boundary
    // masses have springs connecting them to the obvious neighbors ("face"
    // mass has 5 neighbors, "edge" mass has 4 neighbors, "corner" mass has 3
    // neighbors).  The masses are arranged in lexicographical order:
    // position[c+C*(r+R*s)] = X[s][r][c] for 0 <= s < S, 0 <= r < R, and
    // 0 <= c < C.  The other arrays are stored similarly.
    MassSpringVolume (int numSlices, int numRows, int numCols, Real step);
    virtual ~MassSpringVolume ();

    int GetNumSlices () const;
    int GetNumRows () const;
    int GetNumCols () const;
    void SetMass (int slice, int row, int col, Real mass);
    Real GetMass (int slice, int row, int col) const;
    TVector*** Positions3D () const;
    TVector& Position (int slice, int row, int col);
    TVector*** Velocities3D () const;
    TVector& Velocity (int slice, int row, int col);

    // Each interior mass at (s,r,c) has 6 adjacent springs.  Face masses
    // have only 5 neighbors, edge masses have only 4 neighbors, and corner
    // masses have only 3 neighbors.  Each mass provides access to 3 adjacent
    // springs at (s,r,c+1), (s,r+1,c), and (s+1,r,c).  The face, edge, and
    // corner masses provide access to only an appropriate subset of these.
    // The caller is responsible for ensuring the validity of the (s,r,c)
    // inputs.
    Real& ConstantS (int slice, int row, int col);  // spring to (s+1,r,c)
    Real& LengthS (int slice, int row, int col);    // spring to (s+1,r,c)
    Real& ConstantR (int slice, int row, int col);  // spring to (s,r+1,c)
    Real& LengthR (int slice, int row, int col);    // spring to (s,r+1,c)
    Real& ConstantC (int slice, int row, int col);  // spring to (s,r,c+1)
    Real& LengthC (int slice, int row, int col);    // spring to (s,r,c+1)

    // Callback for acceleration (ODE solver uses x" = F/m) applied to
    // particle i.  The positions and velocities are not necessarily
    // mPositions and mVelocities since the ODE solver evaluates the
    // impulse function at intermediate positions.
    virtual TVector Acceleration (int i, Real time,
        const TVector* positions, const TVector* velocities);

    // The default external force is zero.  Derive a class from this one to
    // provide nonzero external forces such as gravity, wind, friction,
    // and so on.  This function is called by Acceleration(...) to append the
    // acceleration F/m generated by the external force F.
    virtual TVector ExternalAcceleration (int i, Real time,
        const TVector* positions, const TVector* velocities);

protected:
    using ParticleSystem<Real,TVector>::mInvMasses;
    using ParticleSystem<Real,TVector>::mPositions;
    using ParticleSystem<Real,TVector>::mVelocities;

    int GetIndex (int slice, int row, int col) const;
    void GetCoordinates (int i, int& slice, int& row, int& col) const;

    int mNumSlices;            // S
    int mNumRows;              // R
    int mNumCols;              // C
    int mSliceQuantity;        // R*C
    TVector*** mPositionGrid;  // S-by-R-by-C
    TVector*** mVelocityGrid;  // S-by-R-by-C

    int mNumSlicesM1;          // S-1
    int mNumRowsM1;            // R-1
    int mNumColsM1;            // C-1
    Real*** mConstantsS;       // (S-1)-by-R-by-C
    Real*** mLengthsS;         // (S-1)-by-R-by-C
    Real*** mConstantsR;       // S-by-(R-1)-by-C
    Real*** mLengthsR;         // S-by-(R-1)-by-C
    Real*** mConstantsC;       // S-by-R-by-(C-1)
    Real*** mLengthsC;         // S-by-R-by-(C-1)
};

typedef MassSpringVolume<float,Vector3f> MassSpringVolume3f;
typedef MassSpringVolume<double,Vector3d> MassSpringVolume3d;

}

#endif
