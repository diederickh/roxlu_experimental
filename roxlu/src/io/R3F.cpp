/*
#include "R3F.h"
#include "Quat.h"
#include "VertexData.h"
#include "SceneItem.h"
#include "Material.h"
#include "../IOBuffer.h"
#include "File.h"
*/
#include "../Roxlu.h"
namespace roxlu {

R3F::R3F() {

}

R3F::~R3F() {

}

void R3F::load(string fileName, bool inDataPath) {
	IOBuffer buffer;
	
	if(inDataPath) {
		fileName = File::toDataPath(fileName);
	}
	
	// Load R3F file.
	buffer.loadFromFile(fileName);
	uint8_t command = buffer.consumeUI8();
	if(command != R3F_VERTEX_DATAS) {
		printf("Error, incorrect r3f file format: no vertex datas found\n");		
		return;
	}

	// Consume: vertex datas
	uint32_t num_vertex_datas = buffer.consumeUI32LE();
	for(uint32_t i = 0; i < num_vertex_datas; ++i) {
		string name = buffer.consumeStringWithSizeLE();
		uint32_t num_vertices = buffer.consumeUI32LE();
		VertexData* vd = new VertexData(name);
		vertex_datas.push_back(vd);
		
		// vertices
		for(uint32_t j = 0; j < num_vertices; ++j) {
			Vec3 v(buffer.consumeFloatLE(), buffer.consumeFloatLE(), buffer.consumeFloatLE());
			vd->addVertex(v);
		}
		
		// texcoords
		uint32_t num_texcoords = buffer.consumeUI32LE();
		for(uint32_t j = 0; j < num_texcoords; ++j) {
			Vec2 v(buffer.consumeFloatLE(), buffer.consumeFloatLE());
			vd->addTexCoord(v);
		}
				
		// normals
		uint32_t num_normals = buffer.consumeUI32LE();
		for(uint32_t j = 0; j < num_normals; ++j) {
			Vec3 v(buffer.consumeFloatLE(), buffer.consumeFloatLE(),buffer.consumeFloatLE());
			vd->addNormal(v);
		}
	
		// number of quads
		uint32_t num_quads = buffer.consumeUI32LE();
		for(uint32_t j = 0; j < num_quads; ++j) {
			Quad q(buffer.consumeUI32LE(), buffer.consumeUI32LE(), buffer.consumeUI32LE(), buffer.consumeUI32LE());
			vd->addQuad(q);
		}
		
		// number of triangles
		uint32_t num_tris = buffer.consumeUI32LE();
		for(uint32_t j = 0; j < num_tris; ++j) {
			Triangle t(buffer.consumeUI32LE(), buffer.consumeUI32LE(), buffer.consumeUI32LE());
			vd->addTriangle(t);
		}
	}

	// consume materials
	command = buffer.consumeUI8();
	if(command != R3F_MATERIALS) {
		printf("Error, incorrect r3f format: no materials entry found (%d)\n", command);
		return;
	}
	
	uint32_t num_materials = buffer.consumeUI32LE();
	for(int i = 0; i < num_materials; ++i) {
		string mat_name = buffer.consumeStringWithSizeLE();
		int num_textures = buffer.consumeUI8();

		Material* m = new Material(mat_name);
		materials.push_back(m);
		for(int j = 0; j < num_textures; ++j) {
			int texture_type = buffer.consumeUI8();
			string texture_file = buffer.consumeStringWithSizeLE();
			m->loadTexture(texture_type, texture_file);
		}
	}


	// Consume: scene items
	command = buffer.consumeUI8();
	if(command != R3F_SCENE_ITEMS) {
		printf("Error, incorrect r3f format: no scene items found\n");
		return;
	}
	uint32_t num_scene_items = buffer.consumeUI32LE();
	for(uint32_t i = 0; i < num_scene_items; ++i) {
		string vertex_data_name = buffer.consumeStringWithSizeLE();
		string si_name = buffer.consumeStringWithSizeLE();
		VertexData* vd = getVertexData(vertex_data_name);
		if(vd == NULL) {
			printf("Error, cannot find vertex data object: '%s'\n", vertex_data_name.c_str());
			continue;
		}
		
		SceneItem* si = new SceneItem(si_name);
		scene_items.push_back(si);
				
		Vec3 origin(buffer.consumeFloatLE(), buffer.consumeFloatLE(), buffer.consumeFloatLE());
		Vec3 scale(buffer.consumeFloatLE(), buffer.consumeFloatLE(), buffer.consumeFloatLE());
		Quat orientation(buffer.consumeFloatLE(),buffer.consumeFloatLE(),buffer.consumeFloatLE(),buffer.consumeFloatLE());
		si->setPosition(origin);
		si->setScale(scale);
		si->setOrientation(orientation);
		si->createFromVertexData(vd);
		
		// check if the scene item has a material
		bool has_material = buffer.consumeBool();
		if(has_material) {
			string mat_name = buffer.consumeStringWithSizeLE();
			Material* mat = getMaterial(mat_name);
			if(mat == NULL) {
				printf("Error, cannot find material for scene item: '%s'\n", mat_name.c_str());
			}
			else {
				si->setMaterial(mat);
			}
		}
	}
}

void R3F::save(string fileName, bool inDataPath) {
	IOBuffer buffer;
	
	// store vertex datas.
	vector<VertexData*>::iterator it_data = vertex_datas.begin();
	buffer.storeUI8(R3F_VERTEX_DATAS);
	buffer.storeUI32LE(vertex_datas.size());
	while(it_data != vertex_datas.end()) {
		VertexData& vd = *(*it_data);
		storeVertexData(buffer, vd);
		++it_data;
	}

	// store materials.
	buffer.storeUI8(R3F_MATERIALS);
	buffer.storeUI32LE(materials.size());
	vector<Material*>::iterator mat_it = materials.begin();
	while(mat_it != materials.end()) {
		storeMaterial(buffer, *(*mat_it));
		++mat_it;
	}

	// store Scene Item (instances of vertex datas)
	buffer.storeUI8(R3F_SCENE_ITEMS);
	buffer.storeUI32LE(scene_items.size());
	vector<SceneItem*>::iterator scene_it = scene_items.begin();
	while(scene_it != scene_items.end()) {
		storeSceneItem(buffer, *(*scene_it));
		++scene_it;
	}
	if(inDataPath) {
		fileName = File::toDataPath(fileName);
	}
	buffer.saveToFile(fileName);	
}

void R3F::storeMaterial(IOBuffer& buffer, Material& m) {
	buffer.storeUI16LE(m.getName().size());
	buffer.storeString(m.getName());
	
	// store textures
	int num_textures = m.getNumTextures();
	buffer.storeUI8(num_textures);

	Material::texture_map map = m.getTextures();
	Material::texture_iterator it = map.begin();
	while(it != map.end()) {
		buffer.storeUI8(it->first);
		string file_path = m.getTextureFilePath(it->first);
		buffer.storeUI16LE(file_path.size());
		buffer.storeString(file_path);
		++it;
	}
	
}

void R3F::storeVertexData(IOBuffer& buffer, VertexData& vd) {
	// store name
	buffer.storeUI16LE(vd.getName().size());
	buffer.storeString(vd.getName());
	
	// store: vertices.
	int num = vd.getNumVertices();
	buffer.storeUI32LE(num);
	for(int i = 0; i < num; ++i) {
		Vec3 v = vd.getVertex(i);
		buffer.storeFloatLE(v.x);
		buffer.storeFloatLE(v.y);
		buffer.storeFloatLE(v.z);
	}
	
	// store: texcoords
	int num_t = vd.getNumTexCoords();
	buffer.storeUI32LE(num_t);
	for(int i = 0; i < num_t; ++i) {
		Vec2 v = vd.getTexCoord(i);
		buffer.storeFloat(v.x);
		buffer.storeFloat(v.y);
	}
	
	// store: normals
	int num_n = vd.getNumNormals();
	buffer.storeUI32LE(num_n);
	for(int i = 0; i < num_n; ++i) {
		Vec3 v = vd.getNormal(i);
		buffer.storeFloatLE(v.x);
		buffer.storeFloatLE(v.y);
		buffer.storeFloatLE(v.z); 
	}
	
	// store: quads
	int num_q = vd.getNumQuads(); 
	buffer.storeUI32LE(num_q);
	if(num_q > 0) {
		for(int i = 0; i < num_q; ++i) {
			Quad& q = *vd.getQuadPtr(i);
			buffer.storeUI32LE(q.a);
			buffer.storeUI32LE(q.b);
			buffer.storeUI32LE(q.c);
			buffer.storeUI32LE(q.d);
		}
	}
	
	// store: trianges
	num_t = vd.getNumTriangles();
	buffer.storeUI32LE(num_t);
	for(int i = 0; i < num_t; ++i) {
		Triangle& tri = *vd.getTrianglePtr(i);
		buffer.storeUI32LE(tri.a);
		buffer.storeUI32LE(tri.b);
		buffer.storeUI32LE(tri.c);
	}
}

void R3F::storeSceneItem(IOBuffer& buffer, SceneItem& si) {
	// store "ID" of vertex data
	buffer.storeUI16LE(si.getVertexData()->getName().size());
	buffer.storeString(si.getVertexData()->getName());
	
	// store name
	buffer.storeUI16LE(si.getName().size());
	buffer.storeString(si.getName());
	
	// store origin
	Vec3 pos = si.getPosition();
	buffer.storeFloatLE(pos.x);
	buffer.storeFloatLE(pos.y);
	buffer.storeFloatLE(pos.z);
	
	// store scale
	Vec3 scale = si.getScale();
	buffer.storeFloatLE(scale.x);
	buffer.storeFloatLE(scale.y);
	buffer.storeFloatLE(scale.z);
	
	// store orientation
	Quat q = si.getOrientation();
	buffer.storeFloatLE(q.x);
	buffer.storeFloatLE(q.y);
	buffer.storeFloatLE(q.z);
	buffer.storeFloatLE(q.w);

	// store material flag
	buffer.storeBool(si.hasMaterial());
	if(si.hasMaterial()) {
		buffer.storeUI16LE(si.getMaterial()->getName().size());
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