// Geometric Tools, LLC
// Copyright (c) 1998-2011
// Distributed under the Boost Software License, Version 1.0.
// http://www.boost.org/LICENSE_1_0.txt
// http://www.geometrictools.com/License/Boost/LICENSE_1_0.txt
//
// File Version: 5.0.1 (2010/09/19)

//----------------------------------------------------------------------------
#ifdef WM5_USE_MEMORY
//----------------------------------------------------------------------------
template <typename T>
T* Memory::New1 (const int bound0)
{
    T* data;

    if (msMap)
    {
        // Insert T[] into memory map.
        data = (T*)CreateBlock(bound0*sizeof(T), 1);

        // Call the default constructors for T.
        T* object = data;
        for (int i = 0; i < bound0; ++i, ++object)
        {
            ::new(object) T;
        }
    }
    else
    {
#ifdef WM5_USE_MEMORY_ASSERT_ON_PREINIT_POSTTERM_OPERATIONS
        assertion(false, "Allocations are not tracked outside a "
            "Memory::Initialize/Memory::Terminate block.\n");
#endif
        data = new T[bound0];
    }

    return data;
}
//----------------------------------------------------------------------------
template <typename T>
T** Memory::New2 (const int bound0, const int bound1)
{
    const int bound01 = bound0*bound1;
    T** data;

    if (msMap)
    {
        // No constructor for T*, insert T*[] into memory map.
        data = (T**)CreateBlock(bound1*sizeof(T*), 2);

        // Insert T[] into memory map.
        data[0] = (T*)CreateBlock(bound01*sizeof(T), 1);

        // Call default constructors for T.  If T is a pointer type, the
        // compiler will not generate any code for the constructor call.
        T* object = data[0];
        for (int i = 0; i < bound01; ++i, ++object)
        {
            ::new(object) T;
        }
    }
    else
    {
#ifdef WM5_USE_MEMORY_ASSERT_ON_PREINIT_POSTTERM_OPERATIONS
        assertion(false, "Allocations are not tracked outside a "
            "Memory::Initialize/Memory::Terminate block.\n");
#endif
        data = new T*[bound1];
        data[0] = new T[bound01];
    }

    // Hook up the pointers to form the 2D array.
    for (int i1 = 1; i1 < bound1; ++i1)
    {
        int j0 = bound0*i1;  // = bound0*(i1 + j1) where j1 = 0
        data[i1] = &data[0][j0];
    }

    return data;
}
//----------------------------------------------------------------------------
template <typename T>
T*** Memory::New3 (const int bound0, const int bound1, const int bound2)
{
    const int bound12 = bound1*bound2;
    const int bound012 = bound0*bound12;
    T*** data;

    if (msMap)
    {
        // No constructor for T**, insert T**[] into memory map.
        data = (T***)CreateBlock(bound2*sizeof(T**), 3);

        // No constructor for T*, insert T*[] into memory map.
        data[0] = (T**)CreateBlock(bound12*sizeof(T*), 2);

        // Insert T[] into memory map.
        data[0][0] = (T*)CreateBlock(bound012*sizeof(T), 1);

        // Call default constructors for T.  If T is a pointer type, the
        // compiler will not generate any code for the constructor call.
        T* object = data[0][0];
        for (int i = 0; i < bound012; ++i, ++object)
        {
            ::new(object) T;
        }
    }
    else
    {
#ifdef WM5_USE_MEMORY_ASSERT_ON_PREINIT_POSTTERM_OPERATIONS
        assertion(false, "Allocations are not tracked outside a "
            "Memory::Initialize/Memory::Terminate block.\n");
#endif
        data = new T**[bound2];
        data[0] = new T*[bound12];
        data[0][0] = new T[bound012];
    }

    // Hook up the pointers to form the 3D array.
    for (int i2 = 0; i2 < bound2; ++i2)
    {
        int j1 = bound1*i2;  // = bound1*(i2 + j2) where j2 = 0
        data[i2] = &data[0][j1];
        for (int i1 = 0; i1 < bound1; ++i1)
        {
            int j0 = bound0*(i1 + j1);
            data[i2][i1] = &data[0][0][j0];
        }
    }

    return data;
}
//----------------------------------------------------------------------------
template <typename T>
T**** Memory::New4 (const int bound0, const int bound1, const int bound2,
    const int bound3)
{
    const int bound23 = bound2*bound3;
    const int bound123 = bound1*bound23;
    const int bound0123 = bound0*bound123;
    T**** data;

    if (msMap)
    {
        // No constructor for T***, insert T***[] into memory map.
        data = (T****)CreateBlock(bound3*sizeof(T***), 4);

        // No constructor for T**, insert T**[] into memory map.
        data[0] = (T***)CreateBlock(bound23*sizeof(T**), 3);

        // No constructor for T*, insert T*[] into memory map.
        data[0][0] = (T**)CreateBlock(bound123*sizeof(T*), 2);

        // Insert T[] into memory map.
        data[0][0][0] = (T*)CreateBlock(bound0123*sizeof(T), 1);

        // Call default constructors for T.  If T is a pointer type, the
        // compiler will not generate any code for the constructor call.
        T* object = data[0][0][0];
        for (int i = 0; i < bound0123; ++i, ++object)
        {
            ::new(object) T;
        }
    }
    else
    {
#ifdef WM5_USE_MEMORY_ASSERT_ON_PREINIT_POSTTERM_OPERATIONS
        assertion(false, "Allocations are not tracked outside a "
            "Memory::Initialize/Memory::Terminate block.\n");
#endif
        data = new T***[bound3];
        data[0] = new T**[bound23];
        data[0][0] = new T*[bound123];
        data[0][0][0] = new T[bound0123];
    }

    // Hook up the pointers to form the 4D array.
    for (int i3 = 0; i3 < bound3; ++i3)
    {
        int j2 = bound2*i3;  // = bound2*(i3 + j3) where j3 = 0
        data[i3] = &data[0][j2];
        for (int i2 = 0; i2 < bound2; ++i2)
        {
            int j1 = bound1*(i2 + j2);
            data[i3][i2] = &data[0][0][j1];
            for (int i1 = 0; i1 < bound1; ++i1)
            {
                int j0 = bound0*(i1 + j1);
                data[i3][i2][i1] = &data[0][0][0][j0];
            }
        }
    }

    return data;
}
//----------------------------------------------------------------------------
template <typename T>
void Memory::Delete0 (T*& data)
{
    if (data)
    {
        if (!msMap)
        {
#ifdef WM5_USE_MEMORY_ASSERT_ON_PREINIT_POSTTERM_OPERATIONS
            assertion(false, "Deallocations are not tracked outside a "
                "Memory::Initialize/Memory::Terminate block.\n");
#endif
            delete data;
            data = 0;
            return;
        }

        msMutex.Enter();

        MemoryMap::iterator iter = msMap->find(data);
        if (iter != msMap->end())
        {
            if (iter->second.mNumDimensions == 0)
            {
                // Call destructor for T.  If T is a pointer type, the
                // compiler will not generate any code for the destructor
                // call.
                data->~T();

                // Remove T from memory map.
                msMap->erase(data);
                msDeallocator(data, mFile, mLine);
            }
            else
            {
                assertion(false, "Mismatch in dimensions.\n");
            }
        }
        else
        {
#ifdef WM5_USE_MEMORY_ALLOW_DELETE_ON_FAILED_MAP_LOOKUP
            delete data;
#else
            assertion(false, "Memory block not in map.\n");
#endif
        }

        data = 0;

        msMutex.Leave();
    }
}
//----------------------------------------------------------------------------
template <typename T>
void Memory::Delete1 (T*& data)
{
    if (data)
    {
        if (!msMap)
        {
#ifdef WM5_USE_MEMORY_ASSERT_ON_PREINIT_POSTTERM_OPERATIONS
            assertion(false, "Deallocations are not tracked outside a "
                "Memory::Initialize/Memory::Terminate block.\n");
#endif
            delete[] data;
            data = 0;
            return;
        }

        msMutex.Enter();

        MemoryMap::iterator iter = msMap->find(data);
        if (iter != msMap->end())
        {
            if (iter->second.mNumDimensions == 1)
            {
                // Call destructor for T.  If T is a pointer type, the
                // compiler will not generate any code for the destructor
                // call.
                const int numElements = iter->second.mNumBytes/sizeof(T);
                T* object = data;
                for (int i = 0; i < numElements; ++i, ++object)
                {
                    object->~T();
                }

                // Remove T[] from memory map.
                msMap->erase(data);
                msDeallocator(data, mFile, mLine);
            }
            else
            {
                assertion(false, "Mismatch in dimensions.\n");
            }
        }
        else
        {
#ifdef WM5_USE_MEMORY_ALLOW_DELETE_ON_FAILED_MAP_LOOKUP
            delete[] data;
#else
            assertion(false, "Memory block not in map.\n");
#endif
        }

        data = 0;

        msMutex.Leave();
    }
}
//----------------------------------------------------------------------------
template <typename T>
void Memory::Delete2 (T**& data)
{
    if (data)
    {
        if (!msMap)
        {
#ifdef WM5_USE_MEMORY_ASSERT_ON_PREINIT_POSTTERM_OPERATIONS
            assertion(false, "Deallocations are not tracked outside a "
                "Memory::Initialize/Memory::Terminate block.\n");
#endif
            delete[] data[0];
            delete[] data;
            data = 0;
            return;
        }

        msMutex.Enter();

        MemoryMap::iterator iter = msMap->find(data);
        if (iter != msMap->end())
        {
            if (iter->second.mNumDimensions == 2)
            {
                // Call destructor for T.  If T is a pointer type, the
                // compiler will not generate any code for the destructor
                // call.
                const int numElements = iter->second.mNumBytes/sizeof(T);
                T* object = data[0];
                for (int i = 0; i < numElements; ++i, ++object)
                {
                    object->~T();
                }

                // Remove T[] from memory map.
                msMap->erase(data[0]);
                msDeallocator(data[0], mFile, mLine);

                // No destructor for T*, remove T*[] from memory map.
                msMap->erase(data);
                msDeallocator(data, mFile, mLine);
            }
            else
            {
                assertion(false, "Mismatch in dimensions.\n");
            }
        }
        else
        {
#ifdef WM5_USE_MEMORY_ALLOW_DELETE_ON_FAILED_MAP_LOOKUP
            delete[] data[0];
            delete[] data;
#else
            assertion(false, "Memory block not in map.\n");
#endif
        }

        data = 0;

        msMutex.Leave();
    }
}
//----------------------------------------------------------------------------
template <typename T>
void Memory::Delete3 (T***& data)
{
    if (data)
    {
        if (!msMap)
        {
#ifdef WM5_USE_MEMORY_ASSERT_ON_PREINIT_POSTTERM_OPERATIONS
            assertion(false, "Deallocations are not tracked outside a "
                "Memory::Initialize/Memory::Terminate block.\n");
#endif
            delete[] data[0][0];
            delete[] data[0];
            delete[] data;
            data = 0;
            return;
        }

        msMutex.Enter();

        MemoryMap::iterator iter = msMap->find(data);
        if (iter != msMap->end())
        {
            if (iter->second.mNumDimensions == 3)
            {
                // Call destructor for T.  If T is a pointer type, the
                // compiler will not generate any code for the destructor
                // call.
                const int numElements = iter->second.mNumBytes/sizeof(T);
                T* object = data[0][0];
                for (int i = 0; i < numElements; ++i, ++object)
                {
                    object->~T();
                }

                // Remove T[] from memory map.
                msMap->erase(data[0][0]);
                msDeallocator(data[0][0], mFile, mLine);

                // No destructor for T*, remove T*[] from memory map.
                msMap->erase(data[0]);
                msDeallocator(data[0], mFile, mLine);

                // No destructor for T**, remove T**[] from memory map.
                msMap->erase(data);
                msDeallocator(data, mFile, mLine);
            }
            else
            {
                assertion(false, "Mismatch in dimensions.\n");
            }
        }
        else
        {
#ifdef WM5_USE_MEMORY_ALLOW_DELETE_ON_FAILED_MAP_LOOKUP
            delete[] data[0][0];
            delete[] data[0];
            delete[] data;
#else
            assertion(false, "Memory block not in map.\n");
#endif
        }

        data = 0;

        msMutex.Leave();
    }
}
//----------------------------------------------------------------------------
template <typename T>
void Memory::Delete4 (T****& data)
{
    if (data)
    {
        if (!msMap)
        {
#ifdef WM5_USE_MEMORY_ASSERT_ON_PREINIT_POSTTERM_OPERATIONS
            assertion(false, "Deallocations are not tracked outside a "
                "Memory::Initialize/Memory::Terminate block.\n");
#endif
            delete[] data[0][0][0];
            delete[] data[0][0];
            delete[] data[0];
            delete[] data;
            data = 0;
            return;
        }

        msMutex.Enter();

        MemoryMap::iterator iter = msMap->find(data);
        if (iter != msMap->end())
        {
            if (iter->second.mNumDimensions == 4)
            {
                // Call destructor for T.  If T is a pointer type, the
                // compiler will not generate any code for the destructor
                // call.
                const int numElements = iter->second.mNumBytes/sizeof(T);
                T* object = data[0][0][0];
                for (int i = 0; i < numElements; ++i, ++object)
                {
                    object->~T();
                }

                // Remove T[] from memory map.
                msMap->erase(data[0][0][0]);
                msDeallocator(data[0][0][0], mFile, mLine);

                // No destructor for T*, remove T*[] from memory map.
                msMap->erase(data[0][0]);
                msDeallocator(data[0][0], mFile, mLine);

                // No destructor for T**, remove T**[] from memory map.
                msMap->erase(data[0]);
                msDeallocator(data[0], mFile, mLine);

                // No destructor for T***, remove T***[] from memory map.
                msMap->erase(data);
                msDeallocator(data, mFile, mLine);
            }
            else
            {
                assertion(false, "Mismatch in dimensions.\n");
            }
        }
        else
        {
#ifdef WM5_USE_MEMORY_ALLOW_DELETE_ON_FAILED_MAP_LOOKUP
            delete[] data[0][0][0];
            delete[] data[0][0];
            delete[] data[0];
            delete[] data;
#else
            assertion(false, "Memory block not in map.\n");
#endif
        }

        data = 0;

        msMutex.Leave();
    }
}
//----------------------------------------------------------------------------
#else
//----------------------------------------------------------------------------
template <typename T>
T* new1 (const int bound0)
{
    return new T[bound0];
}
//----------------------------------------------------------------------------
template <typename T>
T** new2 (const int bound0, const int bound1)
{
    const int bound01 = bound0*bound1;
    T** data = new T*[bound1];
    data[0] = new T[bound01];

    for (int i1 = 1; i1 < bound1; ++i1)
    {
        int j0 = bound0*i1;  // = bound0*(i1 + j1) where j1 = 0
        data[i1] = &data[0][j0];
    }
    return data;
}
//----------------------------------------------------------------------------
template <typename T>
T*** new3 (const int bound0, const int bound1, const int bound2)
{
    const int bound12 = bound1*bound2;
    const int bound012 = bound0*bound12;
    T*** data = new T**[bound2];
    data[0] = new T*[bound12];
    data[0][0] = new T[bound012];

    for (int i2 = 0; i2 < bound2; ++i2)
    {
        int j1 = bound1*i2;  // = bound1*(i2 + j2) where j2 = 0
        data[i2] = &data[0][j1];
        for (int i1 = 0; i1 < bound1; ++i1)
        {
            int j0 = bound0*(i1 + j1);
            data[i2][i1] = &data[0][0][j0];
        }
    }
    return data;
}
//----------------------------------------------------------------------------
template <typename T>
T**** new4 (const int bound0, const int bound1, const int bound2,
    const int bound3)
{
    const int bound23 = bound2*bound3;
    const int bound123 = bound1*bound23;
    const int bound0123 = bound0*bound123;
    T**** data = new T***[bound3];
    data[0] = new T**[bound23];
    data[0][0] = new T*[bound123];
    data[0][0][0] = new T[bound0123];

    for (int i3 = 0; i3 < bound3; ++i3)
    {
        int j2 = bound2*i3;  // = bound2*(i3 + j3) where j3 = 0
        data[i3] = &data[0][j2];
        for (int i2 = 0; i2 < bound2; ++i2)
        {
            int j1 = bound1*(i2 + j2);
            data[i3][i2] = &data[0][0][j1];
            for (int i1 = 0; i1 < bound1; ++i1)
            {
                int j0 = bound0*(i1 + j1);
                data[i3][i2][i1] = &data[0][0][0][j0];
            }
        }
    }
    return data;
}
//----------------------------------------------------------------------------
template <typename T>
void delete0 (T*& data)
{
    delete data;
    data = 0;
}
//----------------------------------------------------------------------------
template <typename T>
void delete1 (T*& data)
{
    delete[] data;
    data = 0;
}
//----------------------------------------------------------------------------
template <typename T>
void delete2 (T**& data)
{
    if (data)
    {
        delete[] data[0];
        delete[] data;
        data = 0;
    }
}
//----------------------------------------------------------------------------
template <typename T>
void delete3 (T***& data)
{
    if (data)
    {
        delete[] data[0][0];
        delete[] data[0];
        delete[] data;
        data = 0;
    }
}
//----------------------------------------------------------------------------
template <typename T>
void delete4 (T****& data)
{
    if (data)
    {
        delete[] data[0][0][0];
        delete[] data[0][0];
        delete[] data[0];
        delete[] data;
        data = 0;
    }
}
//----------------------------------------------------------------------------
#endif
//----------------------------------------------------------------------------
