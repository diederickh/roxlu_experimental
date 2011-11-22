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
	
	// Load R3F file.
	buffer.loadFromFile(fileName);
	uint8_t command = buffer.consumeUI8();
	if(command != R3F_VERTEX_DATAS) {
		printf("Error, incorrect r3f file format: no vertex datas found\n");		
		return;
	}

	// Consume: vertex datas
	uint32_t num_vertex_datas = buffer.consumeUI32();
	for(uint32_t i = 0; i < num_vertex_datas; ++i) {
		string name = buffer.consumeStringWithSize();
		uint32_t num_vertices = buffer.consumeUI32();
		VertexData* vd = new VertexData(name);
		vertex_datas.push_back(vd);
		printf("Num vertices: %d\n", num_vertices);
		
		// vertices
		for(uint32_t j = 0; j < num_vertices; ++j) {
			Vec3 v(buffer.consumeFloat(), buffer.consumeFloat(), buffer.consumeFloat());
			vd->addVertex(v);
		}
		
		// texcoords
		uint32_t num_texcoords = buffer.consumeUI32();
		printf("Num texcoords: %d\n", num_texcoords);
		for(uint32_t j = 0; j < num_texcoords; ++j) {
			Vec2 v(buffer.consumeFloat(), buffer.consumeFloat());
			vd->addTexCoord(v);
		}
				
		// normals
		uint32_t num_normals = buffer.consumeUI32();
		printf("num normals: %d\n", num_normals);
		for(uint32_t j = 0; j < num_normals; ++j) {
			Vec3 v(buffer.consumeFloat(), buffer.consumeFloat(),buffer.consumeFloat());
			vd->addNormal(v);
		}
	
		// number of quads
		uint32_t num_quads = buffer.consumeUI32();
		for(uint32_t j = 0; j < num_quads; ++j) {
			Quad q(buffer.consumeUI32(), buffer.consumeUI32(), buffer.consumeUI32(), buffer.consumeUI32());
			vd->addQuad(q);
		}
	}

	// consume materials
	command = buffer.consumeUI8();
	if(command != R3F_MATERIALS) {
		printf("Error, incorrect r3f format: no materials entry found (%d)\n", command);
		return;
	}
	
	uint32_t num_materials = buffer.consumeUI32();
	for(int i = 0; i < num_materials; ++i) {
		string mat_name = buffer.consumeStringWithSize();
		printf("Material name: %s\n", mat_name.c_str());
		int num_textures = buffer.consumeUI8();

		Material* m = new Material(mat_name);
		materials.push_back(m);
		
		printf("Material with textuers: %d\n", num_textures);
		for(int j = 0; j < num_textures; ++j) {
			int texture_type = buffer.consumeUI8();
			printf("texture type: %d\n", texture_type);
			string texture_file = buffer.consumeStringWithSize();
			m->loadTexture(texture_type, texture_file);
			printf("texture file: %s\n", texture_file.c_str());
		}
	}


	// Consume: scene items
	command = buffer.consumeUI8();
	if(command != R3F_SCENE_ITEMS) {
		printf("Error, incorrect r3f format: no scene items found\n");
		return;
	}
	uint32_t num_scene_items = buffer.consumeUI32();
	for(uint32_t i = 0; i < num_scene_items; ++i) {
		string vertex_data_name = buffer.consumeStringWithSize();
		string si_name = buffer.consumeStringWithSize();
		VertexData* vd = getVertexData(vertex_data_name);
		if(vd == NULL) {
			printf("Error, cannot find vertex data object: '%s'\n", vertex_data_name.c_str());
			continue;
		}
		
		SceneItem* si = new SceneItem(si_name);
		scene_items.push_back(si);
				
		Vec3 origin(buffer.consumeFloat(), buffer.consumeFloat(), buffer.consumeFloat());
		Vec3 scale(buffer.consumeFloat(), buffer.consumeFloat(), buffer.consumeFloat());
		Quat orientation(buffer.consumeFloat(),buffer.consumeFloat(),buffer.consumeFloat(),buffer.consumeFloat());
		si->setPosition(origin);
		si->setScale(scale);
		si->setOrientation(orientation);
		si->createFromVertexData(vd);
		
		// check if the scene item has a material
		bool has_material = buffer.consumeBool();
		if(has_material) {
			string mat_name = buffer.consumeStringWithSize();
			printf("mat: %s\n", mat_name.c_str());
			Material* mat = getMaterial(mat_name);
			if(mat == NULL) {
				printf("Error, cannot find material for scene item: '%s'\n", mat_name.c_str());
			}
			else {
				si->setMaterial(mat);
			}
			
			
			
		}
	}
	
	printf("Number of si: %d\n", num_scene_items);
	
		
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

	// store materials.
	buffer.storeUI8(R3F_MATERIALS);
	buffer.storeUI32(materials.size());
	vector<Material*>::iterator mat_it = materials.begin();
	while(mat_it != materials.end()) {
		storeMaterial(buffer, *(*mat_it));
		++mat_it;
	}

	// store Scene Item (instances of vertex datas)
	buffer.storeUI8(R3F_SCENE_ITEMS);
	buffer.storeUI32(scene_items.size());
	vector<SceneItem*>::iterator scene_it = scene_items.begin();
		while(scene_it != scene_items.end()) {
		storeSceneItem(buffer, *(*scene_it));
		++scene_it;
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
	// store name
	buffer.storeUI16(vd.getName().size());
	buffer.storeString(vd.getName());
	
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
	buffer.storeUI16(si.getVertexData()->getName().size());
	buffer.storeString(si.getVertexData()->getName());
	
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

inline VertexData* R3F::getVertexData(string name){
	vector<VertexData*>::iterator it = std::find_if(vertex_datas.begin(), vertex_datas.end(), CompareVertexDataByName(name));
	if(it == vertex_datas.end()) {
		return NULL;
	}
	return *it;
}

SceneItem* R3F::getSceneItem(string name) {
	vector<SceneItem*>::iterator it = std::find_if(scene_items.begin(), scene_items.end(), CompareSceneItemByName(name));
	if(it == scene_items.end()) {
		return NULL;
	}
	return *it;
}

Material* R3F::getMaterial(string name) {
	vector<Material*>::iterator it = std::find_if(materials.begin(), materials.end(), CompareMaterialByName(name));
	if(it == materials.end()) {
		return NULL;
	}	
	return *it;
}

}; // roxlu