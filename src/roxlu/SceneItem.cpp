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
,effect(NULL)
,scaling(1.0)
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
		
	if(material != NULL) {
		material->bind();
	}
//	printf("si: %s\n", name.c_str());
//	printf("effect: %p\n", effect);
//	printf("material:%p\n", material);
//	printf("--------\n\n");

	Mat4 modelview_matrix = viewMatrix * mm();
	Mat4 modelview_projection_matrix = projectionMatrix * modelview_matrix ;

	effect->getShader().enable();
		effect->updateLights();
		effect->getShader().uniformMat4f("projection", projectionMatrix.getPtr());
		effect->getShader().uniformMat4f("modelview", modelview_matrix.getPtr());
		effect->getShader().uniformMat4f("modelview_projection", modelview_projection_matrix.getPtr());
	
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