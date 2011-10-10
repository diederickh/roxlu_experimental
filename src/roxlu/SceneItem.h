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
	SceneItem();
	~SceneItem();
	void drawArrays();
	bool createFromVertexData(VertexData* vd);
	bool createFromVertexData(VertexData& vd);
	inline void setPosition(float x, float y, float z);
	inline void setPosition(const Vec3& p);
	inline void translate(float x, float y, float z);
	inline void translate(const Vec3& t);
	inline void updateModelMatrix();
	inline void setShader(Shader* sh);
	
	inline Mat4& mm(); // get model matrix.
	Mat4 model_matrix; 
	Vec3 position;
	Quat orientation;
			
	VBO* vbo;
	VAO* vao;
	Shader* shader;	
};


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