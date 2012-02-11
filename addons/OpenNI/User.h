#ifndef ROXLU_OPENNI_USERH
#define ROXLU_OPENNI_USERH

namespace roxlu {
namespace openni {

class User {

public:
	
	User(int id);
	~User();
	void setup(int imageWidth, int imageHeight);
	int getID();	
	
	bool is_tracked;
	unsigned char* mask_pixels;
	int id;
	int width;
	int height;
};


inline int User::getID() {
	return id;
}

}} // roxlu::openni
#endif