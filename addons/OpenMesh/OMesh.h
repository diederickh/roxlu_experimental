#ifndef ROXLU_OPEN_MESHH
#define ROXLU_OPEN_MESHH

/**
 * Simple wrapper around OpenMesh
 *
 * Usage:
 * -------
 * - Make sure to add "-DOM_STATIC_BUILD" to the compile flags.
 *
 *
 * Example getting vertices:
 * ------------------------
 * 
 *	m.load(ofToDataPath("shape.obj", true)(;
 *		
 *	MeshType::VertexIter vit = m.mesh.vertices_begin();
 *	while(vit != m.mesh.vertices_end()) {
 *		MeshType::Point p = m.mesh.point(vit.handle());
 *		v.veins.addSource(p[0], p[1], p[2]);
 *		++vit;
 *	}
 * 
 */

//#include "ofMain.h"
#include "OpenGL.h"
#include <string>
#include <vector>

using std::string;

#undef check
#include <OpenMesh/Core/IO/writer/OBJWriter.hh>
#include <OpenMesh/Core/IO/MeshIO.hh> // include before kernel type!
#include <OpenMesh/Core/Mesh/TriMesh_ArrayKernelT.hh>
#include <OpenMesh/Tools/Utils/Timer.hh>


struct MyTraits : public OpenMesh::DefaultTraits {
	
	VertexAttributes( 	
		OpenMesh::Attributes::Normal 
		| OpenMesh::Attributes::Color 
	);

 	FaceAttributes( 
		OpenMesh::Attributes::Normal 
	);
};

typedef OpenMesh::TriMesh_ArrayKernelT<MyTraits>  MeshType;


class OMesh {
public:
	OMesh();
	~OMesh();
	
	MeshType::VertexHandle addVertex(const float& x, const float& y, const float& z);
	MeshType::FaceHandle addFace(const MeshType::VertexHandle& a, const MeshType::VertexHandle& b, const MeshType::VertexHandle& c);
	MeshType::FaceHandle addFace(const MeshType::VertexHandle& a, const MeshType::VertexHandle& b, const MeshType::VertexHandle& c, const MeshType::VertexHandle& d);
	MeshType::FaceHandle addFace(const MeshType::VertexHandle* verts, const unsigned int& a, const unsigned int& b, const unsigned int& c);
	MeshType::FaceHandle addFace(const MeshType::VertexHandle* verts, const unsigned int& a, const unsigned int& b, const unsigned int& c, const unsigned int& d);
	MeshType::FaceHandle getClosestFace(const MeshType::Point& pos);
	MeshType::Point getFaceCenter(const MeshType::FaceHandle& faceHandle);
	MeshType::Point getFaceCenter(const MeshType::HalfedgeHandle& faceHalfEdge);
	MeshType::Point getFaceCenter(const MeshType::Face& face);
	MeshType::Normal getFaceNormal(const MeshType::FaceHandle& faceHandle);
	MeshType::Normal getFaceNormal(const MeshType::Face& face);
	
	void debugDraw();
	void debugDrawFace(const MeshType::FaceHandle& faceHandle);
	bool save(const string& filepath);
	bool load(const string& filepath);
	
	
	// utils
	MeshType::Point lineIntersection(
		const MeshType::Point& originA
		,const MeshType::Point& directionA
		,const MeshType::Point& originB
		,const MeshType::Point& directionB
	);
	
	
	MeshType mesh;
};


inline MeshType::VertexHandle OMesh::addVertex(const float& x, const float& y, const float& z) {
	return mesh.add_vertex(MeshType::Point(x,y,z));
}

inline MeshType::FaceHandle OMesh::addFace(const MeshType::VertexHandle& a, const MeshType::VertexHandle& b, const MeshType::VertexHandle& c) {
	std::vector<MeshType::VertexHandle> handles;
	handles.push_back(a);
	handles.push_back(b);
	handles.push_back(c);
	return mesh.add_face(handles);
}


inline MeshType::FaceHandle OMesh::addFace(const MeshType::VertexHandle& a, const MeshType::VertexHandle& b, const MeshType::VertexHandle& c, const MeshType::VertexHandle& d) {
	std::vector<MeshType::VertexHandle>  handles;
	handles.push_back(a);
	handles.push_back(b);
	handles.push_back(c);
	handles.push_back(d);
	return mesh.add_face(handles);
}

inline MeshType::FaceHandle OMesh::addFace(const MeshType::VertexHandle* verts, const unsigned int& a, const unsigned int& b, const unsigned int& c) {
	return addFace(verts[a], verts[b], verts[c]);
}

inline MeshType::FaceHandle OMesh::addFace(const MeshType::VertexHandle* verts, const unsigned int& a, const unsigned int& b, const unsigned int& c, const unsigned int& d) {
	return addFace(verts[a], verts[b], verts[c], verts[d]);
}


struct OMesh_FaceDrawer {
	OMesh_FaceDrawer(OMesh& mesh)
		:mesh(mesh) 
	{
	}
	
	void operator()(MeshType::Face& f) {
		MeshType::FaceVertexIter face_vert =  mesh.mesh.fv_iter(mesh.mesh.handle(f));
		while(face_vert) {
			MeshType::Point p = mesh.mesh.point(face_vert);
			glVertex3fv(p.data());
			++face_vert;
		}
	}
	
	OMesh& mesh;
};


#endif