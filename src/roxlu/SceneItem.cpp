#include "SceneItem.h"
#include <cstdlib>

namespace roxlu {

SceneItem::SceneItem() {
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
	
	vao->bind();
	shader->enable();
	
	// create the VBO
	vbo = new VBO();
	Vertex*	vertex_data = NULL;
	size_t stride = 0;
	int pos_offset = 0;

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

void SceneItem::drawArrays() {
	vao->bind();
	shader->enable();
	
	glDrawArrays(GL_QUADS, 0, 4); eglGetError();
	shader->disable();
	vao->unbind();
}

} // roxlu