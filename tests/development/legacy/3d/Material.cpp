#include "Material.h"
namespace roxlu {

Material::Material(string name) 
	:set_materials(MAT_NONE)
	,done(false)
	,name(name)
{
}

Material::~Material() {
}

// binding should be done somewhere else!

void Material::unbind() {
	map<int, Texture*>::iterator it = materials.begin();
	while(it != materials.end()) {
		it->second->unbind();
		++it;
	}
}


}; // roxlu