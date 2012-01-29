/*
#include "SceneItem.h"
#include "experimental/Effect.h"
#include "experimental/Light.h"
*/
#include "../Roxlu.h"
#include <sstream>
#include <cstdlib>

using std::stringstream;

namespace roxlu {

SceneItem::SceneItem(string name) 
:vertex_data(NULL)
,vao(NULL)
,vbo(NULL)
,draw_mode(TRIANGLES)
,material(NULL)
,initialized(false)
,is_visible(true)
,effect(NULL)
,scaling(1.0,1.0, 1.0)
,name(name)
,color(1.0,1.0,1.0,1.0)
,specularity(8)
,attenuation(0.3, 0.5, 1.0)
{
	vao = new VAO();
	vbo = new VBO();	
	
	//test
	test_rot_set = false;
}

//SceneItem::SceneItem(const SceneItem& other) {
//	other.vao = vao;
//	other.vbo = vbo;
//	other.draw_mode = draw_mode;
//	other.effect = effect;
//	other.initialized = initialized;
//	other.model_matrix = model_matrix;
//	other.position = position;
//	other.setName(getName());
//}

SceneItem::~SceneItem() {
}

SceneItem* SceneItem::duplicate() {
	SceneItem* si = new SceneItem(name);
	*si = *this;
	return si;
}

bool SceneItem::createFromVertexData(VertexData& vd) {
	createFromVertexData(&vd);
}

bool SceneItem::createFromVertexData(VertexData* vd) {
	vertex_data = vd;
}


void SceneItem::initialize() {
	if(initialized) {
		return;
	}
	effect->setupBuffer(*vao, *vbo,  *vertex_data);
	initialized = true;
}

void SceneItem::drawArrays() {
	vao->bind();
		glDrawArrays(draw_mode, 0, vertex_data->getNumVertices()); eglGetError();
	vao->unbind();
}

void SceneItem::drawElements() {
	vao->bind();
		glDrawElements(draw_mode, vertex_data->getNumIndices(), GL_UNSIGNED_INT, NULL); eglGetError();
	vao->unbind();
}

void SceneItem::draw(Mat4& viewMatrix, Mat4& projectionMatrix) {

	if(!initialized) {
		initialize();
	}
	
	if(vbo == NULL) {
		printf("SceneItem no vbo set.\n"); 
		exit(1);
	}
	else if(vertex_data == NULL) {
		printf("SceneItem no vertex data set\n");
		exit(1);
	}
		
	Mat4 modelview_matrix = viewMatrix * mm();
	Mat4 modelview_projection_matrix = projectionMatrix * modelview_matrix ;

	// @todo we need implement something like: effect()->begin() and effect->end() 	
	// instead of effect->updateShaders + effect->bindMaterial etc...
	effect->updateShaders();  
	effect->enable();
		
	if(material != NULL) {
		effect->bindMaterial(*material);
	}

//	normalMatrix.m[0] = 1.0;
//	normalMatrix.m[1] = 0.0;
//	normalMatrix.m[2] = 0.0;
//	Mat4 modelview_copy = modelview_matrix;
//	Mat4 view_copy = viewMatrix;
//	Mat3 nm = modelview_copy.inverse().transpose();
	Mat3 nm = modelview_matrix.getInverse().transpose();
	effect->updateLights(); // isnt this done in renderer

	effect->getShader().uniform1f("specularity", specularity);
	effect->getShader().uniform4fv("diffuse_color", color.getPtr());
	effect->getShader().uniform3fv("attenuation", attenuation.getPtr());
	effect->getShader().uniformMat4fv("viewmatrix", viewMatrix.getPtr());
	effect->getShader().uniformMat3fv("normalmatrix", nm.getPtr());
	effect->getShader().uniformMat4fv("projection", projectionMatrix.getPtr());
	effect->getShader().uniformMat4fv("modelview", modelview_matrix.getPtr());
	effect->getShader().uniformMat4fv("modelview_projection", modelview_projection_matrix.getPtr());

	if(vbo->hasIndices()) {
		drawElements();
	}
	else {
		drawArrays();
	}
	
	effect->getShader().disable();
	
	if(material != NULL) {
		material->unbind();
	}

}

Mat3 SceneItem::getLookAtMatrix(const Vec3& pos, const Vec3& upVec) {
//	Mat3 m;
//	return m.getLookAtMatrix(position, pos, upVec);
}

void SceneItem::lookAt(const SceneItem* si) {
	return;
}

void SceneItem::lookAt(const Vec3& pos) {
	bool debug = false;
//	debug = !debug;
	Mat3 m = Mat3::getLookAtMatrix(position, pos);
	if(debug) {
		printf("SceneItem lookat:\n");
		m.print();
		printf("------------\n");
	}
	
	orientation.fromMat3(m);
	
	if(debug) {
		printf("Quaterion from mat3:\n");
		orientation.print();
		printf("--\n");
	}
	
	Mat4 quat_converted = orientation.getMat4() ;
	
	if(debug) {	
		printf("Quaternion converted to mat4:\n");
		quat_converted.print();
	}
	
	test_rot_matrix = m;
	test_rot_set = true;
	updateModelMatrix();
	
	if(debug) {
		printf("++++++++++++\n\n");
	}
	return;
}

} // roxlu