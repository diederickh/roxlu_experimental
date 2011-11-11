#include "SceneItem.h"
#include "OBJ.h"
#include "File.h"
#include "HalfEdge.h"
namespace roxlu {

OBJ::OBJ() {
}

OBJ::~OBJ() {
}

void OBJ::exportSceneItem(SceneItem* si) {
	exportSceneItem(*si);
}

void OBJ::exportSceneItem(SceneItem& si) {
	stringstream ss;
	ss << "# Roxlu OBJ export 0.01" << endl;	
	ss << "o " << si.getName() << endl;
	ss.precision(5);
	ss.setf(ios::fixed,ios::floatfield);
	
	/*
	// write vertices.
	VertexData& vd = *si.getVertexData();
	for(int i = 0; i < vd.getNumVertices(); ++i) {
		Vec3 v = vd.getVertex(i);
		ss << "v " << v.x << " " << v.y << " " << v.z << endl;
	}
	
//	for(int i = 0; i < vd.getNumQuads(); ++i) {
//		Quad& q = *vd.getQuadPtr(i);
//		Vec3 a = vd.getVertex(q.a);
//		Vec3 b = vd.getVertex(q.b);
//		Vec3 c = vd.getVertex(q.c);
//		Vec3 d = vd.getVertex(q.d);
//		ss << "v " << a.x << " " << a.y << " " << a.z << endl;
//		ss << "v " << b.x << " " << b.y << " " << b.z << endl;
//		ss << "v " << c.x << " " << c.y << " " << c.z << endl;
//		ss << "v " << d.x << " " << d.y << " " << d.z << endl;
//		//ss << "f " << q.a+1 << " " << q.b+1 << " " << q.c+1 << " " << q.d+1 << endl;
//	}
	

	//ss << "s off" << endl;
	// write faces
	for(int i = 0; i < vd.getNumTriangles(); ++i) {
		Triangle& t = *vd.getTrianglePtr(i);
//		ss << "f " << t.a+1 << " " << t.b+1 << " " << t.c+1 << endl;
	}
	printf("Number of quads: %d\n", vd.getNumQuads()) ;
	for(int i = 0; i < vd.getNumQuads(); ++i) {
		Quad& q = *vd.getQuadPtr(i);
		ss << "f " << q.a+1 << " " << q.b+1 << " " << q.c+1 << " " << q.d+1 << endl;
	}
	*/	

	
	// And write everything to a file.
	File::writeToFile(File::toDataPath("test2.obj"), ss.str());

}



}; // roxlu