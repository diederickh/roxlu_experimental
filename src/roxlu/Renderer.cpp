#include "experimental/Texture.h"
#include "Renderer.h"
#include "Camera.h"
#include "Scene.h"
#include "SceneItem.h"
#include "Shader.h"
#include "IcoSphere.h"
#include "File.h"
#include "Ply.h"
#include "experimental/StringUtil.h"
#include "experimental/UVSphere.h"
#include "experimental/Material.h"
#include <iostream>

using namespace std;

namespace roxlu {

Renderer::Renderer(float screenWidth, float screenHeight) 
:cam(NULL)
,shader(NULL)
,scene(NULL)
,use_fill(true)
,screen_width(screenWidth)
,screen_height(screenHeight)
{
	// create camera
	cam = new EasyCam();
	cam->setup(screen_width, screen_height);
	cam->translate(0,0,-10);
	
	// create a scene instance (which is just a container for all created scene
	// items.
	scene = new Scene();
	
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
//		printf("Rendering: '%s'\n", it->first.c_str());
		SceneItem& si = *(it->second);
		Mat4 modelview_matrix = view_matrix * si.mm();
		Mat4 modelview_projection_matrix = projection_matrix * modelview_matrix ;
//		modelview_matrix.print();
		shader->enable();
		shader->uniformMat4f("modelview", modelview_matrix.getPtr());
		shader->uniformMat4f("modelview_projection", modelview_projection_matrix.getPtr());
		si.draw();
		++it;
	}
}

SceneItem* Renderer::createIcoSphere(string name, int detail, float radius) {
	VertexData* vd = new VertexData();
	SceneItem* si = new SceneItem();

	// Create vertex data for ico sphere.	
	IcoSphere ico_sphere;
	ico_sphere.create(detail, radius, *vd);
	
	// Create scene item from vertex data and make sure shader is set.
	si->setShader(shader);
	si->createFromVertexData(vd);
	si->setDrawMode(SceneItem::TRIANGLES);
	
	// Keep track of the created data.
	scene->addSceneItem(name, si);
	scene->addVertexData(name, vd);

	return si;
}

SceneItem* Renderer::createUVSphere(string name, int phi, int theta, float radius) {
	VertexData* vd = new VertexData();
	SceneItem* si = new SceneItem();

	// Create vertex data for ico sphere.	
	UVSphere uv_sphere;
	uv_sphere.create(radius, phi, theta, *vd);
	
	// Create scene item from vertex data and make sure shader is set.
	si->setShader(shader);
	si->createFromVertexData(vd);
	si->setDrawMode(SceneItem::QUADS);
	
	// Keep track of the created data.
	scene->addSceneItem(name, si);
	scene->addVertexData(name, vd);
	
	return si;
}


void Renderer::onMouseDown(float x, float y) {
	cam->onMouseDown(x,y);
}

void Renderer::onMouseDragged(float x, float y) {
	cam->onMouseDragged(x,y);
}

// exports given vertex data to .ply format.
void Renderer::exportToPly(string sceneItemName, string fileName) {
	SceneItem* si = getSceneItem(sceneItemName);
	string path = File::toDataPath(fileName);
	Ply ply;
	ply.save(path, *si->getVertexData());
	printf("Saved: '%s'\n", path.c_str());
}

Texture* Renderer::createTexture(string name, string fileName) {
	Texture* tex = new Texture();
	tex->loadImage(fileName);
	scene->addTexture(name, tex);
	return tex;
}

// Create a diffuse material.
Material* Renderer::createDiffuseMaterial(
		 string materialName
		,string textureName
		,string diffuseFileName
		,GLuint imageFormat) 
{
	Material* mat = new Material();
	mat->setShader(shader);
	Texture* tex = mat->loadDiffuseMaterial(diffuseFileName, imageFormat);
	
	scene->addMaterial(materialName, mat);
	scene->addTexture(textureName, tex);
	return mat;
}

// loads a diffuse material and sets it for the given material group
void Renderer::loadDiffuseMaterial(
		 string materialName
		,string textureName
		,string diffuseFileName
		,GLuint imageFormat 
)
{
	Material* mat = getMaterial(materialName);
	if(mat == NULL) {
		mat = createDiffuseMaterial(materialName, textureName, diffuseFileName, imageFormat);
		return;
	}
	else {
		Texture* tex = mat->loadDiffuseMaterial(diffuseFileName, imageFormat);
		scene->addTexture(textureName, tex);
	}
}

} // roxlu