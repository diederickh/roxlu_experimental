// Geometric Tools, LLC
// Copyright (c) 1998-2011
// Distributed under the Boost Software License, Version 1.0.
// http://www.boost.org/LICENSE_1_0.txt
// http://www.geometrictools.com/License/Boost/LICENSE_1_0.txt
//
// File Version: 5.0.1 (2011/03/27)

//----------------------------------------------------------------------------
template <int DIMENSION, typename TYPE>
Tuple<DIMENSION,TYPE>::Tuple ()
{
    // Uninitialized for native data.  Initialized for class data as long as
    // TYPE's default constructor initializes its own data.
}
//----------------------------------------------------------------------------
template <int DIMENSION, typename TYPE>
Tuple<DIMENSION,TYPE>::Tuple (const Tuple& tuple)
{
    for (int i = 0; i < DIMENSION; ++i)
    {
        mTuple[i] = tuple.mTuple[i];
    }
}
//----------------------------------------------------------------------------
template <int DIMENSION, typename TYPE>
Tuple<DIMENSION,TYPE>::~Tuple ()
{
}
//----------------------------------------------------------------------------
template <int DIMENSION, typename TYPE>
inline Tuple<DIMENSION,TYPE>::operator const TYPE* () const
{
    return mTuple;
}
//----------------------------------------------------------------------------
template <int DIMENSION, typename TYPE>
inline Tuple<DIMENSION,TYPE>::operator TYPE* ()
{
    return mTuple;
}
//----------------------------------------------------------------------------
template <int DIMENSION, typename TYPE>
inline const TYPE& Tuple<DIMENSION,TYPE>::operator[] (int i) const
{
    return mTuple[i];
}
//----------------------------------------------------------------------------
template <int DIMENSION, typename TYPE>
inline TYPE& Tuple<DIMENSION,TYPE>::operator[] (int i)
{
    return mTuple[i];
}
//----------------------------------------------------------------------------
template <int DIMENSION, typename TYPE>
Tuple<DIMENSION,TYPE>& Tuple<DIMENSION,TYPE>::operator= (const Tuple& tuple)
{
    for (int i = 0; i < DIMENSION; ++i)
    {
        mTuple[i] = tuple.mTuple[i];
    }
    return *this;
}
//----------------------------------------------------------------------------
template <int DIMENSION, typename TYPE>
bool Tuple<DIMENSION,TYPE>::operator== (const Tuple& tuple) const
{
    return memcmp(mTuple, tuple.mTuple, DIMENSION*sizeof(TYPE)) == 0;
}
//----------------------------------------------------------------------------
template <int DIMENSION, typename TYPE>
bool Tuple<DIMENSION,TYPE>::operator!= (const Tuple& tuple) const
{
    return memcmp(mTuple, tuple.mTuple, DIMENSION*sizeof(TYPE)) != 0;
}
//----------------------------------------------------------------------------
template <int DIMENSION, typename TYPE>
bool Tuple<DIMENSION,TYPE>::operator< (const Tuple& tuple) const
{
    return memcmp(mTuple, tuple.mTuple, DIMENSION*sizeof(TYPE)) < 0;
}
//----------------------------------------------------------------------------
template <int DIMENSION, typename TYPE>
bool Tuple<DIMENSION,TYPE>::operator<= (const Tuple& tuple) const
{
    return memcmp(mTuple, tuple.mTuple, DIMENSION*sizeof(TYPE)) <= 0;
}
//----------------------------------------------------------------------------
template <int DIMENSION, typename TYPE>
bool Tuple<DIMENSION,TYPE>::operator> (const Tuple& tuple) const
{
    return memcmp(mTuple, tuple.mTuple, DIMENSION*sizeof(TYPE)) > 0;
}
//----------------------------------------------------------------------------
template <int DIMENSION, typename TYPE>
bool Tuple<DIMENSION,TYPE>::operator>= (const Tuple& tuple) const
{
    return memcmp(mTuple, tuple.mTuple, DIMENSION*sizeof(TYPE)) >= 0;
}
//----------------------------------------------------------------------------
