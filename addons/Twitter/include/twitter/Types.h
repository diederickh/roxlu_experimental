#ifndef ROXLU_TWITTER_TYPESH
#define ROXLU_TWITTER_TYPESH

#include <string>
#include <vector>
#include <algorithm> 
#include <functional> 
#include <cctype>
#include <locale>

namespace roxlu {
  namespace twitter {

    // INTEGER TYPES
    // --------------
#ifndef u64
#ifdef _WIN32
    typedef unsigned __int64   u64;
#else
    typedef unsigned long long u64;
#endif
#endif

    // NOW IN MILLIS
    // -------------
    inline u64 now() {
#ifndef _WIN32
      timeval time;
      gettimeofday(&time, NULL);
      u64 n = time.tv_usec;
      n /= 1000; 
      n += (time.tv_sec * 1000); 
      return n;
#else
      return GetTickCount();
#endif
    }

    // EPOCH TIME STAMP
    // ------------------
    inline u64 timestamp() {
#ifndef _WIN32
      u64 t = time(0);
      return t;
#else
      static LARGE_INTEGER s_frequency;
      static BOOL s_use_qpc = QueryPerformanceFrequency(&s_frequency);
      if(s_use_qpc) {
        LARGE_INTEGER now;
        QueryPerformanceCounter(&now);
        return (1000LL * now.QuadPart) / s_frequency.QuadPart;
      } 
      else {
        return GetTickCount();
      }
      //#error "Need to implement timestamp() on windows";
#endif
    }


    // STRING UTILITIES
    // -----------------
    static inline std::string &ltrim(std::string &s) {
      s.erase(s.begin(), std::find_if(s.begin(), s.end(), std::not1(std::ptr_fun<int, int>(std::isspace))));
      return s;
    }

    static inline std::string &rtrim(std::string &s) {
      s.erase(std::find_if(s.rbegin(), s.rend(), std::not1(std::ptr_fun<int, int>(std::isspace))).base(), s.end());
      return s;
    }

    static inline std::string &trim(std::string &s) {
      return ltrim(rtrim(s));
    }

    inline void split(const std::string& str, const std::string& delim, std::vector<std::string>& result, bool removeSpaces = true) {
      size_t start = 0, end = 0;
      while(end != std::string::npos) {
        end = str.find(delim, start);
        result.push_back(str.substr(start, (end == std::string::npos) ? std::string::npos : end - start));
        if(removeSpaces) {
          trim(result.back());
        }
        start = ((end > (std::string::npos - delim.size())) ? std::string::npos : end + delim.size());
      }
    }

    inline std::string join(const std::vector<std::string>& words, std::string delim) {
      size_t s = words.size();
      std::string result;
      for(size_t i = 0; i < s; ++i) {
        result += words[i];
        if(i != s-1) {
          result += delim;
        }
      }
      return result;
    }

  } // roxlu
} // twitter
#endif
