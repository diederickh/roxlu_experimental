#ifndef ROXLU_FILEH
#define ROXLU_FILEH

#include <string>
#include <fstream>
#include <vector>
#include <cstdlib>
#include <roxlu/core/platform/Platform.h>
#include <sys/stat.h>

#if ROXLU_PLATFORM == ROXLU_APPLE || ROXLU_PLATFORM == ROXLU_IOS
#include <TargetConditionals.h>
#include <mach-o/dyld.h>
#elif ROXLU_PLATFORM == ROXLU_WINDOWS
#include <windows.h>
#elif ROXLU_PLATFORM == ROXLU_LINUX
#include <unistd.h> // getcwd
#endif

using std::string;
using std::ofstream;

namespace roxlu {

	
  class File {
  public:
    File();
    ~File();
	
    static void putFileContents(string file, string contents, bool inDataPath = true) {
      ofstream of;
      if(inDataPath) {
        file = File::toDataPath(file);	
      }
		
      of.open(file.c_str(), std::ios::out);
      if(!of.is_open()) {
        printf("File: cannot open file: '%s'\n", file.c_str());
        return;
      }
      of.write(contents.c_str(), contents.length());
      of.close();
    }
	
    static string getFileContents(string file, bool inDataPath = true) {
      if(inDataPath) {
        file = File::toDataPath(file);
      }
      std::string result = "";
      std::string line = "";
      std::ifstream ifs(file.c_str());
      if(!ifs.is_open()) {
        printf("File: cannot open file: '%s'\n", file.c_str());
        return result;
      }
      while(getline(ifs,line)) {
        result += line +"\n";
      }
      return result;
    }
	
    static string toDataPath(string file) {
      return toDataPath(file.c_str());
    }	          
	
    static string toDataPath(const char* file) {
#if ROXLU_PLATFORM == ROXLU_APPLE
 #if ROXLU_GL_WRAPPER == ROXLU_OPENFRAMEWORKS
      return getCWD() +"/../../../data/" +file;	
      // #elif ROXLU_GL_WRAPPER == ROXLU_GLFW
 #else
      return getCWD() +"data/" +file;	
 #endif
#elif ROXLU_PLATFORM == ROXLU_IOS
      return getCWD() +file;
#elif ROXLU_PLATFORM == ROXLU_WINDOWS
      return getCWD() +"\\data\\" +file;
#elif ROXLU_PLATFORM == ROXLU_LINUX
      return getCWD() +"/data/" +file;
#else	
      return getCWD() +"/" +file;
#endif
    }
	
    static time_t getTimeModified(string file, bool inDataPath = true) {
      if(inDataPath) {
        file = File::toDataPath(file);
      }
      printf("MUST IMPLEMENT getTimeModified\n");
      return 0;
    }
	
    static string getCWD() {
#if ROXLU_PLATFORM == ROXLU_APPLE  || ROXLU_PLATFORM == ROXLU_IOS
      // hackedyhack
      char path[1024];
      uint32_t size = sizeof(path);
      if (_NSGetExecutablePath(path, &size) != 0) {
        printf("buffer too small; need size %u\n", size);
      }
      char res[1024];
      char* clean = realpath(path, res);
      if(clean) {
        string fullpath(clean);
        size_t pos = fullpath.find_last_of('/');
        if(pos != std::string::npos) {
          string noexe = fullpath.substr(0,pos+1);
          return noexe;
        }
      }
      return clean;
#elif ROXLU_PLATFORM == ROXLU_WINDOWS
      char buffer[MAX_PATH];
      GetModuleFileNameA( NULL, buffer, MAX_PATH );
      string::size_type pos = string( buffer ).find_last_of( "\\/" );
      return string( buffer ).substr( 0, pos);
#else
      char buf[1024];
      getcwd(buf, 1024);		
      return buf;
#endif
    }

    // e.g.: File::createPath("/users/home/roxlu/data/images/2012/12/05/")
    static bool createPath(std::string path) {
      std::vector<std::string> dirs;
      while(path.length() > 0) {
        int index = path.find('/');
        std::string dir = (index == -1 ) ? path : path.substr(0, index);
        if(dir.length() > 0) {
          dirs.push_back(dir);
        }
        if(index + 1 >= path.length() || index == -1) {
          break;
        }
        path = path.substr(index + 1);
      }
    
      struct stat s;
      std::string dir_path;
      for(unsigned int i = 0; i < dirs.size(); i++) {
        dir_path += "/";
        dir_path += dirs[i];
        if(stat(dir_path.c_str(), &s) != 0) {
          if(!File::createDirectory(dir_path.c_str())) {
            printf("ERROR: cannot create directory: %s\n", dir_path.c_str());
            return false;
          }
        }
      }
      return true;
    }

    static bool createDirectory(std::string path) {
      if(mkdir(path.c_str(), 0777) != 0) {
        return false;
      }
      return true;
    }
    
  }; // File
} // roxlu

#endif
