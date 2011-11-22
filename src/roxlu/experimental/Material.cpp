#include "Material.h"
namespace roxlu {

Material::Material(string name, Shader* shad) 
	:set_materials(MAT_NONE)
	,shader(shad)
	,done(false)
	,name(name)
{
}

Material::~Material() {
}

// @todo maybe it's cleaner a Material does not know about a shader and the 
// shader uniforms like "diffuse_texture" and "normal_texture". and maybe it
// would be better to add this code somewhere in the Effect class.
//
// update 2011.11.22: I really think the materials should be decoupled from 
// the shader. I creatd the r3f import/exporter for which I need a "NULL"
// shader as while importing no shader exist.
void Material::bind() {
	// set diffuse texture

// 2011.11.22 moved to effect.cpp
//	int n = 0;
//	if(set_materials & MAT_DIFFUSE) {
//		Texture* tex = getTexture(MAT_DIFFUSE);
//		shader->setTextureUnit("diffuse_texture", tex->getTextureID(), n, GL_TEXTURE_2D);
//		tex->bind();
//		//printf(">> diffuse: %d\n", tex->getTextureID());
//		n++;
//	}
//	
//	if(set_materials & MAT_NORMAL) {
//		Texture* tex = getTexture(MAT_NORMAL);
//		//printf(">> normal: %d\n", tex->getTextureID());
//		shader->setTextureUnit("normal_texture", tex->getTextureID(), n, GL_TEXTURE_2D);
//		tex->bind();
//		n++;
//	}
	
}

void Material::unbind() {
	map<int, Texture*>::iterator it = materials.begin();
	while(it != materials.end()) {
		it->second->unbind();
		++it;
	}
}


}; // roxlu