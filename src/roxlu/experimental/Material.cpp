#include "Material.h"
namespace roxlu {

Material::Material() 
	:set_materials(MAT_NONE)
	,shader(NULL)
	,done(false)
{
}

Material::~Material() {
}

void Material::bind() {
	shader->enable();
	Texture* tex = getMaterial(MAT_DIFFUSE);
	if(tex != NULL) {
		shader->setTextureUnit("diffuse_texture", tex->getTextureID(), 0, GL_TEXTURE_2D);
	}
	/*
	map<int, Texture*>::iterator it = materials.begin();
	while(it != materials.end()) {
		it->second->bind();
		++it;
	}
	*/
}

void Material::unbind() {
	map<int, Texture*>::iterator it = materials.begin();
	while(it != materials.end()) {
		it->second->unbind();
		++it;
	}
}


}; // roxlu