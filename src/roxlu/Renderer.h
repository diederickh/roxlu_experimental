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
	void render();
	void renderSceneItem(string name);
	
	inline EasyCam* getCamera();
	
	inline Effect* getEffect();
	
	inline void setScene(Scene* someScene);	
	inline void setScene(Scene& scene);
	inline void	fill();
	inline void noFill();
	inline void translate(float x, float y, float z);
	
	inline SceneItem* getSceneItem(string name);
	inline SceneItem* duplicateSceneItem(string oldName, string newName);
	SceneItem* createIcoSphere(string name, int detail, float radius);
	SceneItem* createUVSphere(string name, int phi, int theta, float radius);
	SceneItem* createBox(string name, float width, float height, float depth);
	SceneItem* createPlane(string name, float width, float height);
	
	// materials
	Material* createMaterial(string name);
	inline Material* getMaterial(string materialName);
	inline void setSceneItemMaterial(string sceneItemName, string materialName);
	inline void setSceneItemPosition(string sceneItemname, float x, float y, float z);

	Material* createDiffuseTexture(string materialName, string textureName, string diffuseFileName, GLuint imageFormat = GL_RGB);
	Material* createNormalTexture(string materialName, string textureName, string normalFileName, GLuint imageFormat = GL_RGB);
		
	// general
	void onMouseDown(float x, float y);
	void onMouseDragged(float x, float y);
	void exportToPly(string sceneItemName, string fileName);
	void exportToOBJ(string fileName);
	void exportToR3F(string fileName);
	Texture* createTexture(string name, string fileName);
	
	// light
	Light* createLight(string name, float r = 1.0, float g = 1.0, float b = 1.0, float a = 1.0);
	void setLightPosition(string name, float x, float y, float z);
	void setLightDiffuseColor(string name, float r, float g, float b, float a = 1.0);
	void setLightSpecularColor(string name, float r, float g, float b, float a = 1.0);	
	Light* getLight(string name);
	
private:
	bool use_fill;
	float screen_width;
	float screen_height;
	Scene* scene;
	EasyCam* cam;
	Effect* effect;
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

inline Effect* Renderer::getEffect() {
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


/* working example (also mouse rotation) 

void testApp::setup(){
	ofVec3f d;
	ofEnableAlphaBlending();
	ofBackground(33);
	ofSetFrameRate(60);
	ofSetVerticalSync(true);
	renderer.getEffect()->enableDiffuseTexture();
	renderer.translate(0,0,-5);	
	renderer.createDiffuseMaterial("sphere_diffuse", "lava", "lavasym.jpg");
	renderer.createLight("spot", 1.0, 0.0, 0.0);
	renderer.createUVSphere("sphere", 40,20,15);
	renderer.createBox("uv", 38,6,1);
	renderer.setSceneItemMaterial("sphere","sphere_diffuse");
	renderer.setSceneItemMaterial("uv","sphere_diffuse");
}

//--------------------------------------------------------------
void testApp::update(){
	ofSetWindowTitle(ofToString(ofGetFrameRate()));
	float x = cos(ofGetElapsedTimef() * 0.4) * 10;
	float y = sin(ofGetElapsedTimef() * 0.4) * 5;
	renderer.getSceneItem("sphere")->setPosition(x+2,y,0);
	renderer.getSceneItem("sphere")->rotateY(0.005);
	renderer.getSceneItem("uv")->rotateX(0.005);
}


//--------------------------------------------------------------
void testApp::draw(){
	glEnable(GL_DEPTH_TEST);
	bool test = true;
	if(test) {
		renderer.render();
		int n = 5;
		for(int i = 0; i < n; ++i) {
			float p = ((float)i/n) * TWO_PI;
			float cx = cos(p) * 5;
			float cy = sin(p) * 5;
			renderer.getSceneItem("uv")->setPosition(cx,cy,0);
			renderer.renderSceneItem("uv");
		}
		
	}
	else {
		renderer.getCamera()->place();
		VertexData& vd = *renderer.getSceneItem("uv")->getVertexData();
		vd.debugDraw(GL_QUADS);

	}
	
	return;

}

*/



} // roxlu
#endif