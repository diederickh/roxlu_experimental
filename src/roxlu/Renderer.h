#ifndef ROXLU_RENDERERH
#define ROXLU_RENDERERH

#include "EasyCam.h"
#include "VertexData.h"
#include "SceneItem.h"
#include "Effect.h"
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
	void 				draw();
	void 				debugDraw();
	void 				drawSceneItem(string name);
	void 				addSceneItem(string name, SceneItem* si);
	void 				addSceneItem(SceneItem* si);
	inline SceneItem* 	duplicateSceneItem(string oldName, string newName);
	inline SceneItem* 	getSceneItem(string name);

	inline Effect* 		getEffectPtr();
	
	inline void 		setScene(Scene* someScene);	
	inline void 		setScene(Scene& scene);

	inline void			fill();
	inline void 		noFill();
	
	// camera
	inline EasyCam* 	getCamera();
	inline void 		translate(float x, float y, float z);
	
	// meshes
	SceneItem* 			createIcoSphere(string name, int detail, float radius);
	SceneItem* 			createUVSphere(string name, int phi, int theta, float radius);
	SceneItem* 			createBox(string name, float width, float height, float depth);
	SceneItem* 			createPlane(string name, float width, float height);
	
	// texture and materials
	Material* 			createMaterial(string name);
	inline Material*	getMaterial(string materialName);
	inline void 		setSceneItemMaterial(string sceneItemName, string materialName);
	inline void 		setSceneItemPosition(string sceneItemname, float x, float y, float z);
	Material* 			createDiffuseTexture(string materialName, string textureName, string diffuseFileName, GLuint imageFormat = GL_RGB);
	Material* 			createNormalTexture(string materialName, string textureName, string normalFileName, GLuint imageFormat = GL_RGB);
	Texture* 			createTexture(string name, string fileName);
			
	// general
	void 				onMouseDown(float x, float y);
	void 				onMouseDragged(float x, float y);
	
	// Exporting
	void 				exportToPly(string sceneItemName, string fileName, bool inDataPath = true);
	void 				exportToOBJ(string fileName, bool inDataPath = true);
	void 				exportToR3F(string fileName, bool inDataPath = true);

	
	// light
	Light* 				createLight(string name, float r = 1.0, float g = 1.0, float b = 1.0, float a = 1.0);
	void 				setLightPosition(string name, float x, float y, float z);
	void	 			setLightDiffuseColor(string name, float r, float g, float b, float a = 1.0);
	void 				setLightSpecularColor(string name, float r, float g, float b, float a = 1.0);	
	Light* 				getLight(string name);
	
private:
	bool 		use_fill;
	float 		screen_width;
	float 		screen_height;
	Scene* 		scene;
	EasyCam* 	cam;
	Effect* 	effect;
};

inline void Renderer::fill() {
	use_fill = true;
}

inline void Renderer::noFill() {
	use_fill = false;
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

inline Effect* Renderer::getEffectPtr() {
	return effect;
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

inline SceneItem* Renderer::duplicateSceneItem(string oldName, string newName) {
	SceneItem* si = getSceneItem(oldName);
	if(si == NULL) {
		printf("Error: cannot duplicate scene item: '%s' as we cannot find it\n", oldName.c_str());
		return NULL;
	}
	
	// @todo a scene item must be initialized before it can be duplicated... 
	// this can lead to confusing when i.e. a material hasn't be applied yet.
	// then the material will not show up.
	si->initialize();
	SceneItem* duplicated = si->duplicate();

	scene->addSceneItem(newName, duplicated);
	return duplicated;
}

inline void Renderer::setSceneItemPosition(string name, float x, float y, float z) {
	getSceneItem(name)->setPosition(x,y,z);
}

} // roxlu
#endif