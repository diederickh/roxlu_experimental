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
#include "Timer.h"
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
class Material;

class Effect {
public:
	Effect();
	~Effect();
	
	enum EffectFeature {
		 EFFECT_FEATURE_NONE				= (0)
		,EFFECT_FEATURE_TEXCOORDS			= (1 << 1)
		,EFFECT_FEATURE_NORMALS				= (1 << 2)
		,EFFECT_FEATURE_TANGENTS			= (1 << 3)
		,EFFECT_FEATURE_DIFFUSE_TEXTURE		= (1 << 4)
		,EFFECT_FEATURE_NORMAL_TEXTURE		= (1 << 5)
	};
	
	
	void createShaders(string& vertShader, string& fragShader);
	void createVertexShader(string& vertShader);
	void createFragmentShader(string& fragShader);
	void setupShader();  // should be private
	void setupShaderAttributesAndUniforms(); // should be private
 	void setupBuffer(VAO& vao, VBO& vbo, VertexData& vd);
	inline Shader& getShader();
	inline Shader* getShaderPtr();

	// auto reload and compile shaders	
	void saveShaders(string name, bool inDataPath = true);
	void autoReloadShaders(string name, bool inDataPath = true);
	void reloadShaders(string name, bool inDataPath = true);

	inline void enableFeature(EffectFeature feature, int neccesaryVertexAttrib);
	inline void disableFeature(EffectFeature feature, int neccesaryVertexAttrib);
	inline bool hasFeature(EffectFeature feature);
	inline bool hasTextures();
	
	inline void enableTexCoords();
	inline void disableTexCoords();
	inline bool hasTexCoords();
	

	// lighting etc..
	inline void enableNormals();
	inline void disableNormals();
	inline bool hasNormals();
	
	// normapmap
	inline void enableTangents();
	inline void disableTangents();
	inline bool hasTangents();
	
	
	// diffuse texture
	inline void enableDiffuseTexture();
	inline void disableDiffuseTexture();
	inline bool hasDiffuseTexture();
	
	// normal texture
	inline void enableNormalTexture();
	inline void disableNormalTexture();
	inline bool hasNormalTexture();

	
	inline void addLight(Light& l);
	inline void addLight(Light* l);
	inline int getNumberOfLights();
	inline bool hasLights();
	void updateShaders();
	void updateLights();
	void bindMaterial(Material& m);
	inline void disable();
	
	// used by the auto reload shader feature.
	void onTimer();
	
private:
	uint64_t necessary_vertex_attribs;
	bool shader_created;
	bool reload_shader_datapath;
	vector<Light*> lights;
	uint64_t features;
	Shader shader;
//	int texunit;
	
	bool reload_shader_enabled;
	string reload_shader_name;
	time_t reload_shader_last_modified_frag;
	time_t reload_shader_last_modified_vert;
	roxlu::Timer<Effect> reload_timer;

};

// general
inline void Effect::enableFeature(EffectFeature feature, int necessaryVertexAttrib) {
	features |= feature;
	necessary_vertex_attribs |= necessaryVertexAttrib;
}

inline void Effect::disableFeature(EffectFeature feature, int necessaryVertexAttrib) {
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

inline void Effect::disable() {
	shader.disable();
}

// texcoords
inline void Effect::enableTexCoords() {
	enableFeature(EFFECT_FEATURE_TEXCOORDS, VERT_TEX );
}

inline void Effect::disableTexCoords() {
	disableFeature(EFFECT_FEATURE_TEXCOORDS, VERT_TEX);
}

inline bool Effect::hasTexCoords() {
	return hasFeature(EFFECT_FEATURE_TEXCOORDS);
}

// normal
inline void Effect::enableNormals() {
	enableFeature(EFFECT_FEATURE_NORMALS, VERT_NORM);
}

inline void Effect::disableNormals() {
	disableFeature(EFFECT_FEATURE_NORMALS, VERT_NORM);
}

inline bool Effect::hasNormals() {
	return hasFeature(EFFECT_FEATURE_NORMALS);
}


// texture: diffuse
inline void Effect::enableDiffuseTexture() {
	enableFeature(EFFECT_FEATURE_DIFFUSE_TEXTURE, VERT_TEX);
}

inline void Effect::disableDiffuseTexture() {
	disableFeature(EFFECT_FEATURE_DIFFUSE_TEXTURE, VERT_TEX);
}

inline bool Effect::hasDiffuseTexture() {
	return hasFeature(EFFECT_FEATURE_DIFFUSE_TEXTURE);
}

// texture: normal (implicitly tells us we need tangents)
inline void Effect::enableNormalTexture() {
	enableFeature(EFFECT_FEATURE_NORMAL_TEXTURE, VERT_TAN);
}

inline void Effect::disableNormalTexture() {
	disableFeature(EFFECT_FEATURE_NORMAL_TEXTURE, VERT_TAN);
}

inline bool Effect::hasNormalTexture() {
	return hasFeature(EFFECT_FEATURE_NORMAL_TEXTURE);
}

// lights
inline void Effect::addLight(Light& l) {
	addLight(&l);
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
inline Shader* Effect::getShaderPtr() {
	return &shader;
}


}; // roxlu

#endif