#ifndef ROXLU_UTILSH
#define ROXLU_UTILSH

//#include "pstdint.h" /* Replace with <stdint.h> if appropriate */
#include <stdint.h>
#undef get16bits
#if (defined(__GNUC__) && defined(__i386__)) || defined(__WATCOMC__) \
|| defined(_MSC_VER) || defined (__BORLANDC__) || defined (__TURBOC__)
#define get16bits(d) (*((const uint16_t *) (d)))
#endif

#if !defined (get16bits)
#define get16bits(d) ((((uint32_t)(((const uint8_t *)(d))[1])) << 8)\
+(uint32_t)(((const uint8_t *)(d))[0]) )
#endif

extern uint32_t gl_string_id(const char * data, int len);

#define EPSILON 0.000001
#define IS_ZERO(f) 	(fabs(f) < EPSILON)	

#endif