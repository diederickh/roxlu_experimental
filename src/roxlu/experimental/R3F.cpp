#include "R3F.h"
#include "Quat.h"
#include "VertexData.h"
#include "SceneItem.h"
#include "Material.h"
#include "../IOBuffer.h"
#include "File.h"

namespace roxlu {

R3F::R3F() {

}

R3F::~R3F() {

}

void R3F::load(string fileName) {
	IOBuffer buffer;
	buffer.loadFromFile(File::toDataPath(fileName));
	// @todo implement
}

void R3F::save(string fileName) {
	IOBuffer buffer;
	
	// store vertex datas.
	vector<VertexData*>::iterator it_data = vertex_datas.begin();
	buffer.storeUI8(R3F_VERTEX_DATAS);
	buffer.storeUI32(vertex_datas.size());
	while(it_data != vertex_datas.end()) {
		VertexData& vd = *(*it_data);
		storeVertexData(buffer, vd);
		++it_data;
	}

	// store Scene Item (instances of vertex datas)
	buffer.storeUI8(R3F_SCENE_ITEMS);
	buffer.storeUI32(scene_items.size());
	vector<SceneItem*>::iterator scene_it = scene_items.begin();
	while(scene_it != scene_items.end()) {
		storeSceneItem(buffer, *(*scene_it));
		++scene_it;
	}
	
	// store materials.
	buffer.storeUI8(R3F_MATERIALS);
	buffer.storeUI32(materials.size());
	vector<Material*>::iterator mat_it = materials.begin();
	while(mat_it != materials.end()) {
		storeMaterial(buffer, *(*mat_it));
		++mat_it;
	}
	
	buffer.saveToFile(File::toDataPath(fileName));	
}

void R3F::storeMaterial(IOBuffer& buffer, Material& m) {
	buffer.storeUI16(m.getName().size());
	buffer.storeString(m.getName());
	
	// store textures
	int num_textures = m.getNumTextures();
	buffer.storeUI8(num_textures);
	Material::texture_map map = m.getTextures();
	Material::texture_iterator it = map.begin();
	while(it != map.end()) {
		buffer.storeUI8(it->first);
		string file_path = m.getTextureFilePath(it->first);
		buffer.storeUI16(file_path.size());
		buffer.storeString(file_path);
		++it;
	}
	
}

void R3F::storeVertexData(IOBuffer& buffer, VertexData& vd) {
	// store ID
	buffer.storeUI32((uint32_t)&vd);
	
	// store: vertices.
	int num = vd.getNumVertices();
	buffer.storeUI32(num);
	for(int i = 0; i < num; ++i) {
		Vec3 v = vd.getVertex(i);
		buffer.storeFloat(v.x);
		buffer.storeFloat(v.y);
		buffer.storeFloat(v.z);
	}
	
	// store: texcoords
	int num_t = vd.getNumTexCoords();
	buffer.storeUI32(num_t);
	for(int i = 0; i < num_t; ++i) {
		Vec2 v = vd.getTexCoord(i);
		buffer.storeFloat(v.x);
		buffer.storeFloat(v.y);
	}
	
	// store: normals
	int num_n = vd.getNumNormals();
	buffer.storeUI32(num_n);
	for(int i = 0; i < num_n; ++i) {
		Vec3 v = vd.getNormal(i);
		buffer.storeFloat(v.x);
		buffer.storeFloat(v.y);
		buffer.storeFloat(v.z); 
	}
	
	// store: quads // @todo add triangles
	int num_q = vd.getNumQuads(); 
	buffer.storeUI32(num_q);
	if(num_q > 0) {
		for(int i = 0; i < num_q; ++i) {
			Quad& q = *vd.getQuadPtr(i);
			buffer.storeUI32(q.a);
			buffer.storeUI32(q.b);
			buffer.storeUI32(q.c);
			buffer.storeUI32(q.d);
		}
	}
	
}

void R3F::storeSceneItem(IOBuffer& buffer, SceneItem& si) {
	// store "ID" of vertex data
	buffer.storeUI32((uint32_t)si.getVertexData());
	
	// store name
	buffer.storeUI16(si.getName().size());
	buffer.storeString(si.getName());
	
	// store origin
	Vec3 pos = si.getPosition();
	buffer.storeFloat(pos.x);
	buffer.storeFloat(pos.y);
	buffer.storeFloat(pos.z);
	
	// store scale
	Vec3 scale = si.getScale();
	buffer.storeFloat(scale.x);
	buffer.storeFloat(scale.y);
	buffer.storeFloat(scale.z);
	
	// store orientation
	Quat q = si.getOrientation();
	buffer.storeFloat(q.v.x);
	buffer.storeFloat(q.v.y);
	buffer.storeFloat(q.v.z);
	buffer.storeFloat(q.w);

	// store material flag
	buffer.storeBool(si.hasMaterial());
	if(si.hasMaterial()) {
		buffer.storeUI16(si.getMaterial()->getName().size());
		buffer.storeString(si.getMaterial()->getName());
	}
	
}



}; // roxlu