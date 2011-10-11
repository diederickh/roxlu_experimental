#ifndef ROXLU_CAMERAH
#define ROXLU_CAMERAH

#include "OpenGL.h"
#include "Quat.h"
#include "Mat4.h"
#include "Vec3.h"

namespace roxlu {

class Camera {
public:
	Camera();
	
	enum ProjectionType {
		 PERSPECTIVE
		,ORTHO_CENTER
		,ORTHO_TOP_LEFT
	};
	int projection_type;

	// projection.
	void perspective(float nFov, float nAspect, float nNear, float nFar);
	void ortho(float nWidth, float nHeight, float nNear, float nFar);
	void orthoTopLeft(float nWidth, float nHeight, float nNear, float nFar);
	void setFar(float nFar);
	void setNear(float nNear);
	void setFov(float nFovY);
	void setScreenWidth(float screenWidth);
	void setScreenHeight(float screenHeight);
	inline float getScreenWidth();
	inline float getScreenHeight();
	
	// translating, rotating.
	void place();
	void translate(float nX, float nY, float nZ);
	void rotate(float nDegrees, float nX, float nY, float nZ);
	void clearRotation();
	void setPosition(float nX, float nY, float nZ);

	Vec3 screenToWorld(float nX, float nY, float nZ);
	void setUpVector(float nX, float nY, float nZ);

	// matrices.	
	float* getInverseViewMatrixPtr();
	Mat4 getInverseViewMatrix();
	Mat4 getInverseViewProjectionMatrix();
	Mat4 getViewMatrix();
	Mat4 getViewProjectionMatrix();
	inline Mat4& vm(); // view matrix
	inline Mat4& pm(); // projection matrix

	
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

inline Mat4& Camera::pm() {
	return projection_matrix;
}

} // roxlu
#endif