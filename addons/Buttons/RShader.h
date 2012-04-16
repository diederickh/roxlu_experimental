#ifndef ROXLU_RSHADERH
#define ROXLU_RSHADERH

#include <string>

class RShader {
public:
	RShader();
	~RShader();
	bool load(const std::string& vs, const std::string fs);
	
};

#endif