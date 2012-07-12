//#include "Ply.h"

#include <roxlu/io/Ply.h>

namespace roxlu {

Ply::Ply() {
}


bool Ply::save(string path, VertexData& vd) {
/*
	vector<ofVec3f> normals;
	vector<ofVec3f> vertices;
	vector<ofVec2f> texcoords;
	vector<Color4f> colors;
	vector<int> indices;
	vector<Triangle> triangles;
	vector<ofVec3f> tangents;
	vector<ofVec3f> bitangents;
*/
	if(vd.getNumVertices() <= 0) {
		cout << "No vertices found" << endl;
		return false;
	}
	stringstream ss;
	
	// HEADER
	// -------------------------------------------
	char cr = '\n';
	ss << "ply" << cr;
	ss << "format ascii 1.0" << cr;
	ss << "comment author: roxlu" << cr;
	ss << "comment object: auto generator with openFrameworks" << cr;
	ss << "element vertex " << vd.getNumVertices() << cr;
	ss << "property float32 x" << cr;
	ss << "property float32 y" << cr;
	ss << "property float32 z" << cr;
	
	// add colors.
	if(vd.getNumColors() > 0) {
		ss << "property uchar red" << cr;
		ss << "property uchar green" << cr;
		ss << "property uchar blue" << cr;
	}

//	ss << "property float32 red" << cr;
//	ss << "property float32 green" << cr;
//	ss << "property float32 blue" << cr;
	if(vd.getNumTriangles() > 0) {
		ss << "element face " << vd.getNumTriangles() << cr;
	}
	else if(vd.getNumQuads() > 0) {
		ss << "element face " << vd.getNumQuads() << cr;
	}
	ss << "property list uchar int vertex_indices " << cr;
	ss << "end_header" << cr;
	
	
	// VERTICES AND COLORS
	// -------------------------------------------
	if(vd.getNumColors() == vd.getNumVertices()) {
		ss << setprecision(5) << fixed;
		for(int i = 0; i < vd.getNumVertices(); ++i) {
			ss	<< vd.vertices[i].x << " " 
				<< vd.vertices[i].y << " " 
				<< vd.vertices[i].z	<< " " 
				<< (int)(vd.colors[i].r * 255) << " "
				<< (int)(vd.colors[i].g * 255) << " "
				<< (int)(vd.colors[i].b * 255) << cr;
		}
	}
	else if(vd.getNumColors() == 0 && vd.getNumVertices() > 0) {
		ss << setprecision(5) << fixed;
		for(int i = 0; i < vd.getNumVertices(); ++i) {
			ss	<< vd.vertices[i].x << " " 
				<< vd.vertices[i].y << " " 
				<< vd.vertices[i].z	<< cr;
		}
	}

	// FACES (TRIANGLES)
	// -------------------------------------------
	if(vd.getNumTriangles() > 0) {
		for (int i = 0; i < vd.getNumTriangles(); ++i) {
			ss << "3 " << vd.triangles[i].va << " " << vd.triangles[i].vb << " " << vd.triangles[i].vc << cr;
		}
	}
	else if(vd.getNumQuads() > 0) {
		// FACES (QUADS)
		// -------------------------------------------
		for (int i = 0; i < vd.getNumQuads(); ++i) {
			ss << "4 " << vd.quads[i].a << " " << vd.quads[i].b << " " << vd.quads[i].c << " " << vd.quads[i].d << cr;
		}
	}

	
	ofstream ofs(path.c_str(), std::ios::trunc | std::ios::out);
	if(!ofs.is_open()) {
		cout << "error: cannot open file '" << path << "'" << endl;
		return false;
	}
	ofs << ss.str();
	ofs.flush();
	ofs.close();
	
	cout << vd.getNumColors() << "===" << vd.getNumVertices() << endl;
	//cout << ss.str() << endl;
	return true;
}

}; // roxlu
