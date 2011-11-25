#include "SceneItem.h"
#include "experimental/Effect.h"
#include "experimental/Light.h"
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
{
	vao = new VAO();
	vbo = new VBO();	
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

void SceneItem::draw(Mat4& viewMatrix, Mat4& projectionMatrix, Mat3& normalMatrix) {
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
	effect->getShader().enable();
		
	if(material != NULL) {
		effect->bindMaterial(*material);
	}
	
	effect->updateLights();
	effect->getShader().uniformMat3fv("normalmatrix", normalMatrix.getPtr());
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

} // roxlu