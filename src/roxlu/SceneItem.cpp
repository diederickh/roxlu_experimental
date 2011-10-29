#include "SceneItem.h"
#include <cstdlib>

namespace roxlu {

SceneItem::SceneItem() 
:vertex_data(NULL)
,vao(NULL)
,vbo(NULL)
,draw_mode(POINTS)
{
	vao = new VAO();
}

SceneItem::~SceneItem() {
}


bool SceneItem::createFromVertexData(VertexData& vd) {
	createFromVertexData(&vd);
}

bool SceneItem::createFromVertexData(VertexData* vd) {
	if(shader == NULL) {
		printf("error SceneItem: you need to set the shader before creating from vertex data\n");
		exit(1);
	}	
	vertex_data = vd;
	
	vao->bind();
	shader->enable();
	
	// create the VBO
	vbo = new VBO();
	Vertex*	vertex_data = NULL;
	size_t stride = 0;
	int pos_offset = 0;

	// when we have shared vertices (aka indices) set them.
	if(vd->getNumIndices() > 0) {
		vbo->setIndices(vd->getIndices(), vd->getNumIndices());
	}


	// set data and set offsets
	if(vd->attribs == VBO_TYPE_VERTEX_P) {
		stride = sizeof(VertexP);
		vbo->setVertexData(
			 vd->getVertexP()
			,vd->getNumVertices()
		);
	}

	// set attributes.
	if(vd->attribs & VERT_POS) {
		GLuint pos_id = shader->addAttribute("pos").getAttribute("pos");
		if(pos_id != -1) {
			shader->enableVertexAttribArray("pos");
			glVertexAttribPointer(
				pos_id
				,3
				,GL_FLOAT
				,GL_FALSE
				,stride
				,(GLvoid*)pos_offset
			);
		}
	}	
		
	shader->disable();
	return true;
}

// @todo: we created a debugDrag in vertex_data, this one should be removed 
// mabye?
void SceneItem::debugDraw() {
	if(vbo->hasIndices()) {
		glColor3f(0,1,0.4);
		shader->disable();
		glBegin(GL_POINTS);
		for(int i = 0; i < vertex_data->indices.size(); ++i) {
			Vec3 p = vertex_data->vertices[vertex_data->indices[i]];
			glVertex3fv(p.getPtr());
		}
		glEnd();
		
	}
	else { 
	}
}

void SceneItem::drawArrays() {
	vao->bind();
	shader->enable();
		glDrawArrays(draw_mode, 0, vertex_data->getNumVertices()); eglGetError();
	shader->disable();
	vao->unbind();
}

void SceneItem::drawElements() {
	vao->bind();
	shader->enable();
	vbo->bind();
		glDrawElements(draw_mode, vertex_data->getNumIndices(), GL_UNSIGNED_INT, NULL); eglGetError();
	shader->disable();
	vao->unbind();
}

void SceneItem::draw() {
	if(vbo == NULL) {
		printf("SceneItem no vbo set.\n"); 
		exit(1);
	}
	else if(vertex_data == NULL) {
		printf("SceneItem no vertex data set\n");
		exit(1);
	}
	
	if(vbo->hasIndices()) {
		drawElements();
	}
	else {
		drawArrays();
	}
}


} // roxlu