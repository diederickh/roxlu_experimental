#include "R3F.h"
#include "Quat.h"
#include "VertexData.h"
#include "SceneItem.h"
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
	buffer.saveToFile(File::toDataPath(fileName));	
}

void R3F::storeVertexData(IOBuffer& buffer, VertexData& vd) {
	// store ID
	buffer.storeUI32((uint32_t)&vd);
	
	// store: vertices.
	buffer.storeUI8(R3F_VERTICES);
	int num = vd.getNumVertices();
	buffer.storeUI32(num);
	for(int i = 0; i < num; ++i) {
		Vec3 v = vd.getVertex(i);
		buffer.storeFloat(v.x);
		buffer.storeFloat(v.y);
		buffer.storeFloat(v.z);
		v.print();
	}
	
	// store: quads
	int num_q = vd.getNumQuads();
	if(num_q > 0) {
		buffer.storeUI8(R3F_FACES);
		buffer.storeUI32(num_q);
		for(int i = 0; i < num_q; ++i) {
			Quad& q = *vd.getQuadPtr(i);
			buffer.storeUI8(R3F_FACE_QUAD);
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

}



}; // roxlu