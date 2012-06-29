#include "User.h"

namespace roxlu {
namespace openni {

User::User(int id) 
	:id(id)
	,is_tracked(false)
{
}

User::~User() {
	delete[] mask_pixels;
}

void User::setup(int imgWidth, int imgHeight) {
	width = imgWidth;
	height = imgHeight;
	mask_pixels = new unsigned char[width * height];
}

}} // roxlu::openni