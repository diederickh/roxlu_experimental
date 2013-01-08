#ifndef ROXLU_OBJH
#define ROXLU_OBJH

#include <sstream>
#include <map>
#include <string>
#include <fstream>

//#include "VertexData.h"

#include <roxlu/3d/VertexData.h>

using namespace std;

namespace roxlu {



class OBJ {
public:
	
	struct Object {
		VertexData vd;
		string name;
	};	
	
	bool import(const string& filepath);
	bool extractFace(string info, int& vertexIndex, int& normalIndex, int& texcoordIndex);
	void print();
	bool contains(const string& objectName);
	VertexData get(const string& name);
	
	std::map<string, Object> objects;
	VertexData operator[](const string& name);
};

inline VertexData OBJ::get(const string& name) {
	return (*this)[name];
}

inline VertexData OBJ::operator[](const string& name) {
	map<string,Object>::iterator it = objects.find(name);
	if(it == objects.end()) {
		printf("Cannot find object in OBJ: %s\n", name.c_str());
		VertexData vd;
		return vd;	
	}
	return it->second.vd;
}

inline bool OBJ::contains(const string& objectName) {
	map<string,Object>::iterator it = objects.find(objectName);
	if(it == objects.end()) {
		return false;
	}
	return true;
}

};

#endif