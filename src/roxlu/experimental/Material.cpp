#include "Material.h"
namespace roxlu {

Material::Material(string name) 
	:set_materials(MAT_NONE)
	,shader(NULL)
	,done(false)
	,name(name)
{
}

Material::~Material() {
}

void Material::bind() {
	shader->enable();
	
	// set diffuse texture
	if(set_materials & MAT_DIFFUSE) {
		Texture* tex = getMaterial(MAT_DIFFUSE);
		shader->setTextureUnit("diffuse_texture", tex->getTextureID(), 0, GL_TEXTURE_2D);
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