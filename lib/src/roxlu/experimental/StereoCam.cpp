//#include "StereoCam.h"

#include <roxlu/experimental/StereoCam.h>

StereoCam::StereoCam() 
	:fov(45.0f * DEG_TO_RAD)
	,n(1.0f)
	,f(100.0f)
	,aspect(1.33333f)
	,convergence(1.0f)
	,eye_separation(0.0f)
{
}


void StereoCam::updateProjectionMatrices() {
	float top, bottom, left, right;
	
	// left matrix.
	top = n * tanf(fov*0.5f) ;
	bottom = -top;
	float a = aspect * tanf(fov*0.5f) * convergence;
	float b = a - (eye_separation * 0.5f);
	float c = a + (eye_separation * 0.5f);
	left = -b * (n/convergence);
	right = c * (n/convergence);
	l_projection_matrix.frustum(left, right, bottom, top, n, f);
	
	// right matrix.
	left = -c * (n/convergence);
	right = b * (n/convergence);
	r_projection_matrix.frustum(left, right, bottom, top, n, f);
	
}

void StereoCam::setPosition(const float x, const float y, const float z) {
	position.x = x;
	position.y = y;
	position.z = z;
	updateViewMatrix();	
}

void StereoCam::translate(const float x, const float y, const float z) {
	position.x += x;
	position.y += y;
	position.z += z;
	updateViewMatrix();
}


void StereoCam::updateViewMatrix() {
	Mat4 trans = Mat4::translation(position);
	view_matrix = trans;
	view_matrix.inverse();
}

void StereoCam::placeLeft() {
	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();
	glLoadMatrixf(pml().getPtr());

	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();
	glLoadMatrixf(vm().getPtr());
	
	setupLeft();
}

void StereoCam::setupLeft() {
	glColorMask(true, false, false, false);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
}

void StereoCam::setupRight() {
	glClear(GL_DEPTH_BUFFER_BIT) ;
	glColorMask(false, true, true, false);
}

void StereoCam::placeRight() {
	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();
	glLoadMatrixf(pmr().getPtr());

	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();
	glLoadMatrixf(vm().getPtr());
	
	setupRight();
}
	
	
	/*
	:fov(60.0f)
,aspect(4.0f/3.0f)
,near(1.0f)
,far(600.0f)
,screen_width(0)
,screen_height(0)
,projection_type(PERSPECTIVE)
	*/