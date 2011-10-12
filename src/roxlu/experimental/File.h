#ifndef ROXLU_FILEH
#define ROXLU_FILEH

#include <string>
//#include <unistd.h>

#ifdef __APPLE__
	#include <TargetConditionals.h>
#endif

using std::string;

namespace roxlu {
class File {
public:
	File();
	~File();
	
	static string toDataPath(string file) {
		return toDataPath(file.c_str());
	}	
	
	static string toDataPath(const char* file) {
		#ifdef __APPLE__
			#ifdef TARGET_OS_MAC 
				return getCWD() +"/../../../data/" +file;
				//return getCWD() +"/data/" +file;
			#else
				return getCWD() +"/../../../data/" +file;
			#endif
		#else 
			return getCWD() +"/" +file;
		#endif
	}
	
	static string getCWD() {
		char buf[1024];
		return getcwd(buf, 1024);		
	}
};
}

#endif