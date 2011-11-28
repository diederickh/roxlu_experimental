#ifndef ROXLU_NOISEH
#define ROXLU_NOISEH

namespace roxlu {

#ifdef __cplusplus
extern "C" {
#endif

float noise1( float x );
float noise2( float x, float y );
float noise3( float x, float y, float z );
float noise4( float x, float y, float z, float w );

#ifdef __cplusplus
}

#endif
}; // roxlu

#endif