#include "File.h"
namespace roxlu {
namespace twitter {
namespace parameter {

namespace rtp = roxlu::twitter::parameter;

File::File(const std::string& n, const std::string& f)
	:file_path(f)
{
	name = n;
	type = rtp::Parameter::PARAM_FILE;
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

}}}// roxlu::twitter::parameter