#ifndef ROXLU_FILEH
#define ROXLU_FILEH

#include <string>
//#include <unistd.h>

using std::string;

namespace roxlu {
class File {
public:
	File();
	~File();
	
		
	static string toDataPath(const char* file) {
		return getCWD() +"/" +file;
	}
	
	static string getCWD() {
		char buf[1024];
		return getcwd(buf, 1024);		
	}
};
}

#endif