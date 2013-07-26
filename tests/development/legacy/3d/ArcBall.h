#ifndef ROXLU_ARCBALLH
#define ROXLU_ARCBALLH

#include <roxlu/opengl/GL.h>
#include <roxlu/math/Quat.h>
#include <roxlu/3d/Camera.h>

namespace roxlu {

/**
 * Based on:
 * ARCBALL: A User Interface for Specifying Three-Dimensional Oriantation
 * Using a Mouse
 *
 * Only difference is that we use a predefined radius of 1. The mouse coordinates
 * are mapped to the positive values of the z-axis. When the user clicks
 * totally left on the window the mapped x-position (mapped onto the sphere)
 * is -1.0f for the x values. When pressed on the right of the window the x maps
 * to 1.0f
 *
 * 
 */
class ArcBall {
public:
	ArcBall();
	void setCamera(Camera& rCam);
	void setScreenSize(float w, float h);
	void onMouseDown(float nX, float nY);
	void onMouseDrag(float nX, float nY);
	Quat rotation; 
	Quat initial_quat;

protected:
	float screen_width;
	float screen_height;
	Vec3 	getSphereIntersection(Vec3 oPos);
	Vec3 	mouse_start;
	Vec3 	mouse_curr;
	Vec3 	axis;
	Camera* camera;
};

} // roxlu
#endif
