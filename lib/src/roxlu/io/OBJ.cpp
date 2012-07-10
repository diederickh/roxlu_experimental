//#include "OBJ.h"

#include <roxlu/io/OBJ.h>

namespace roxlu {

bool OBJ::import(const string& filepath) {
	ifstream ifs;
	ifs.open(filepath.c_str());
	if(!ifs.is_open()) {
		printf("Cannot open: %s\n", filepath.c_str());
		return false;
	}	
	
	Triangle tri;
	Vec3 v;
	Vec2 t;
	OBJ::Object object;
	string line;
	bool object_added = false;
	bool found_object = false;
	string vertex_group = "none";
	size_t tri_index = 0;
	
	while(getline(ifs, line)) {
		if(line.at(0) == '#') {
			continue;
		}
		
		int space = line.find(" ");
		if(space == string::npos) {
			continue;
		}
		
		string cmd = line.substr(0, space);
		
		// object
		if(cmd == "o") {
			if(found_object) {
				object_added = true;
				objects[object.name] = object;
			}
			found_object = true;
			object.name = line.substr(space+1, line.size()-1);
		}
		// vertex
		else if(found_object && cmd == "v") {
			stringstream ss;
			ss << line.substr(space+1, line.size()-1);
			ss >> v.x >> v.y >> v.z;
			object.vd.addVertex(v);
		}
		// normal
		else if(found_object && cmd == "vn") {
			stringstream ss;
			ss << line.substr(space+1, line.size()-1);
			ss >> v.x >> v.y >> v.z;
			object.vd.addNormal(v);
		}
		// texcoord
		else if(found_object && cmd == "vt") {
			stringstream ss;
			ss << line.substr(space+1, line.size()-1);
			ss >> t.x >> t.y;
			object.vd.addTexCoord(t);
		}
		// vertex group
		else if(found_object && cmd == "g") {
			stringstream ss;
			ss << line.substr(space+1, line.size()-1);
			vertex_group = ss.str();
		}
		// face
		else if(found_object && cmd == "f") {
			stringstream ss;
			ss << line.substr(space+1, line.size()-1);
			string face;
			
			ss >> face;
			extractFace(face, tri.va, tri.na, tri.tc_a);
		
			
			ss >> face;			
			extractFace(face, tri.vb, tri.nb, tri.tc_b);
			
			ss >> face;			
			extractFace(face, tri.vc, tri.nc, tri.tc_c);
			
			object.vd.addTriangle(tri);
			
			object.vd.addToVertexGroup(vertex_group, tri_index);
		}

	}
	if(!object_added) {
		objects[object.name] = object;
	}
	
	return true;
	
	// Calculate binormal + tangent;
	/*
	map<string, Object>::iterator it = objects.begin();
	while(it != objects.end()) {
		Object& obj = it->second;
		for(int i = 0; i < obj.vd.triangles.size(); ++i) {
			Triangle& tri = obj.vd.triangles[i];
			
			if(obj.vd.normals.size() <= 0) {
				continue;
			}
			
			Vec3 a_b = (obj.vd.vertices[tri.vb] - obj.vd.vertices[tri.va]).normalize();
			Vec3 b_c = (obj.vd.vertices[tri.vc] - obj.vd.vertices[tri.vb]).normalize();
			Vec3 c_a = (obj.vd.vertices[tri.va] - obj.vd.vertices[tri.vc]).normalize();
			
			Vec3 ba = cross(a_b,obj.vd.normals[tri.na]).normalize();
			Vec3 bb = cross(b_c,obj.vd.normals[tri.nb]).normalize();
			Vec3 bc = cross(c_a,obj.vd.normals[tri.nc]).normalize();
			
			tri.ta = obj.vd.addTangent(a_b);
			tri.tb = obj.vd.addTangent(b_c);
			tri.tc = obj.vd.addTangent(c_a);
			
			tri.ba = obj.vd.addBinormal(ba);
			tri.bb = obj.vd.addBinormal(bb);
			tri.bc = obj.vd.addBinormal(bc);
		}
		++it;
	}
	*/
}

bool OBJ::extractFace(string info, int& vertexIndex, int& normalIndex, int& texcoordIndex) {
	stringstream fss;
	fss << info;
	string fv;
	
	// get vertex index
	if(getline(fss, fv, '/')) {
		if(fv.size()) {
			stringstream ofss;
			ofss << fv;
			ofss >> vertexIndex;
			--vertexIndex;
		}
	}
	else {
		return false;
	}
	
	// get texcoord index
	if(getline(fss, fv, '/')) {
		if(fv.size()) {
			stringstream ofss;
			ofss << fv;
			ofss >> texcoordIndex;
			--texcoordIndex;
		}
	}
	
	// get normal index
	if(getline(fss, fv, '/')) {
		if(fv.size()) {
			stringstream ofss;
			ofss << fv;
			ofss >> normalIndex;
			--normalIndex;
		}
	}
	
	return true;
}

void OBJ::print() {
	std::map<string, Object>::iterator it = objects.begin();
	while(it != objects.end()) {
		printf("- '%s'\n", it->first.c_str());
		++it;
	}
}

};