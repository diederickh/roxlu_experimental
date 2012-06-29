#include "File.h"
namespace roxlu {
namespace curl {
namespace parameter {

namespace rcp = roxlu::curl::parameter;

File::File(const std::string& n, const std::string& f)
	:file_path(f)
{
	name = n;
	type = rcp::Parameter::PARAM_FILE;
	
	std::ifstream ifs(f.c_str());
	if(!ifs.is_open()) {
		printf("Error: cannot find file: '%s'; make sure to use complete path.\n", f.c_str());
	}
}

File::File(File* other) {
	name = other->name;
	type = other->type;
	file_path = other->file_path;
}

bool File::mustUseInSignature() {
	return false;
}

std::string File::getStringValue() {
	return file_path;
}

}}}// roxlu::curl::parameter