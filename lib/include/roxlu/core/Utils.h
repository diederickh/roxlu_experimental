#ifndef ROXLU_UTILSH
#define ROXLU_UTILSH

//#include "pstdint.h" /* Replace with <stdint.h> if appropriate */
//#include "Constants.h"

#include <roxlu/core/Constants.h>
#include <stdio.h>
#include <time.h>
#include <vector>
#include <sstream>

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


// as suggested: http://stackoverflow.com/questions/4100657/problem-with-my-clamp-macro
template <typename T> 
inline T rx_clamp(T value, T low, T high) {
    return (value < low) ? low : ((value > high) ? high : value);
}


template<typename T>
inline T rx_map(T value, T minIn, T maxIn, T minOut, T maxOut) {
	T range = ((value - minIn) / (maxIn - minIn) * (maxOut - minOut) + minOut);
	return range;
}

// as described in: "From Quaternion to Matrix and Back", J.M.P. van Waveren, 27th feb. 2005, id software
static float rx_fast_sqrt(float x) {
    long i; 
    float y, r; 
    y = x * 0.5f; 
    i = *(long *)( &x ); 
    i = 0x5f3759df - ( i >> 1 ); 
    r = *(float *)( &i ); 
    r = r * ( 1.5f - r * r * y ); 
    return r; 
}

// rx_strftime("%Y/%m%d"), http://www.cplusplus.com/reference/clibrary/ctime/strftime/
inline std::string rx_strftime(const char* timestr) {
  time_t t;
  struct tm* info;
  char buf[4096]; // must be enough..
  time(&t);
  info = localtime(&t);
  strftime(buf, 4096, timestr, info);
  std::string result(buf);
  return result;
}

template<class T>
static std::string rx_join(const std::vector<T>& entries, std::string sep) {
  std::string result;
  for(typename  std::vector<T>::const_iterator it = entries.begin(); it != entries.end(); ++it) {
    std::stringstream ss; 
    ss << *it;
    result.append(ss.str());
    if(it + 1 != entries.end()) {
      result.append(sep);
    }
  }
  return result;
}

#endif
