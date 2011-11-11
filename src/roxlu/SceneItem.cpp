#include "SceneItem.h"
#include <cstdlib>

namespace roxlu {

SceneItem::SceneItem(string name) 
:vertex_data(NULL)
,vao(NULL)
,vbo(NULL)
,draw_mode(TRIANGLES)
,material(NULL)
,initialized(false)
,name(name)
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
}

void SceneItem::initialize() {
	if(initialized) {
		return;
	}
	initialized = true;
	vao->bind();
	shader->enable();

	// create the VBO
	vbo = new VBO();
	Vertex*	vertex = NULL;
	size_t stride = 0;
	int pos_offset = 0;
	int tex_offset = 0;
	VertexData* vd = vertex_data;
	
	// when we have shared vertices (aka indices) set them.
	if(vd->getNumIndices() > 0) {
		vbo->setIndices(vd->getIndicesPtr(), vd->getNumIndices());
	}


	// set data and set offsets
	if(vd->attribs == VBO_TYPE_VERTEX_P) {
		stride = sizeof(VertexP);
		vbo->setVertexData(
			 vd->getVertexP()
			,vd->getNumVertices()
		);
	}
	else if(vd->attribs == VBO_TYPE_VERTEX_PT) {
		printf("we have both pos+texcoords");
		stride = sizeof(VertexPT);
		vbo->setVertexData(
			 vd->getVertexPT()
			,vd->getNumVertices()
		);
		tex_offset = offsetof(VertexPT, tex);
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
			); eglGetError();
		}
	}	
	// attribute: texture
	if(vd->attribs & VERT_TEX) {
		GLuint tex_id = shader->addAttribute("tex").getAttribute("tex");
		if(tex_id != -1) {
			shader->enableVertexAttribArray("tex");
			glVertexAttribPointer(
				tex_id
				,2
				,GL_FLOAT
				,GL_FALSE
				,stride
				,(GLvoid*)tex_offset
			); eglGetError();
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
		glBegin(GL_POINTS); eglGetError();
		for(int i = 0; i < vertex_data->indices.size(); ++i) {
			Vec3 p = vertex_data->vertices[vertex_data->indices[i]];
			glVertex3fv(p.getPtr()); eglGetError();
		}
		glEnd(); eglGetError();
		
	}
	else { 
	}
}

void SceneItem::drawArrays() {
	vao->bind();
	shader->enable();

	if(material != NULL) {
		material->bind();
	}
	glDrawArrays(draw_mode, 0, vertex_data->getNumVertices()); eglGetError();
	
	if(material != NULL) {
		material->unbind();
	}


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
	
	if(vbo->hasIndices()) {
		drawElements();
	}
	else {
		drawArrays();
	}
}


} // roxlu