#include "Renderer.h"
#include "Camera.h"
#include "Scene.h"
#include "Shader.h"
#include "experimental/StringUtil.h"

#include <iostream>
using namespace std;

namespace roxlu {

Renderer::Renderer() 
:cam(NULL)
,shader(NULL)
,scene(NULL)
{
	// create default shader.
	shader = new Shader();
	StringUtil str_util(__FILE__);
	string path = str_util.split('/').pop().join("/") +"/experimental/shaders/";
	string vert_shader = StringUtil::stringFromFile(path +"uber.vert");
	string frag_shader = StringUtil::stringFromFile(path +"uber.frag");
	shader->create(vert_shader, frag_shader);
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
}


} // roxlu