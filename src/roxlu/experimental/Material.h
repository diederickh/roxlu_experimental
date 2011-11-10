#ifndef ROXLU_MATERIALH
#define ROXLU_MATERIALH


#include "Texture.h" // texture uses ofImage now.. so we cannot use "OpenGL.h"
#include "Shader.h"
//#include "OpenGL.h"
#include <map>

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
	
	Material();
	~Material();
	
	void bind();
	void unbind();
	
	inline void setShader(Shader* shader);
	
	// generic material loading
	inline Texture* loadMaterial(int type, string file, int imageFormat = GL_RGB);
	inline Texture* getMaterial(int type);
	inline bool hasMaterial(int type);
	inline void setMaterial(int type, Texture* tex);
	
	// diffuse material
	inline Texture* loadDiffuseMaterial(string file, int imageFormat = GL_RGB);
	Texture* getDiffuseMaterial();
	bool hasDiffuseMaterial();
	
private:
	bool done;
	int set_materials;
	Shader* shader;	
	map<int, Texture*> materials;
};


inline void Material::setShader(Shader* shad) {
	shader = shad;
}

inline Texture* Material::loadDiffuseMaterial(string file, int imageFormat) {
	shader->addUniform("diffuse_texture");
	return loadMaterial(MAT_DIFFUSE, file, imageFormat);
}

inline Texture* Material::getMaterial(int type) {
	map<int, Texture*>::iterator it = materials.find(type);
	if(it == materials.end()) {
		return NULL;
	}
	return it->second;
}

inline bool Material::hasMaterial(int type) {
	map<int, Texture*>::iterator it = materials.find(type);
	return (it != materials.end());
}


inline void Material::setMaterial(int type, Texture* tex) {
	materials.insert(std::pair<int, Texture*>(type, tex));
}

inline Texture* Material::loadMaterial(int type, string file, GLint imageFormat) {
	Texture* tex = new Texture();
	tex->loadImage(file, imageFormat);
	materials.insert(std::pair<int, Texture*>(type, tex));
	set_materials |= type;
	return tex;
}

}; // roxlu
#endif