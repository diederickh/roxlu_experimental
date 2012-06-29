#include "OMesh.h"

OMesh::OMesh() {

}

OMesh::~OMesh() {
}

bool OMesh::save(const string& filepath) {
	try { 	
		OpenMesh::IO::Options opt;
		opt += OpenMesh::IO::Options::VertexNormal; 
		opt += OpenMesh::IO::Options::FaceNormal; 

		if(!OpenMesh::IO::write_mesh(mesh, filepath),opt) {
			return false;
		}
		

	}
	catch(std::exception& x) {	
		printf("Cannot write mesh: %s, %s\n", filepath.c_str(), x.what());
		return false;
	}
	return true;
}

bool OMesh::load(const string& filepath) {
	try { 
		OpenMesh::IO::Options opt;
		opt += OpenMesh::IO::Options::VertexNormal; 
		opt += OpenMesh::IO::Options::FaceNormal; 
		if(!OpenMesh::IO::read_mesh(mesh, filepath, opt)) {
			printf("Cannot read: %s\n", filepath.c_str());
			return false;
		}
	}
	catch(std::exception& x) {	
		printf("Cannot read mesh: %s, %s\n", filepath.c_str(), x.what());
		return false;
	}
	return true;
}

void OMesh::debugDraw() {
	glEnable(GL_CULL_FACE);
	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
		
	// draw faces.
	glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
	glColor4f(0.2f, 0.3f, 0.4f, 0.5f);
	glBegin(GL_TRIANGLES);
		std::for_each(mesh.faces_begin(), mesh.faces_end(), OMesh_FaceDrawer(*this));
	glEnd();
		
	
	// draw triangles.
	glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
	glColor4f(8.0f, 0.3f, 0.05f, 0.8f);
	glBegin(GL_TRIANGLES);
		std::for_each(mesh.faces_begin(), mesh.faces_end(), OMesh_FaceDrawer(*this));
	glEnd();
	glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
	

	// draw face normals.
	glColor3f(0.1, 0.8, 0.7);
	if(mesh.has_face_normals()) {
		glBegin(GL_LINES);
		MeshType::FaceIter face_it =  mesh.faces_begin();
		while(face_it != mesh.faces_end()) {
			
			// It seems that mesh.normal(face_it.handle()) returns an incorrect normal!
			//MeshType::Normal norm = mesh.normal(face_it.handle());	
			
			MeshType::Normal norm = getFaceNormal(face_it.handle());
			MeshType::Point center = getFaceCenter(face_it.handle());
			MeshType::Point end = center + norm;
			glVertex3fv(center.data());
			glVertex3fv(end.data());
			++face_it;
		}
		glEnd();
	}
}

void OMesh::debugDrawFace(const MeshType::FaceHandle& faceHandle) {
	if(!mesh.is_valid_handle(faceHandle)) {
		return;
	}
	
	OMesh_FaceDrawer drawer(*this);
	glBegin(GL_TRIANGLES);
		drawer(mesh.face(faceHandle));
	glEnd();
}

// assuming trimesh
MeshType::Normal OMesh::getFaceNormal(const MeshType::Face& face) {
	return getFaceNormal(mesh.handle(face));
}
MeshType::Normal OMesh::getFaceNormal(const MeshType::FaceHandle& faceHandle) {
	MeshType::HalfedgeHandle heh_a, heh_b, heh_c;
	heh_a = mesh.halfedge_handle(faceHandle);
	heh_b = mesh.next_halfedge_handle(heh_a);
	heh_c = mesh.next_halfedge_handle(heh_b);
	
	MeshType::Point a = mesh.point(mesh.to_vertex_handle(heh_a));
	MeshType::Point b = mesh.point(mesh.to_vertex_handle(heh_b));
	MeshType::Point c = mesh.point(mesh.to_vertex_handle(heh_c));
	MeshType::Point ab = b - a;
	MeshType::Point ac = c - a;
	MeshType::Normal norm = ab % ac;
	return norm;
}


// we assume a trimesh!
MeshType::Point OMesh::getFaceCenter(const MeshType::FaceHandle& faceHandle) {
	return getFaceCenter(mesh.halfedge_handle(faceHandle));
}

MeshType::Point OMesh::getFaceCenter(const MeshType::Face& face) {
	return getFaceCenter(mesh.handle(face));

}

MeshType::Point OMesh::getFaceCenter(const MeshType::HalfedgeHandle& faceHalfEdge) {
	MeshType::HalfedgeHandle heh_a, heh_b, heh_c;
	heh_a = faceHalfEdge;
	heh_b = mesh.next_halfedge_handle(heh_a);
	heh_c = mesh.next_halfedge_handle(heh_b);
	MeshType::Point a = mesh.point(mesh.to_vertex_handle(heh_a));
	MeshType::Point b = mesh.point(mesh.to_vertex_handle(heh_b));
	MeshType::Point c = mesh.point(mesh.to_vertex_handle(heh_c));
	MeshType::Point ab = b - a;
	MeshType::Point ac = c - a;
	MeshType::Point ba = a - b;
	MeshType::Point bc = c - b;
	MeshType::Point bisect_a = ab+ac;
	MeshType::Point bisect_b = ba+bc;
	MeshType::Point intersection = lineIntersection(a,bisect_a,b,bisect_b);
	return intersection;
}



// assuming given vectors cross, we return the intersection point.
MeshType::Point OMesh::lineIntersection(
		const MeshType::Point& originA
		,const MeshType::Point& dirA
		,const MeshType::Point& originB
		,const MeshType::Point& dirB
)
{
	MeshType::Point dir_a = dirA;
	MeshType::Point dir_b = dirB;
	dir_a.normalize();
	dir_b.normalize();

	MeshType::Point n = dir_a % dir_b; 
	MeshType::Point sr = originA - originB;
	MeshType::Scalar abs_x = abs(n[0]);
	MeshType::Scalar abs_y = abs(n[1]);
	MeshType::Scalar abs_z = abs(n[2]);
	float t;
	
	if(abs_z > abs_x && abs_z > abs_y) {
		t = (sr[0] * dir_b[1] - sr[1] * dir_b[0]) / n[2];
	}
	else if(abs_x > abs_y) {
		t = (sr[1] * dir_b[2] - sr[2] * dir_b[1]) / n[0];
	}
	else  {
		t = (sr[2] * dir_b[0] - sr[0] * dir_b[2]) / n[1];
	}
	return originA - t * dir_a;
}

// get the closest face for the given position
MeshType::FaceHandle OMesh::getClosestFace(const MeshType::Point& pos) {
	float min_dist = FLT_MAX;
	float dist = 0.0f;
	MeshType::FaceHandle found_handle;
	MeshType::FaceIter face_it =  mesh.faces_begin();
	while(face_it != mesh.faces_end()) {
		MeshType::Point face_center = getFaceCenter(face_it.handle());
		MeshType::Point dir = face_center - pos;
		dist = dir.length();
		if(dist < min_dist) {
			found_handle = face_it.handle();
			min_dist = dist;
		}
		++face_it;
	}
	
	return found_handle;
}
