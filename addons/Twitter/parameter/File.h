#ifndef ROXLU_TWITTER_FILEH
#define ROXLU_TWITTER_FILEH

#include <string>
#include <sstream>
#include "Parameter.h"

namespace roxlu {
namespace twitter {
namespace parameter {

namespace rtp = roxlu::twitter::parameter;

class File : public rtp::Parameter {
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




}}} // roxlu::twitter::parameter
#endif