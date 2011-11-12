#ifndef ROXLU_RENDERERH
#define ROXLU_RENDERERH

#include "EasyCam.h"
#include "VertexData.h"
#include "SceneItem.h"
//#include "Texture.h" 
//#include "OpenGL.h"


#include "Scene.h"

#include <string>

using std::string;

namespace roxlu {

/**
 * Goal of this Renderer class is to make it as easy as possible to create
 * a 3D/2D scene using modern openGL, not to provide the best implementation of 
 * object oriented library.
 *
 *
 */
 
class Scene;
class EasyCam;
class Shader;
class Texture;
class Material;
class ShaderGenerator;
class Light;

class Renderer {
public:
	Renderer(float screenWidth, float screenHeight);
	~Renderer();
	void render();
	void renderSceneItem(string name);
	
	inline EasyCam* getCamera();
	
	// shader.
	inline void setShader(Shader* someShader);
	inline void setShader(Shader& someShader);
	inline Shader* getShader();
	ShaderGenerator createShaderGenerator(SceneItem& si);
	
	inline void setScene(Scene* someScene);	
	inline void setScene(Scene& scene);
	inline void	fill();
	inline void noFill();
	inline void translate(float x, float y, float z);
	
	inline SceneItem* getSceneItem(string name);
	SceneItem* createIcoSphere(string name, int detail, float radius);
	SceneItem* createUVSphere(string name, int phi, int theta, float radius);
	SceneItem* createBox(string name, float width, float height, float depth);
	SceneItem* createPlane(string name, float width, float height);
	Material* createMaterial(string name);
	void loadDiffuseMaterial(string materialName, string textureName, string diffuseFileName, GLuint imageFormat = GL_RGB);
	Material* createDiffuseMaterial(string materialName, string textureName, string diffuseFileName, GLuint imageFormat = GL_RGB);
	inline Material* getMaterial(string materialName);
	inline void setSceneItemMaterial(string sceneItemName, string materialName);
	
	void onMouseDown(float x, float y);
	void onMouseDragged(float x, float y);
	void exportToPly(string sceneItemName, string fileName);
	Texture* createTexture(string name, string fileName);
	Light* createLight(string name, float r = 1.0, float g = 1.0, float b = 1.0);
	

private:
	bool use_fill;
	float screen_width;
	float screen_height;
	Scene* scene;
	EasyCam* cam;
	Shader* shader;
};

inline void Renderer::fill() {
	use_fill = true;
}

inline void Renderer::noFill() {
	use_fill = false;
}

inline void Renderer::setShader(Shader& someShader) {
	setShader(&someShader);
}

inline void Renderer::setShader(Shader* someShader) {
	shader = someShader;
}

inline void Renderer::setScene(Scene& someScene) {
	setScene(&someScene);
}

inline void Renderer::setScene(Scene* someScene) {
	scene = someScene;
}

inline SceneItem* Renderer::getSceneItem(string name) {
	return scene->getSceneItem(name);
}

inline Shader* Renderer::getShader() {
	return shader;
}

inline void Renderer::translate(float x, float y, float z) {
	cam->translate(x,y,z);
}

inline Material* Renderer::getMaterial(string materialName) {
	return scene->getMaterial(materialName);
}

inline void Renderer::setSceneItemMaterial(string sceneItemName, string materialName) {
	getSceneItem(sceneItemName)->setMaterial(getMaterial(materialName));
}

inline EasyCam* Renderer::getCamera() {
	return cam;
}


} // roxlu
#endif