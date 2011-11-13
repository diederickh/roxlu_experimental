#include "Material.h"
namespace roxlu {

Material::Material(string name) 
	:set_materials(MAT_NONE)
//	,shader(NULL)
	,done(false)
	,name(name)
{
}

Material::~Material() {
}

void Material::bind() {
	// set diffuse texture
	if(set_materials & MAT_DIFFUSE) {
		Texture* tex = getTexture(MAT_DIFFUSE);
		tex->bind();
	}
}

void Material::unbind() {
	map<int, Texture*>::iterator it = materials.begin();
	while(it != materials.end()) {
		it->second->unbind();
		++it;
	}
}


}; // roxlu