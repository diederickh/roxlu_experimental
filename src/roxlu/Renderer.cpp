#include "experimental/Texture.h"
#include "Renderer.h"
#include "Camera.h"
#include "Scene.h"
#include "SceneItem.h"
#include "Shader.h"
#include "IcoSphere.h"
#include "File.h"
#include "Ply.h"
#include "Light.h"
#include "OBJ.h"
#include "experimental/StringUtil.h"
#include "experimental/UVSphere.h"
#include "experimental/Plane.h"
#include "experimental/Box.h"
#include "experimental/Material.h"
#include "experimental/ShaderGenerator.h"
#include "experimental/R3F.h"
#include <iostream>
#include <map>

using namespace std;

namespace roxlu {

Renderer::Renderer(float screenWidth, float screenHeight) 
:cam(NULL)
,scene(NULL)
,use_fill(true)
,screen_width(screenWidth)
,screen_height(screenHeight)
{
	// create camera
	cam = new EasyCam();
	cam->setup(screen_width, screen_height);
	cam->translate(0,0,-10);
	
	// create a scene instance (which is just a container for all created scene  items.
	scene = new Scene();
	effect = new Effect(); // our default effect
}

Renderer::~Renderer() {
	printf("~Renderer");
	delete effect;
}

void Renderer::draw() {
	if(cam == NULL || scene == NULL) {
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
	Mat3 normal_matrix = cam->nm();

	// draw all scene items.
	Scene::scene_item_iterator it = scene->scene_items.begin();
	while(it != scene->scene_items.end()) {
		SceneItem& si = *(it->second);
		if(si.isVisible()) {
			//if(si.hasMaterial()) {
			//	effect->bindMaterial(*si.getMaterial());
			//}
			si.draw(view_matrix, projection_matrix, normal_matrix);
		}
		++it;
	}
}

void Renderer::debugDraw() {
//	glFrontFace(GL_CW);
//	glMatrixMode(GL_PROJECTION);
//	glLoadIdentity();
//	gluPerspective(45.0, 4.0f/3.0f, 0.01, 100);
//	
//	glMatrixMode(GL_MODELVIEW);
//	glLoadIdentity();
//	glTranslatef(0,0,-35);
//	glRotatef(sin(ofGetElapsedTimef()*0.5)*360,0,1,0);
	effect->disable();
	cam->place();
//	glEnable(GL_CULL_FACE);
	glEnable(GL_DEPTH_TEST);
	Mat4 rot;
	Scene::scene_item_iterator it = scene->scene_items.begin();
	while(it != scene->scene_items.end()) {
		SceneItem& si = *(it->second);
		VertexData& vd = *si.getVertexData();
		if(si.isVisible()) {
			glPushMatrix();
				Vec3& scale = si.getScale();
//				scale.print();
				
				Vec3 pos = si.getPosition();
				rot.identity();
				
				si.getOrientation().toMat4(rot);
				//glTranslatef(pos.x, pos.y, pos.z);
				glMatrixMode(GL_MODELVIEW);
				//glMultMatrixf(rot.getPtr());
				glMultMatrixf(si.mm().getPtr());
				//glScalef(scale.x, scale.y, scale.z);
				vd.debugDraw(si.getDrawMode());
			//si.draw(view_matrix, projection_matrix);
			glPopMatrix();
		}
		++it;
	}
	
	Scene::light_iterator lit = scene->lights.begin();
	while(lit != scene->lights.end()) {
		lit->second->debugDraw();
		++lit;
	}

}

void Renderer::drawSceneItem(string name) {
	cam->updateViewMatrix();
	Mat4& view_matrix = cam->vm();
	Mat4& projection_matrix = cam->pm();
	Mat3 normal_matrix = cam->nm();
	
	SceneItem& si = *getSceneItem(name);
	si.draw(view_matrix, projection_matrix, normal_matrix);
}

SceneItem* Renderer::createIcoSphere(string name, int detail, float radius) {
	VertexData* vd = new VertexData(name +"_vertex_data");
	SceneItem* si = new SceneItem(name);

	// Create vertex data for ico sphere.	
	IcoSphere ico_sphere;
	ico_sphere.create(detail, radius, *vd);
	
	// Create scene item from vertex data and make sure shader is set.
	si->setEffect(effect);
	si->createFromVertexData(vd);
	si->setDrawMode(SceneItem::TRIANGLES);
	
	// Keep track of the created data.
	scene->addSceneItem(name, si);
	scene->addVertexData(name, vd);

	return si;
}

SceneItem* Renderer::createUVSphere(string name, int phi, int theta, float radius) {
	VertexData* vd = new VertexData(name +"_vertex_data");
	SceneItem* si = new SceneItem(name);

	// Create vertex data for ico sphere.	
	UVSphere uv_sphere;
	uv_sphere.create(radius, phi, theta, *vd);
	
	// Create scene item from vertex data and make sure shader is set.
	si->setEffect(effect);
	si->createFromVertexData(vd);
	si->setDrawMode(SceneItem::QUAD_STRIP);

	// Keep track of the created data.
	scene->addSceneItem(name, si);
	scene->addVertexData(name, vd);
	return si;
}

SceneItem* Renderer::createBox(string name, float width, float height, float depth) {
	VertexData* vd = new VertexData(name +"_vertex_data");
	SceneItem* si = new SceneItem(name);

	// Create vertex data for ico sphere.	
	Box box;
	box.create(width, height, depth, *vd);
	
	// Create scene item from vertex data and make sure shader is set.
	si->setEffect(effect);
	si->createFromVertexData(vd);
	si->setDrawMode(SceneItem::QUADS);
	
	// Keep track of the created data.
	scene->addSceneItem(name, si);
	scene->addVertexData(name, vd);
	return si;
}

SceneItem* Renderer::createPlane(string name, float width, float height) {
	VertexData* vd = new VertexData(name +"_vertex_data");
	SceneItem* si = new SceneItem(name);

	// Create vertex data for ico sphere.	
	Plane plane;
	plane.create(width, height, *vd);
	
	// Create scene item from vertex data and make sure shader is set.
	si->setEffect(effect);
	si->createFromVertexData(vd);
	si->setDrawMode(SceneItem::QUADS);
	
	// Keep track of the created data.
	scene->addSceneItem(name, si);
	scene->addVertexData(name, vd);
	return si;
}


void Renderer::addSceneItem(string name, SceneItem* si) {
	si->setName(name);
	addSceneItem(si);
}
void Renderer::addSceneItem(SceneItem* si) {
	if(!si->hasEffect()) {
		si->setEffect(effect);
	}
	scene->addSceneItem(si->getName(), si);
	scene->addVertexData(si->getVertexData()->getName(), si->getVertexData());
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
Material* Renderer::createDiffuseTexture(
		 string materialName
		,string textureName
		,string diffuseFileName
		,GLuint imageFormat) 
{
	// @todo create a function: getMaterialOrCreateIfNotExist
	Material* mat = getMaterial(materialName);
	if(mat == NULL) {
		mat =  new Material(materialName);
	}
	
	//effect->enableDiffuseTexture();
	
	Texture* tex = mat->loadDiffuseTexture(diffuseFileName, imageFormat);
	scene->addMaterial(materialName, mat);
	scene->addTexture(textureName, tex);
	return mat;
}


Material* Renderer::createMaterial(string name) {
	Material* m = new Material(name);
	scene->addMaterial(name, m);
	return m;
}

Material* Renderer::createNormalTexture(
	 string materialName
	,string textureName
	,string normalFileName
	,GLuint imageFormat) 
{
	// @todo create a function: getMaterialOrCreateIfNotExist
	Material* mat = getMaterial(materialName);
	if(mat == NULL) {
		mat =  new Material(materialName);
	}
	
	effect->enableNormalTexture();
	
	Texture* tex = mat->loadNormalTexture(normalFileName, imageFormat);
	scene->addMaterial(materialName, mat);
	scene->addTexture(textureName, tex);

	return mat;
}


Light* Renderer::createLight(string name, float r, float g, float b, float a) {
	Light* l = new Light();
	l->setDiffuseColor(r,g,b, a);
	scene->addLight(name, l);
	effect->addLight(l);
	return l;
}

void Renderer::setLightPosition(string name, float x, float y, float z) {
	Light* l = scene->getLight(name);
	l->setPosition(x,y,z);
}

void Renderer::setLightDiffuseColor(string name, float r, float g, float b, float a) {
	Light* l = scene->getLight(name);
	l->setDiffuseColor(r,g,b,a);
}

Light* Renderer::getLight(string name) {
	return scene->getLight(name);
}

void Renderer::setLightSpecularColor(string name, float r, float g, float b, float a) {
	Light* l = scene->getLight(name);
	l->setSpecularColor(r,g,b,a);
}

void Renderer::exportToOBJ(string fileName) {
	printf("export!");
	OBJ obj;
	
	// add scene items.
	const map<string, SceneItem*>& items = scene->getSceneItems();
	map<string, SceneItem*>::const_iterator items_it = items.begin();
	while(items_it != items.end()) {
		obj.addSceneItem(items_it->second);
		++items_it;
	}
	
	// add materials.
	const map<string, Material*>& maps = scene->getMaterials();
	map<string, Material*>::const_iterator map_it = maps.begin();
	while(map_it != maps.end()) {
		obj.addMaterial(map_it->second);
		++map_it;
	}
	
	obj.save("test.obj");
}

void Renderer::exportToR3F(string fileName) {
	R3F rf;
	
	// add vertex datas
	const map<string, VertexData*>& datas = scene->getVertexDatas();
	map<string, VertexData*>::const_iterator it_data = datas.begin();
	while(it_data != datas.end()) {
		rf.addVertexData(it_data->second);
		++it_data;
	}
	
	// add scene items.
	const map<string, SceneItem*>& items = scene->getSceneItems();
	map<string, SceneItem*>::const_iterator items_it = items.begin();
	while(items_it != items.end()) {
		rf.addSceneItem(items_it->second);
		++items_it;
	}
	

	// add materials.
	const map<string, Material*>& maps = scene->getMaterials();
	map<string, Material*>::const_iterator map_it = maps.begin();
	while(map_it != maps.end()) {
		rf.addMaterial(map_it->second);
		++map_it;
	}

	rf.save("test.r3f");	
//	rf.addSceneItem(plane_si);
}


} // roxlu