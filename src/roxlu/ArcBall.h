#ifndef ROXLU_ARCBALLH
#define ROXLU_ARCBALLH
#include "OpenGL.h"
#include "Camera.h"
#include "Quat.h"

namespace roxlu {

class ArcBall {
public:
	ArcBall();
	void draw();
	void setCamera(Camera& rCam);
	void setRadius(float nRadius);
	void setCenter(float nX, float nY);
	void onMouseDown(float nX, float nY);
	void onMouseDrag(float nX, float nY);
	Quat rotation; 
	Quat initial_quat;
	inline Vec3 getCenter();
protected:
	Vec3 	getSphereIntersection(Vec3 oPos);
	Vec3 	center;
	Vec3 	mouse_start;
	Vec3 	mouse_curr;
	Vec3 	mc; // only for debug.
	Vec3 	axis;
	float 	radius;
	float 	inv_radius;
	Camera* camera;
};

inline Vec3 ArcBall::getCenter() {
	return center;
}

} // roxlu
#endif