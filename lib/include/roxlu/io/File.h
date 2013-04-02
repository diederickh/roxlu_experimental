#ifndef ROXLU_FILEH
#define ROXLU_FILEH

#include <string>
#include <fstream>
#include <vector>
#include <cstdlib>
#include <roxlu/core/platform/Platform.h>
#include <roxlu/core/Log.h>
#include <sys/stat.h>
#include <stdio.h>
#include <errno.h>
#include <string.h> /* errno on linux */

#if defined(_WIN32)
#   include <roxlu/external/dirent.h> 
#else
#   include <dirent.h>
#endif


#if ROXLU_PLATFORM == ROXLU_APPLE || ROXLU_PLATFORM == ROXLU_IOS
#  include <TargetConditionals.h>
#  include <mach-o/dyld.h>
#  include <unistd.h> // getcwd(), access()

#elif ROXLU_PLATFORM == ROXLU_WINDOWS
#  include <windows.h>
#  include <direct.h> // _mkdir
#  include <Shlwapi.h>

#elif ROXLU_PLATFORM == ROXLU_LINUX
#  include <unistd.h> // getcwd

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
        RX_ERROR("File: cannot open file: '%s'", file.c_str());
        return;
      }
      of.write(contents.c_str(), contents.length());
      of.close();
    }

    // @todo > switch over to rx_get_file_contents
    static string getFileContents(string file, bool inDataPath = true) {
      if(inDataPath) {
        file = File::toDataPath(file);
      }
      std::string result = "";
      std::string line = "";
      std::ifstream ifs(file.c_str());
      if(!ifs.is_open()) {
        RX_ERROR("File: cannot open file: '%s'", file.c_str());
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
	
    static rx_int64 getTimeModified(string filepath) {
      struct stat st;
      if(stat(filepath.c_str(), &st) == 0) {
        return st.st_mtime;
      }
      else {
        return 0;
      }
      return 0;
    }

    static bool remove(std::string filepath) {
      if(::remove(filepath.c_str()) != 0) {
        RX_ERROR("cannot remove file: %s - %s", filepath.c_str(), strerror(errno));
        return false;
      }
      return true;
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
    // @deprecated use rx_create_path
    static bool createPath(std::string path) {

#ifdef _WIN32
      std::string drive;
      for(int i = 0; i < path.size()-1; ++i) {
        if(path[i] == ':' && path[i + 1] == '\\') {
          break;
        }
        drive.push_back(path[i]);
      }
      path = path.substr(drive.size() + 2);
      drive = drive + ":";

#endif

      std::vector<std::string> dirs;
      while(path.length() > 0) {

     
#ifdef _WIN32
        int index = path.find('\\'); // win 
#else
        int index = path.find('/'); // posix
#endif
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
#ifdef _WIN32
      dir_path = drive;
#endif
      for(unsigned int i = 0; i < dirs.size(); i++) {
#ifdef _WIN32
        dir_path += "\\";
#else
        dir_path += "/";
#endif

        dir_path += dirs[i];
        if(stat(dir_path.c_str(), &s) != 0) {
          if(!File::createDirectory(dir_path.c_str())) {
            RX_ERROR("ERROR: cannot create directory: %s", dir_path.c_str());
            return false;
          }
        }
      }
      return true;
    }

    // @deprecated use rx_create_dir()
    static bool createDirectory(std::string path) {
#ifdef _WIN32
      if(_mkdir(path.c_str()) != 0) {
        if(errno == ENOENT) { 
          RX_ERROR("Cannot create directory: %s (ENOENT)", path.c_str());
          return false;
        }
        else if(errno == EEXIST) {
          RX_ERROR("Cannot create directory: %s (EEXIST)", path.c_str());
        }
      }
      return true;

#else
      if(mkdir(path.c_str(), 0777) != 0) {
        return false;
      }
      return true;
#endif
    }

    // GetFileAttributes may want a wide char as param which obviously is not what we support.
#if defined(_MSC_VER) && defined(UNICODE)
#  error "Your project is setup for unicode builds which is not supported. Change your project settings: Configuration Properties > General > Character Set: Not Set "
#endif

    static bool exists(std::string filepath) {
#if defined(_WIN32)
      char* lptr = (char*)filepath.c_str();
      DWORD dwattrib = GetFileAttributes(lptr);
      return (dwattrib != INVALID_FILE_ATTRIBUTES && !(dwattrib & FILE_ATTRIBUTE_DIRECTORY));

#elif defined(__APPLE__)
      int res = access(filepath.c_str(), R_OK);
      if(res < 0) {
        return false;
      }
#endif

      return true;
    }
      
    
    static std::vector<std::string> getDirectories(std::string path) {
      std::vector<std::string> result;
      DIR* dir;
      struct dirent* ent;
      if((dir = opendir(path.c_str())) != NULL) {
        while((ent = readdir(dir)) != NULL) {
          if(ent->d_type == DT_DIR) {
            if(strcmp(ent->d_name, ".") == 0 || strcmp(ent->d_name, "..") == 0) {
              continue;
            }
            result.push_back(ent->d_name);
          }
        }
        closedir(dir);
      }
      return result;
    }
    
    static std::vector<std::string> getFiles(std::string path) {
      std::vector<std::string> result;
      DIR* dir;
      struct dirent* ent;
      if((dir = opendir(path.c_str())) != NULL) {
        while((ent = readdir(dir)) != NULL) {
          if(ent->d_type == DT_REG) {
            result.push_back(path +"/" +ent->d_name);
          }
        }
        closedir(dir);
      }
      return result;
    }



  }; // File
} // roxlu

#endif
