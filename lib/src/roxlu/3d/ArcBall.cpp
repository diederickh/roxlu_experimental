#include <roxlu/3d/ArcBall.h>

namespace roxlu {

ArcBall::ArcBall()
:mouse_start(0,0,0)
,mouse_curr(0,0,0)
{
}


void ArcBall::setScreenSize(float w, float h) {
	screen_width = w;
	screen_height = h;
}

void ArcBall::setCamera(Camera& rCam) {
	camera = &rCam;
}

void ArcBall::onMouseDown(float nX, float nY) {
	mouse_start.set(nX, nY, 0); 
	mouse_start = getSphereIntersection(mouse_start);
	initial_quat = rotation;
}

void ArcBall::onMouseDrag(float nX, float nY) {
	mouse_curr.set(nX, nY, 0); 
	mouse_curr = getSphereIntersection(mouse_curr);
	axis = cross(mouse_start, mouse_curr);
		
	Quat q;
	q.w = dot(mouse_start,mouse_curr); // cos(a)
	q.x = axis.x;
	q.y = axis.y;
	q.z = axis.z;

	rotation = q * initial_quat;
	camera->setRotation(rotation);
}

Vec3 ArcBall::getSphereIntersection(Vec3 oPos) {
	// We get the coordinate on the sphere (actually we only use one side of 
	// the sphere which is in the possitive Z part of the scene. We map the 
	// x and y screen coordinates onto a sphere at the center of the screen
	// and with a radius of 1.
	Vec3 to_center(0,0,0);
	to_center.x = -1.0 + ((oPos.x/screen_width)) * 2.0f;
	to_center.y = -(-1.0 + (oPos.y/screen_height) * 2.0f);

	// normalize 
	float l = to_center.x * to_center.x + to_center.y * to_center.y;
	if(l > 1.0f) {
		float s = 1.0f / to_center.length();
		to_center.x *= s;
		to_center.y *= s;
		to_center.z = 0.0f;
	}
	else {
		// when not on outer edge get z
		to_center.z = sqrt(1.0f - l);
	}
	return to_center;
}

} // roxlu