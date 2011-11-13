#ifndef ROXLU_MATERIALH
#define ROXLU_MATERIALH


#include "Texture.h" // texture uses ofImage now.. so we cannot use "OpenGL.h"
#include "Shader.h"
#include <map>
#include <string>

using std::string;
using std::map;
namespace roxlu {


class Material {
public:
	enum MaterialType {
		 MAT_NONE			= 0
		,MAT_DIFFUSE		= (1 << 0)
		,MAT_SPECULAR		= (1 << 1)
		,MAT_AMBIENT		= (1 << 2)
		,MAT_HEIGHT			= (1 << 3)
		,MAT_NORMAL			= (1 << 4)
		,MAT_SHININESS		= (1 << 5)
		,MAT_OPACITY		= (1 << 6)
		,MAT_DISPLACEMENT	= (1 << 7)
		,MAT_LIGHT			= (1 << 8)
		,MAT_REFLECTION		= (1 << 9)
		,MAT_CUBEMAP		= (1 << 10)
	};
	
	Material(string name);
	~Material();
	
	void bind();
	void unbind();
	
	// generic material loading
	inline Texture* loadTexture(int type, string file, int imageFormat = GL_RGB);
	inline Texture* getTexture(int type);
	inline bool hasTexture(int type);
	inline void setTexture(int type, Texture* tex);
	inline string getTextureFilePath(int type);
	
	// diffuse material
	inline Texture* loadDiffuseTexture(string file, int imageFormat = GL_RGB);
	inline Texture* getDiffuseTexture();
	inline bool hasDiffuseTexture();
	inline string getDiffuseTextureFilePath();
	
	// normal map
	inline Texture* loadNormalTexture(string file, int imageFormat = GL_RGB);
	
	
	// name
	inline void setName(string name);
	inline string getName();
	
private:
	string name;
	bool done;
	int set_materials;
	map<int, Texture*> materials;
};


inline Texture* Material::loadDiffuseTexture(string file, int imageFormat) {
	return loadTexture(MAT_DIFFUSE, file, imageFormat);
}

inline bool Material::hasDiffuseTexture() {
	return hasTexture(MAT_DIFFUSE);
}

inline string Material::getDiffuseTextureFilePath() {
	return getTextureFilePath(MAT_DIFFUSE);
}

inline Texture* Material::getTexture(int type) {
	map<int, Texture*>::iterator it = materials.find(type);
	if(it == materials.end()) {
		return NULL;
	}
	return it->second;
}

inline void Material::setName(string matName) {
	name = matName;
}

inline Texture* Material::getDiffuseTexture() {
	return getTexture(MAT_DIFFUSE);
}

inline string Material::getName() {
	return name;
}

inline bool Material::hasTexture(int type) {
	return set_materials & type;
}

inline void Material::setTexture(int type, Texture* tex) {
	materials.insert(std::pair<int, Texture*>(type, tex));
}

inline Texture* Material::loadTexture(int type, string file, GLint imageFormat) {
	Texture* tex = new Texture();
	tex->loadImage(file, imageFormat);
	materials.insert(std::pair<int, Texture*>(type, tex));
	set_materials |= type;
	return tex;
}

inline string Material::getTextureFilePath(int type) {
	return getTexture(type)->getImageFilePath();
}

}; // roxlu
#endif