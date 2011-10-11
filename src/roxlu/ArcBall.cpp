#include "ArcBall.h"

namespace roxlu {

ArcBall::ArcBall()
:center(0,0,0)
,radius(1.0)
,mouse_start(0,0,0)
,mouse_curr(0,0,0)
{
	inv_radius = 1/radius;
}

// debug.
void ArcBall::draw() {
	/*
	
	// @todo: MAKE THIS NON-INTERMEDIATE
	
	glColor3f(1,1,0);
	glBegin(GL_LINES);
		glVertex3fv(&center.x);
		glVertex3fv(&mouse_start.x);
		glVertex3fv(&center.x);
		glVertex3fv(&mouse_curr.x);
	glEnd();
	
	// draw axis.
	glColor3f(0,1,1);
	glBegin(GL_LINES);
		glVertex3fv(&center.x);
		glVertex3fv(&axis.x);
	glEnd();

	glPointSize(10);

	// start 
	glColor3f(1,0,0);
	glBegin(GL_POINTS);
		glVertex3fv(&mouse_start.x);
	glEnd();
	
	// current.
	glColor3f(0,0,1);
	glBegin(GL_POINTS);
		glVertex3fv(&mouse_curr.x);
	glEnd();
	glPointSize(1);
	
	glPointSize(15);
	glColor3f(1,0,0.5);
	glBegin(GL_POINTS);
		glVertex3fv(&mc.x);
	glEnd();
	*/
}

void ArcBall::setCamera(Camera& rCam) {
	camera = &rCam;
}

// good radius: std::min((float)ofGetWidth() * 0.5, (float) ofGetHeight() * 0.5);
void ArcBall::setRadius(float nRadius) {
	radius = nRadius;
	inv_radius = 1/radius;
}

// setCenter(ofGetWidth()*0.5, ofGetHeight() * 0.5) for best results
void ArcBall::setCenter(float nX, float nY) {
	center.x = nX;
	center.y = nY;
	center.z = 0;
}

void ArcBall::onMouseDown(float nX, float nY) {
	mouse_start.set(nX, nY, 0); 
	mouse_start = getSphereIntersection(mouse_start);
	initial_quat = rotation;
}

// borrowed from libcinder
void ArcBall::onMouseDrag(float nX, float nY) {
	mouse_curr.set(nX, nY, 0); 
	mc = mouse_curr;
	mouse_curr = getSphereIntersection(mouse_curr);

	Vec3 axis = mouse_start.getCrossed(mouse_curr);
	axis.y *= -1;
	Quat q;
	q.w = dot(mouse_start,mouse_curr);
	q.v = axis;
	
	rotation = initial_quat * q;
	rotation.normalize();
	camera->rotation = rotation;
}

Vec3 ArcBall::getSphereIntersection(Vec3 oPos) {
	Vec3 to_center;
	to_center.x = (oPos.x - center.x) / (radius * 2);
	to_center.y = (oPos.y - center.y) / (radius * 2);
	to_center.z = 0.0f;
	
	float l = to_center.lengthSquared();
	if(l > 1.0f) {
		to_center.normalize();
	}
	else {
		to_center.z = sqrt(1.0f - l);
		to_center.normalize();
	}
	return to_center;
}

} // roxlu