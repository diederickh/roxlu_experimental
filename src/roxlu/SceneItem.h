#ifndef SCENEITEMH
#define SCENEITEMH

#include "VBO.h"
#include "VAO.h"
#include "Shader.h"

class SceneItem {
public:
	SceneItem();
	~SceneItem();
	
	VBO* vbo;
	VAO* vao;
	Shader* shader;	
};
#endif