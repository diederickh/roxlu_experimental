#include "SceneItem.h"
#include "OBJ.h"
#include "File.h"
#include "HalfEdge.h"
#include "Material.h"
#include "Mat4.h"

namespace roxlu {

OBJ::OBJ() {
}

OBJ::~OBJ() {
}

void OBJ::exportSceneItem(SceneItem* si) {
	return exportSceneItem(*si);
}

void OBJ::createMaterialFile(Material& mat) {
	string material_file = "mat_" +mat.getName() +".mtl";
	printf("Material file: '%s'\n", material_file.c_str());
	stringstream ss;
	ss << "newmtl " << mat.getName() << endl;
	ss << "Ka 1.0 1.0 1.0" << endl;
	ss << "Kd 1.0 1.0 1.0" << endl;
	ss << "Ks 1.0 1.0 1.0" << endl;
	ss << "Ni 1.0" << endl;
	ss << "d 1.0" << endl;
	ss << "illum 2" << endl;
	
	if(mat.hasDiffuseTexture()) {
		printf("File: '%s'\n", mat.getDiffuseTextureFilePath().c_str());	
		ss << "map_Kd " << mat.getDiffuseTextureFilePath() << endl;
	}
	else {
		printf("??????");
	}
	File::putFileContents(File::toDataPath(material_file), ss.str());

}

void OBJ::exportSceneItem(SceneItem& si) {
	Mat4 mm = si.mm();
	stringstream ss;
	ss << "# Roxlu OBJ export 0.01" << endl;	
	ss << "o " << si.getName() << endl;

	createMaterialFile(*si.getMaterial());
	ss << "mtllib mat_" << (*si.getMaterial()).getName() << ".mtl" << endl;
	
	ss.precision(5);
	ss.setf(ios::fixed,ios::floatfield);

	VertexData& vd = *si.getVertexData();		
	
	// vertices
	for(int i = 0; i < vd.getNumVertices(); ++i) {
		Vec3& v = *vd.getVertexPtr(i);
		v = mm.transform(v);
		ss << "v " << v.x << " " << v.y << " " << v.z << endl;
	}
	
	// texcoords
	for(int i = 0; i < vd.getNumTexCoords(); ++i) {
		Vec2& tc = *vd.getTexCoordPtr(i);
		ss << "vt " << tc.x << " " << tc.y << endl;
	}
		
	if(si.getMaterial()->hasDiffuseTexture()) {
		ss << "usemtl " << si.getMaterial()->getName() << endl;
	}
	if(vd.getNumTexCoords() == 0) {
		// quads.
		for(int i = 0; i < vd.getNumQuads(); ++i) {
			Quad& q = *vd.getQuadPtr(i);
			//printf("%d, %d, %d, %d <---\n", q.a, q.b, q.c, q.d);
			ss << "f " << q.d+1 << " " << q.c+1 << " " << q.b+1 << endl;
			ss << "f " << q.b+1 << " " << q.a+1 << " " << q.d+1 << endl;
		}
		
		// triangles
		for(int i = 0; i < vd.getNumTriangles(); ++i) {
			Triangle& t = *vd.getTrianglePtr(i);
			ss << "f " << t.a+1 << " " << t.b+1 << " " << t.c+1 << endl;
		}
	}
	else {
		// quads.
		for(int i = 0; i < vd.getNumQuads(); ++i) {
			Quad& q = *vd.getQuadPtr(i);
			ss << "f " 	<< q.d+1 << "/" << q.d+1 << " "
						<< q.c+1 << "/" << q.c+1 << " "
						<< q.b+1 << "/" << q.b+1 << endl;
						
			ss << "f " 	<< q.b+1 << "/" << q.b+1 << " "
						<< q.a+1 << "/" << q.a+1 << " "
						<< q.d+1 << "/" << q.d+1 << endl;
			
		
		}
		
		// triangles
		for(int i = 0; i < vd.getNumTriangles(); ++i) {
			Triangle& t = *vd.getTrianglePtr(i);
			ss << "f " 	<< t.a+1 << "/" << t.a+1 << " "
						<< t.b+1 << "/" << t.b+1 << " "
						<< t.c+1 << "/" << t.c+1 << endl;
		}
	}
	
	// create unique list with vertices.
	/*
	vector<Vec2> unique_texcoords;
	vector<Vec3> unique_vertices;
	VertexData& vd = *si.getVertexData();	
	for(int i = 0; i < vd.getNumVertices(); ++i) {
		bool found = false;
		Vec3& a = *vd.getVertexPtr(i);
		int dx = 0;
		for(int j = 0; j < unique_vertices.size(); ++j) {
			Vec3 b = unique_vertices[j];
			if(b.lengthSquared(a) < 0.01) {
				dx = j;
				found = true;
				break;
			}
		}
		if(!found) {
			unique_vertices.push_back(a);
			
			// copy texcoord
			if(vd.getNumTexCoords() > 0) {
				unique_texcoords.push_back(vd.getTexCoord(i));
			}
		}
	}

	// write vertices
	for(int i = 0; i < unique_vertices.size(); ++i) {
		Vec3& v = unique_vertices[i];
		ss << "v " << v.x << " " << v.y << " " << v.z << endl;
	}
	
	// now for face find the index from the unique vertices.
	
	int num_quads = vd.getNumQuads();
	if(num_quads > 0) {
		vector<Quad> new_quads;
		for(int i = 0; i < vd.getNumQuads(); ++i) {
			Quad new_quad;
			Quad& q = *vd.getQuadPtr(i);
			Vec3 v;
			// = vd.getVertex(q.a);
			for(int k = 0; k < 4; ++k) {
				v = vd.getVertex(q[k]);
				for(int j = 0; j < unique_vertices.size(); ++j) {
					if(v.lengthSquared(unique_vertices[j]) < 0.01) {
						printf("%d wordt %d\n", q[k], j);
						new_quad[k] = j;
						break;
					}
				}
			}
			new_quads.push_back(new_quad);
		}
	
		for(int i = 0; i < new_quads.size(); ++i) {
			Quad& q = new_quads[i];
			ss << "f " << q.d+1 << " " << q.c+1 << " " << q.b+1 << endl;
			ss << "f " << q.b+1 << " " << q.a+1 << " " << q.d+1 << endl;
		}

		printf("We have: %d  unique vertices\n", unique_vertices.size());		
	}
	
	*/
	
			

		
	// And write everything to a file.
	File::putFileContents(File::toDataPath("test2.obj"), ss.str());

}



}; // roxlu