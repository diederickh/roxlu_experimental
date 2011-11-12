#ifndef ROXLU_EFFECTH
#define ROXLU_EFFECTH

/*
An effect is used to i.e. texture a scene item, add lighting to a scene item
or to add cube mapping etc..

IMPORTANT: it's the effect object an scene item has which defines what kind
of data the VertexData member of the scene item must have! So:
- when you add lighting, we assume the vertexdata has normals
- when you add textures, we assume the vertexdata has texcoords

*/

#include <string>
#include <vector>

#include "Shader.h"
#include "VertexTypes.h"

using std::vector;
using std::string;

/*
	 VERT_NONE 		= ( 0 )
	,VERT_POS 		= ( 1 << 0 )
	,VERT_TEX 		= ( 1 << 1 )
	,VERT_NORM		= ( 1 << 2 )
	,VERT_COL		= ( 1 << 3 )
	,VERT_BINORM 	= ( 1 << 4 )
	,VERT_TAN		= ( 1 << 5 )
*/

namespace roxlu {

class Light;
class VAO;
class VBO;
class VertexData;


class Effect {
public:
	Effect();
	~Effect();
	
	enum EffectFeature {
		 EFFECT_FEATURE_NONE				= (0)
		,EFFECT_FEATURE_TEXCOORDS			= (1 << 1)
		,EFFECT_FEATURE_NORMALS				= (1 << 2)
		,EFFECT_FEATURE_DIFFUSE_TEXTURE		= (1 << 3)
	};
	
	void createShader(string& vertShader, string& fragShader);
	void setupShader();
	void setupBuffer(VAO& vao, VBO& vbo, VertexData& vd);
	inline Shader& getShader();
	
	inline void enableFeature(EffectFeature feature, VertexAttrib neccesaryVertexAttrib);
	inline void disableFeature(EffectFeature feature, VertexAttrib neccesaryVertexAttrib);
	inline bool hasFeature(EffectFeature feature);
	inline bool hasTextures();
	
	inline void enableTexCoords();
	inline void disableTexCoords();
	inline bool hasTexCoords();
	
	inline void enableNormals();
	inline void disableNormals();
	inline bool hasNormals();
	
	inline void enableDiffuseTexture();
	inline void disableDiffuseTexture();
	inline bool hasDiffuseTexture();
	
	inline void addLight(Light* l);
	inline int getNumberOfLights();
	inline bool hasLights();
	
private:
	uint64_t necessary_vertex_attribs;
	bool shader_created;
	vector<Light*> lights;
	uint64_t features;
	Shader shader;
};

inline void Effect::enableFeature(EffectFeature feature, VertexAttrib necessaryVertexAttrib) {
	features |= feature;
	necessary_vertex_attribs |= necessaryVertexAttrib;
}

inline void Effect::disableFeature(EffectFeature feature, VertexAttrib necessaryVertexAttrib) {
	features &= ~feature;
	necessary_vertex_attribs &= ~necessaryVertexAttrib;
}

inline bool Effect::hasFeature(EffectFeature feature) {
	return (features & feature);
}

inline bool Effect::hasTextures() {
	uint64_t texture_types = EFFECT_FEATURE_DIFFUSE_TEXTURE;
	return (features & texture_types);
}

inline void Effect::enableTexCoords() {
	enableFeature(EFFECT_FEATURE_TEXCOORDS, VERT_TEX );
}

inline void Effect::disableTexCoords() {
	disableFeature(EFFECT_FEATURE_TEXCOORDS, VERT_TEX);
}

inline bool Effect::hasTexCoords() {
	return hasFeature(EFFECT_FEATURE_TEXCOORDS);
}

inline void Effect::enableNormals() {
	enableFeature(EFFECT_FEATURE_NORMALS, VERT_NORM);
}

inline void Effect::disableNormals() {
	disableFeature(EFFECT_FEATURE_NORMALS, VERT_NORM);
}

inline bool Effect::hasNormals() {
	return hasFeature(EFFECT_FEATURE_NORMALS);
}

inline void Effect::enableDiffuseTexture() {
	enableFeature(EFFECT_FEATURE_DIFFUSE_TEXTURE, VERT_TEX);
}

inline void Effect::disableDiffuseTexture() {
	disableFeature(EFFECT_FEATURE_DIFFUSE_TEXTURE, VERT_TEX);
}

inline bool Effect::hasDiffuseTexture() {
	return hasFeature(EFFECT_FEATURE_DIFFUSE_TEXTURE);
}

inline void Effect::addLight(Light* l) {
	lights.push_back(l);
	enableNormals(); 
}

inline int Effect::getNumberOfLights() {
	return lights.size();
}

inline bool Effect::hasLights() {
	return lights.size() > 0;
}

inline Shader& Effect::getShader() {
	return shader;
}



}; // roxlu

#endif