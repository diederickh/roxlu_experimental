#ifndef ROXLU_CAMERAH
#define ROXLU_CAMERAH

#include "OpenGL.h"
#include "Quat.h"
#include "Mat4.h"
#include "Mat3.h"
#include "Vec3.h"

namespace roxlu {

class Camera {
public:
	Camera();
	
	enum ProjectionType {
		 PERSPECTIVE
		,ORTHO_CENTER
		,ORTHO_TOP_LEFT
		,ORTHO_BOTTOM_LEFT
	};
	int projection_type;

	// projection.
	void perspective(float nFov, float nAspect, float nNear, float nFar);
	void ortho(float nWidth, float nHeight, float nNear, float nFar);
	void orthoTopLeft(float nWidth, float nHeight, float nNear, float nFar);
	void orthoBottomLeft(float nWidth, float nHeight, float nNear, float nFar);
	void setFar(float nFar);
	void setNear(float nNear);
	void setFov(float nFovY);
	void setScreenWidth(float screenWidth);
	void setScreenHeight(float screenHeight);
	float getScreenWidth();
	float getScreenHeight();
	
	// translating, rotating.
	void place();
	void translate(float nX, float nY, float nZ);
	void rotate(float nDegrees, float nX, float nY, float nZ);
	void setRotation(const Quat& rot);
	void clearRotation();
	void setPosition(float nX, float nY, float nZ);
	Vec3& getPositionRef();
	Vec3 getPosition();
	Vec3 getWorldPosition();

	Vec3 screenToWorld(float nX, float nY, float nZ);
	void setUpVector(float nX, float nY, float nZ);
	void getBillboardVectors(Vec3& right, Vec3& up) const;

	// matrices.	
	float* getInverseViewMatrixPtr();
	Mat4 getInverseViewMatrix();
	Mat4 getInverseViewProjectionMatrix();
	Mat4 getViewMatrix();
	Mat4 getViewProjectionMatrix();

	Mat4& vm(); // view matrix
	Mat4& pm(); // projection matrix
	Mat3 nm(); // normal matrix
	const Mat4& vm() const;
	const Mat4& pm() const;
	const Mat3 nm() const;
	
	// internally used for caching
	void updateViewMatrix();
	void updateProjectionMatrix();
	
	// members
	Quat rotation;
	Mat4 projection_matrix;
	Mat4 view_matrix;
	
	Vec3 up;
	Vec3 position;

	float fov;
	float near;
	float far;
	float aspect;
	float fov_tan;
	float near_width;
	float near_height;
	
	float ortho_width;
	float ortho_height;
	float screen_width;
	float screen_height;
	
};

// @todo: maybe we need a "Renderer" class which holds this information.. 
// or a renderer plus a surface. A renderer then renders to a surface... 
inline float Camera::getScreenWidth() {
	return screen_width;		
}

inline float Camera::getScreenHeight() {
	return screen_height;
}

inline Mat4& Camera::vm() {
	return view_matrix;
}

inline const Mat4& Camera::vm() const {
	return view_matrix;
}

inline Mat4& Camera::pm() {
	return projection_matrix;
}

inline const Mat4& Camera::pm() const {
	return projection_matrix;
}

inline Mat3 Camera::nm() {
	return rotation.getMat3();
}

inline const Mat3 Camera::nm() const {
	return rotation.getMat3();
}

inline Vec3& Camera::getPositionRef() {
	return position;
}

inline Vec3 Camera::getPosition() {
	return position;
}

inline Vec3 Camera::getWorldPosition() {
	Vec3 p = rotation.rotate(position * -1);	
	return p;
}

inline void Camera::setRotation(const Quat& rot) {
	rotation = rot;
	updateViewMatrix();
}



} // roxlu
#endif