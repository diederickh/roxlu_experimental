#ifndef ROXLU_SCENEITEMH
#define ROXLU_SCENEITEMH

#include "Material.h"
#include "VBO.h"
#include "VAO.h"
#include "Shader.h"
#include "Mat3.h"
#include "Mat4.h"
#include "Vec3.h"
#include "Quat.h"
#include "VertexData.h"
#include <string>
#include <vector>


using std::vector;
using std::string;

namespace roxlu {

class Light;
class Effect;

class SceneItem {
public:
	enum SceneItemDrawMode {
		 TRIANGLES 		= GL_TRIANGLES
		,TRIANGLE_STRIP = GL_TRIANGLE_STRIP
		,QUADS 			= GL_QUADS
		,QUAD_STRIP 	= GL_QUAD_STRIP
		,POINTS 		= GL_POINTS
		,LINE_STRIP		= GL_LINE_STRIP
	};

	SceneItem(string name);
	~SceneItem();
	SceneItem* duplicate();
	
	
	void draw(Mat4& viewMatrix, Mat4& projectionMatrix);
	bool createFromVertexData(VertexData* vd);
	bool createFromVertexData(VertexData& vd);
	inline VertexData* getVertexData();
	inline void setPosition(float x, float y, float z);
	inline void setPosition(const Vec3& p);
	inline Vec3 getPosition() const;
	inline void scale(float s);
	inline Vec3& getScale();
	inline void setScale(const Vec3& s);
	inline void translate(float x, float y, float z);
	inline void translate(const Vec3& t);
	inline void rotate(float radians, const float x, const float y, const float z);
	inline void rotateX(float radians);
	inline void rotateY(float radians);
	inline void rotateZ(float radians);
	inline void rotateXYZ(float x, float y, float z);
	inline void setRotation(float x, float y, float z);
	inline void setRotation(float angle, Vec3 axis);
	inline Quat& getOrientation();
	inline void setOrientation(const Quat& q);
	inline void updateModelMatrix();
	inline VBO* getVBOPtr();
	void lookAt(const SceneItem* si);
	void lookAt(const Vec3& pos); // pos is in world coordinates
	Mat3 getLookAtMatrix(const Vec3& pos, const Vec3& upVec = Vec3(0,1,0));
	
	inline void setName(string itemName);
	inline string getName() const;
	
	// shader
	inline bool hasEffect();
	inline void setEffect(Effect& eff);
	inline void setEffect(Effect* eff);
	
	// material
	inline void setMaterial(Material* mat);
	inline Material* getMaterial();
	inline bool hasMaterial();
	
	// draw modes
	inline void	setDrawMode(SceneItemDrawMode mode);
	inline int getDrawMode();
	inline void	drawUsingTriangles();
	inline void drawUsingQuads();
	inline void drawUsingPoints();
	inline void drawUsingTriangleStrip();
	inline void drawUsingQuadStrip();
	inline void drawUsingLineStrip();
	
	// material
	inline void setColor(Color4 col);
	inline void setColor(float r, float g, float b, float a = 1.0);
	inline Color4* getColorPtr();
	inline void setSpecularity(float spec);
	inline float getSpecularity();
	inline void setAttenuation(float ambientAttenuation, float specularAttenuation, float diffuseAttenuation);
	const inline float* getAttenuationPtr();
	
	// matrix related
	inline Mat4& mm(); // get model matrix.
	Mat4 model_matrix; 
	Vec3 position;
	Vec3 scaling;
	Quat orientation;
	VertexData* vertex_data;
			
	VBO* vbo;
	VAO* vao;
	
	//Shader* shader;	
	Material* material;
	void initialize();
	bool isInitialized();
	
	inline void hide();
	inline void show();
	inline bool isVisible();

	Mat3 test_rot_matrix;
	bool test_rot_set;

	
private:
	bool is_visible;
	Effect* effect;
	string name;
	bool initialized;
	int draw_mode;
	
	// material properties
	float specularity;
	Color4 color;
	Vec3 attenuation; // x = ambient, y = diffuse, z = specular
	void drawElements(); // indexed data
	void drawArrays(); 
	

};

// vector<SceneItem*>::iterator it = std::find_if(items.begin(), items.end(), CompareSceneItemByName(name));
class CompareSceneItemByName {
public:
	CompareSceneItemByName(string name):name(name){}
	bool const operator()(const SceneItem* other) const {
		return name == other->getName();
	}
	string name;
};

inline void SceneItem::drawUsingTriangles() {
	setDrawMode(TRIANGLES);
}

inline void SceneItem::drawUsingQuads() {
	setDrawMode(QUADS);
}

inline void SceneItem::drawUsingPoints() {
	setDrawMode(POINTS);
}

inline void SceneItem::drawUsingTriangleStrip() {
	setDrawMode(TRIANGLE_STRIP);
}

inline void SceneItem::drawUsingQuadStrip() {
	setDrawMode(QUAD_STRIP);
}

inline void SceneItem::drawUsingLineStrip() {
	setDrawMode(LINE_STRIP);
}

inline void SceneItem::setDrawMode(SceneItemDrawMode mode) {
	draw_mode = mode;
}

inline int SceneItem::getDrawMode() {
	return draw_mode;
}

inline Mat4& SceneItem::mm() {
	return model_matrix;
}

inline void SceneItem::setPosition(float x, float y, float z) {
	position.set(x,y,z);
	//printf("position.y = %f\n", position.y);	
	updateModelMatrix();
}

inline void SceneItem::setPosition(const Vec3& p) {
	position = p;	
	updateModelMatrix();
}

inline Vec3 SceneItem::getPosition() const {
	return position;
}

inline void SceneItem::translate(float x, float y, float z) {
	position.x += x;
	position.y += y;
	position.z += z;
	updateModelMatrix();
}

inline void SceneItem::translate(const Vec3& v) {
	position += v;
	updateModelMatrix();
}

inline void SceneItem::setEffect(Effect& eff) {
	setEffect(&eff);
}

inline void SceneItem::setEffect(Effect* eff) {
	effect = eff;
}

inline void SceneItem::updateModelMatrix() {
	Mat4 rotation_matrix; 
	Mat4 translation_matrix;
	
	rotation_matrix = orientation.getMat4();
	translation_matrix = Mat4::translation(position);
	
	model_matrix = rotation_matrix * translation_matrix;
//	model_matrix.print();
//	printf("y: %f\n", model_matrix[13]);
	return;

	Mat4 rotation = orientation.getMat4();
	model_matrix = model_matrix.translation(position); 
	model_matrix.scale(scaling.x, scaling.y, scaling.z);
	bool debug = true;
//	debug = !debug;
	if(debug) {
		printf("In SceneItem::updateModelMatrix:\n");
		printf("Rotation matrix:\n");
		rotation.print();
		printf("\nTranslation matrix:\n");
		model_matrix.print();
	}
//	printf("--\n");
//	model_matrix.print();
//	printf("==\n");
	//model_matrix = rotation * model_matrix;
//	model_matrix *= model_matrix;
//	model_matrix.print();
//	model_matrix.translate(position);
//	model_matrix = model_matrix * rotation;

	model_matrix = rotation * model_matrix;
//	model_matrix.m[12] *= -1;
//	model_matrix.m[13] *= -1;
//	model_matrix.m[14] *= -1;
	if(debug) {
		printf("\nFinal model matrix:\n");
		model_matrix.print();	
		printf("-------------\n\n\n");
	}
	
//	
//	printf("++\n");
	//rotation.print();
//	printf("--\n");
}

inline void SceneItem::rotate(float radians, const float x, const float y, const float z) {
	orientation.rotate(radians, x, y, z);
	updateModelMatrix();
}

inline void SceneItem::rotateX(float radians) {
	orientation.rotate(radians,1,0,0);
	updateModelMatrix();
}

inline void SceneItem::rotateY(float radians) {
	orientation.rotate(radians,0,1,0);
	updateModelMatrix();
}

inline void SceneItem::rotateZ(float radians) {
	orientation.rotate(radians,0,0,1);
	updateModelMatrix();
}

inline void SceneItem::setOrientation(const Quat& q) {
	orientation = q;
	updateModelMatrix();
}

inline void SceneItem::rotateXYZ(float x, float y, float z) {
	rotateX(x);
	rotateY(y);
	rotateZ(z);	
}

inline void SceneItem::setRotation(float angle, Vec3 axis) {
	//printf("SET ROTATION: %f, %f, %f\n", axis.x, axis.y, axis.z);
	orientation.setRotation(angle, axis);
	updateModelMatrix();
}

inline VertexData* SceneItem::getVertexData() {
	return vertex_data;
}

inline void SceneItem::setMaterial(Material* mat) {
	material = mat;
}

inline Material* SceneItem::getMaterial() {
	return material;
}

inline bool SceneItem::hasMaterial() {
	return material != NULL;
}

inline void SceneItem::setName(string itemName) {
	name = itemName;
}

inline string SceneItem::getName() const {
	return name;
}

inline void SceneItem::scale(float s) {
	scaling.set(s,s,s);
	updateModelMatrix();
}

inline Vec3& SceneItem::getScale() {
	return scaling;
}

inline void SceneItem::setScale(const Vec3& s) {
	scaling = s;
	updateModelMatrix();
}


inline Quat& SceneItem::getOrientation() {
	return orientation;
}

inline void SceneItem::hide() {
	is_visible = false;
}

inline void SceneItem::show() {
	is_visible = true;
}

inline bool SceneItem::isVisible() {
	return is_visible;
}

inline bool SceneItem::hasEffect() {
	return effect != NULL;
}

inline void SceneItem::setColor(Color4 col) {
	color = col;
}

inline void SceneItem::setColor(float r, float g, float b, float a) {
	color.set(r,g,b,a);
}

inline Color4* SceneItem::getColorPtr() {
	return &color;
}

inline void SceneItem::setSpecularity(float spec) {
	specularity = spec;
}

inline float SceneItem::getSpecularity() {
	return specularity;
}
	
inline void SceneItem::setAttenuation(float ambientAttenuation, float specularAttenuation, float diffuseAttenuation) {
	attenuation.set(ambientAttenuation, specularAttenuation, diffuseAttenuation);
}

inline const float* SceneItem::getAttenuationPtr() {
	return attenuation.getPtr();
}

inline VBO* SceneItem::getVBOPtr() {
	return vbo;
}

inline bool SceneItem::isInitialized() {
	return initialized;
}


} // roxlu

#endif