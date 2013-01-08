#ifndef ROXLU_SHADERGENERATORH 
#define ROXLU_SHADERGENERATORH

#include <string>

/*

	WHILE CODING THIS I REALIZED WE NEED A EFFECT TYPE WHICH DOES THIS
	KINDS OF THINGS. AN EFFECT WILL DEFINE WHAT KIND OF DATA A SCENEITEM
	OR RATHER A VERTEXDATA MUST HAVE!	

*/

using std::string;

namespace roxlu {

class ShaderGenerator {
public:
	enum ShaderFeature {
		 SHADER_FEATURE_NONE				= (0)
		,SHADER_FEATURE_TEXCOORDS			= (1 << 1)
		,SHADER_FEATURE_NORMALS				= (1 << 2)
		,SHADER_FEATURE_DIFFUSE_TEXTURE		= (1 << 3)
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
	
	inline void enableNormals();
	inline void disableNormals();
	inline bool hasNormals();
		
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
	enableFeature(SHADER_FEATURE_TEXCOORDS);
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

inline void ShaderGenerator::enableNormals() {
	enableFeature(SHADER_FEATURE_NORMALS);
}

inline void ShaderGenerator::disableNormals() {
	disableFeature(SHADER_FEATURE_NORMALS);
}

inline bool ShaderGenerator::hasNormals() {
	return hasFeature(SHADER_FEATURE_NORMALS);
}

}; // roxlu

#endif