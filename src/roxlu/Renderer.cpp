#include "Renderer.h"
#include "Camera.h"
#include "Scene.h"
#include "SceneItem.h"
#include "Shader.h"
#include "experimental/StringUtil.h"

#include <iostream>
using namespace std;

namespace roxlu {

Renderer::Renderer() 
:cam(NULL)
,shader(NULL)
,scene(NULL)
,use_fill(true)
{
	// create default shader.
	shader = new Shader();
	StringUtil str_util(__FILE__);
	string path = str_util.split('/').pop().join("/") +"/experimental/shaders/";
	string vert_shader = StringUtil::stringFromFile(path +"uber.vert");
	string frag_shader = StringUtil::stringFromFile(path +"uber.frag");
	shader->create(vert_shader, frag_shader);
	
	shader->addUniform("modelview")
		.addUniform("projection")
		.addUniform("modelview_projection");
}

Renderer::~Renderer() {
	if(shader != NULL) {
		delete shader;
	}
}

void Renderer::render() {
	if(cam == NULL || shader == NULL || scene == NULL) {
		printf("No cam, shader or scene set!\n");
		exit(1);
	}
	
	glEnable(GL_DEPTH_TEST);
	if(!use_fill) {
		glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
	}
	
	// create matrices for shaders
	cam->updateViewMatrix();
	Mat4& view_matrix = cam->vm();
	Mat4& projection_matrix = cam->pm();
	Mat4 projection_view_matrix = view_matrix * projection_matrix;

	shader->enable();
	shader->uniformMat4f("projection", projection_matrix.getPtr());
	
	// draw all scene items.
	Scene::scene_item_iterator it = scene->scene_items.begin();
	while(it != scene->scene_items.end()) {
		SceneItem& si = *(*it);
		Mat4 modelview_matrix = si.mm() * view_matrix;
		Mat4 modelview_projection_matrix = projection_matrix * modelview_matrix ;
		shader->uniformMat4f("modelview", modelview_matrix.getPtr());
		shader->uniformMat4f("modelview_projection", modelview_projection_matrix.getPtr());
		si.draw();
		++it;
	}
}


} // roxlu