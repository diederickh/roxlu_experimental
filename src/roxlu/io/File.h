#ifndef ROXLU_FILEH
#define ROXLU_FILEH

#include <string>
#include <fstream>
#include <cstdlib>

//#include <unistd.h>

// NO POCO!
//#include "Poco/File.h"
//#include "Poco/Timestamp.h"

#ifdef __APPLE__
	#include <TargetConditionals.h>
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
	
	static time_t getTimeModified(string file, bool inDataPath = true) {
		if(inDataPath) {
			file = File::toDataPath(file);
		}
		printf("NOOOOOO\n");
//		Poco::File f(file);
//		Poco::Timestamp stamp = f.getLastModified();
//		return stamp.epochTime();
	}
	
	static string getCWD() {
		char buf[1024];
		return getcwd(buf, 1024);		
	}
};
}

#endif