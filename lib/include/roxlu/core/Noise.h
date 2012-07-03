#ifndef ROXLU_NOISEH
#define ROXLU_NOISEH

//#include "Vec2.h"
#include <roxlu/core/platform/Platform.h>
#include <roxlu/math/Vec2.h>

namespace roxlu {

#ifdef __cplusplus
extern "C" {
#endif

float noise1( float x );
float noise2( float x, float y );
float noise3( float x, float y, float z );
float noise4( float x, float y, float z, float w );
float fbm(const Vec2& v);

#ifdef __cplusplus
}

#endif
}; // roxlu

#endif