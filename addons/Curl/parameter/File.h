#ifndef ROXLU_TWITTER_FILEH
#define ROXLU_TWITTER_FILEH

#include <string>
#include <sstream>
#include <fstream>
#include "Parameter.h"

namespace roxlu {
namespace curl {
namespace parameter {

namespace rcp = roxlu::curl::parameter;

class File : public rcp::Parameter {
public:	
	File(const std::string& n, const std::string& filePath);
	File(File* other);
	~File();
	
	virtual std::string getStringValue();
	virtual bool mustUseInSignature();
	bool operator==(const File& other) const;
	void print();
	
private:
	std::string file_path;
};

inline bool File::operator==(const File& other) const {
	return (name == other.name && file_path == other.file_path);
}




}}} // roxlu::curl::parameter
#endif