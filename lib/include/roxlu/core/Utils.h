#ifndef ROXLU_UTILSH
#define ROXLU_UTILSH

#include <roxlu/opengl/GL.h>
#include <roxlu/opengl/Error.h>
#include <roxlu/core/Constants.h>
#include <roxlu/core/Log.h>
#include <stdio.h>
#include <time.h>
#include <string.h>
#include <vector>
#include <sstream>
#include <fstream>
#include <algorithm> /* std::replace() */

#if defined(_WIN32)
#   include <roxlu/external/dirent.h> /* for stat() */  
#else
#   include <dirent.h> /* for stat() */
#   include <errno.h> /* for errno */
#endif


// @todo maybe add this stuff to "Platform.h" 
#if defined(__APPLE__)
#  include <libgen.h> /* dirname */
#  include <CoreFoundation/CFRunLoop.h>
#  include <mach/mach.h>
#  include <mach/mach_time.h>
#  include <mach-o/dyld.h> /* _NSGetExecutablePath */
#  include <sys/resource.h>
#  include <sys/sysctl.h>
#  include <sys/stat.h> /* stat() */
#  include <unistd.h>  /* sysconf */
#elif defined(__linux) 
#  include <unistd.h> /* readlink */
#  include <sys/time.h> /* timeofday */
#  include <libgen.h> /* dirname */
#  define MAX_PATH 4096
#elif defined(_WIN32)
#  include <roxlu/core/platform/Platform.h>
#endif

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

inline void rx_sleep_millis(int millis) {
#ifdef _WIN32
  Sleep(millis);
#else
  usleep(millis * 1000);
#endif
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


#ifdef ROXLU_WITH_OPENGL

// Creates a vertex + frag shader and a program. 
// We do not yet link the program so you can set attribute locations
inline GLuint rx_create_shader(const char* vs, const char* fs) {
  GLuint vert_id = glCreateShader(GL_VERTEX_SHADER);
  GLuint frag_id = glCreateShader(GL_FRAGMENT_SHADER);
  glShaderSource(vert_id, 1, &vs, NULL);
  glShaderSource(frag_id, 1, &fs, NULL);
  glCompileShader(vert_id); eglGetShaderInfoLog(vert_id);
  glCompileShader(frag_id); eglGetShaderInfoLog(frag_id);
  GLuint prog = glCreateProgram();
  glAttachShader(prog, vert_id);
  glAttachShader(prog, frag_id);
  return prog;
}
#endif

// -------------------------------------- WIN ---------------------------------------
#ifdef _WIN32

static std::string rx_get_exe_path() {
  char buffer[MAX_PATH];

  // Try to get the executable path with a buffer of MAX_PATH characters.
  DWORD result = ::GetModuleFileNameA(nullptr, buffer, static_cast<DWORD>(MAX_PATH));
  if(result == 0) {
    return "";
  }

  std::string::size_type pos = std::string(buffer).find_last_of( "\\/" );

  return std::string(buffer).substr(0, pos) +"\\";
}

static rx_int64 rx_millis(void) {
  static LARGE_INTEGER s_frequency;
  static BOOL s_use_qpc = QueryPerformanceFrequency(&s_frequency);
  if (s_use_qpc) {
    LARGE_INTEGER now;
    QueryPerformanceCounter(&now);
    return (1000LL * now.QuadPart) / s_frequency.QuadPart;
  } 
  else {
    return GetTickCount();
  }
}

/* returns epoch timestamp */
static time_t rx_time() {
  return time(NULL);
}


// -------------------------------------- LINUX -------------------------------------
#elif defined(__linux) 


static std::string rx_get_exe_path() {
  char buffer[MAX_PATH];
  size_t size = MAX_PATH;
  ssize_t n = readlink("/proc/self/exe", buffer, size - 1);
  if (n <= 0) {
    return "";
  }
  buffer[n] = '\0';


  const char* dn = dirname(buffer);
  size = strlen(dn);
  std::string ret(dn, size) ;
  ret.push_back('/');
  return ret;
}

static int64_t rx_millis() {
  timeval time;
  gettimeofday(&time, NULL);
  int64_t n = time.tv_usec;
  n /= 1000; // convert seconds to millis
  n += (time.tv_sec * 1000); // convert micros to millis
  return n;
}

/* returns epoch timestamp */
static time_t rx_time() {
  return time(NULL);
}

// -------------------------------------- OSX ---------------------------------------
#elif defined(__APPLE__)
static std::string rx_get_exe_path() {
  char buffer[1024];
  uint32_t usize = sizeof(buffer);;

  int result;
  char* path;
  char* fullpath;

  result = _NSGetExecutablePath(buffer, &usize);
  if (result) {
    return "";
  }

  path = (char*)malloc(2 * PATH_MAX);
  fullpath = realpath(buffer, path);

  if (fullpath == NULL) {
    free(path);
    return "";
  }

  strncpy(buffer, fullpath, usize);
  free(fullpath);
  const char* dn = dirname(buffer);
  usize = strlen(dn);
  std::string ret(dn, usize) ;
  ret.push_back('/');
  return ret;
}

static int64_t rx_millis(void) {
  mach_timebase_info_data_t info;
  if (mach_timebase_info(&info) != KERN_SUCCESS) {
    abort();
  }
  return (mach_absolute_time() * info.numer / info.denom) / 1000000;
}

/* returns epoch timestamp */
static time_t rx_time() {
  return time(NULL);
}

#endif

// ---------------------------------------------------------------------------------
static bool rx_is_dir(std::string filepath) {
  struct stat st;
  int result = stat(filepath.c_str(), &st);

  if(result < 0) {
    if(errno == EACCES) {
      RX_ERROR(("EACCESS: no permission for: %s", filepath.c_str()));
    }
    else if(errno == EFAULT) {
      RX_ERROR(("EFAULT: bad address, for: %s", filepath.c_str()));
    }
    else if(errno == ELOOP) {
      RX_ERROR(("ELOOP: too many links, for: %s", filepath.c_str()));
    }
    else if(errno == ENAMETOOLONG) {
      RX_ERROR(("ENAMETOOLONG: for: %s", filepath.c_str()));
    }
    else if(errno == ENOENT) {
      // we expect this when the dir doesn't exist
      return false;
    }
    else if(errno == ENOMEM) {
      RX_ERROR(("ENOMEM: for: %s", filepath.c_str()));
    }
    else if(errno == ENOTDIR) {
      RX_ERROR(("ENOTDIR: for: %s", filepath.c_str()));
    }
    else if(errno == EOVERFLOW) {
      RX_ERROR(("EOVERFLOW: for: %s", filepath.c_str()));
    }

    return false;
  }

#if defined(__APPLE__) or defined(__linux__)
  return S_ISDIR(st.st_mode);
#else 
  return result == 0;
#endif  

}

static std::string rx_to_data_path(const std::string filename) {
  std::string exepath = rx_get_exe_path();

#if defined(__APPLE__)
  if(rx_is_dir(exepath +"data")) {
    exepath += "data/" +filename;
  }
  else if(rx_is_dir(exepath +"../MacOS")) {
    exepath += "../../../data/" +filename;
  }
#else 
  exepath += "data/" +filename;
#endif  

  return exepath;
}

static std::string rx_to_exe_path(std::string filename) {
  return rx_get_exe_path() +filename;
}

static std::string rx_get_file_contents(std::string filepath, bool datapath = false) {
  if(datapath) {
    filepath = rx_to_data_path(filepath);
  }

  std::string result = "";
  std::string line = "";
  std::ifstream ifs(filepath.c_str());
  if(!ifs.is_open()) {
    RX_ERROR(("Cannot open file: '%s'", filepath.c_str()));
    return result;
  }
  while(getline(ifs,line)) {
    result += line +"\n";
  }
  return result;
}

/* 
   double check that your window is really APP_WIDTH and APP_HEIGHT!
   rx_ortho(0, APP_WIDTH, APP_HEIGHT, 0, -1.0, 1.0, pm);
 */
static void rx_ortho(float l, float r, float b, float t, float n, float f, float* dest) {
  dest[0] = (2.0f / (r - l));
  dest[1] = 0.0f;
  dest[2] = 0.0f;
  dest[3] = 0.0f;

  dest[4] = 0.0f;
  dest[5] = (2.0f / (t - b));
  dest[6] = 0.0f;
  dest[7] = 0.0f;
  
  dest[8] = 0.0f; 
  dest[9] = 0.0f;
  dest[10] = (-2.0f / (f - n));
  dest[11] = 0.0f;

  dest[12] = - ((r + l) / (r - l));
  dest[13] = - ((t + b) / (t - b));
  dest[14] = - ((f + n) / (f - n));
  dest[15] = 1.0f;
}

static size_t rx_string_to_sizet(std::string str) {
  size_t result = 0;
  std::stringstream ss;
  ss << str;
  ss >>  result;
  return result;
}

static int rx_string_to_int(std::string str) {
  int result = 0;
  std::stringstream ss;
  ss << str;
  ss >>  result;
  return result;
}


static std::string rx_int_to_string(int num) {
  std::string str;
  std::stringstream ss;
  ss << num;
  return ss.str();
}

static std::string rx_string_replace(std::string str, char from, char to) {
  std::replace(str.begin(), str.end(), from, to);
  return str;
}

static std::string rx_strip_filename(std::string path) {
  std::string directory;
  path = rx_string_replace(path, '\\', '/');
  const size_t last_slash_idx = path.rfind('/');

  if(std::string::npos != last_slash_idx) {
    directory = path.substr(0, last_slash_idx + 1);
  }

#if defined(_WIN32)
  directory = rx_string_replace(directory, '/', '\\');
#endif

  return directory;
}

#endif // ROXLU_UTILSH
