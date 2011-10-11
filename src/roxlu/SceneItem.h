#ifndef ROXLU_SCENEITEMH
#define ROXLU_SCENEITEMH

#include "VBO.h"
#include "VAO.h"
#include "Shader.h"
#include "Mat4.h"
#include "Vec3.h"
#include "Quat.h"
#include "VertexData.h"

namespace roxlu {

class SceneItem {
public:
	enum SceneItemDrawMode {
		 TRIANGLES = GL_TRIANGLES
		,QUADS = GL_QUADS
		,POINTS = GL_POINTS
	};

	SceneItem();
	~SceneItem();
	void draw();
	bool createFromVertexData(VertexData* vd);
	bool createFromVertexData(VertexData& vd);
	inline void setPosition(float x, float y, float z);
	inline void setPosition(const Vec3& p);
	inline void translate(float x, float y, float z);
	inline void translate(const Vec3& t);
	inline void updateModelMatrix();
	inline void setShader(Shader* sh);
	
	// draw modes
	inline void	setDrawMode(SceneItemDrawMode mode);
	inline void	drawUsingTriangles();
	inline void drawUsingQuads();
	inline void drawUsingPoints();
	
	// matrix related
	inline Mat4& mm(); // get model matrix.
	Mat4 model_matrix; 
	Vec3 position;
	Quat orientation;
	VertexData* vertex_data;
			
	VBO* vbo;
	VAO* vao;
	Shader* shader;	
private:
	int draw_mode;
	void debugDraw();
	void drawElements(); // indexed data
	void drawArrays(); 
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

inline void SceneItem::setDrawMode(SceneItemDrawMode mode) {
	draw_mode = mode;
}

inline Mat4& SceneItem::mm() {

	return model_matrix;
}

inline void SceneItem::setPosition(float x, float y, float z) {
	position.set(x,y,z);
	updateModelMatrix();
}

inline void SceneItem::setPosition(const Vec3& p) {
	position = p;
	updateModelMatrix();
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

inline void SceneItem::setShader(Shader* sh) {
	shader = sh;
}

inline void SceneItem::updateModelMatrix() {
	model_matrix.identity();
	model_matrix.translate(position);
	Mat4 rotation = orientation.getMat4();
	model_matrix *= rotation;
}

} // roxlu

#endif