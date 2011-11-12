#ifndef ROXLU_SHADERGENERATORH 
#define ROXLU_SHADERGENERATORH

#include <string>

using std::string;

namespace roxlu {

class ShaderGenerator {
public:
	enum ShaderFeature {
		 SHADER_FEATURE_NONE				= (0)
		,SHADER_FEATURE_TEXCOORD			= (1 << 1)
		,SHADER_FEATURE_DIFFUSE_TEXTURE		= (1 << 2)
	};
	
	ShaderGenerator();
	~ShaderGenerator();
	void createShader(string& vertShader, string& fragShader);
	inline void enableDiffuseTexture();
	inline void disableDiffuseTexture();
	inline bool hasDiffuseTexture();
	
	inline void enableFeature(ShaderFeature feature);
	inline void disableFeature(ShaderFeature feature);
	inline bool hasFeature(ShaderFeature feature);
	inline bool hasTextures();
	
	inline void setNumberOfLights(int num);
	inline int getNumberOfLights();
	inline bool hasLights();
		
private:
	int number_of_lights;
	uint64_t features;
};

inline void ShaderGenerator::enableFeature(ShaderFeature feature) {
	features |= feature;
}

inline void ShaderGenerator::disableFeature(ShaderFeature feature) {
	features &= ~feature;
}


inline bool ShaderGenerator::hasTextures() {
	uint64_t texture_types = SHADER_FEATURE_DIFFUSE_TEXTURE;
	return (features & texture_types);
}

inline bool ShaderGenerator::hasFeature(ShaderFeature feature) {
	return (features & feature);
}

inline void ShaderGenerator::enableDiffuseTexture() {
	enableFeature(SHADER_FEATURE_TEXCOORD);
	enableFeature(SHADER_FEATURE_DIFFUSE_TEXTURE);
}

inline void ShaderGenerator::disableDiffuseTexture() {
	disableFeature(SHADER_FEATURE_DIFFUSE_TEXTURE);
}

inline bool ShaderGenerator::hasDiffuseTexture() {
	return hasFeature(SHADER_FEATURE_DIFFUSE_TEXTURE);
}

inline void ShaderGenerator::setNumberOfLights(int num) {
	number_of_lights = num;
}

inline int ShaderGenerator::getNumberOfLights() {
	return number_of_lights;
}

inline bool ShaderGenerator::hasLights() {
	return number_of_lights > 0;
}

}; // roxlu

#endif