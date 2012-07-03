#ifndef ROXLU_STEREO_CAMERAH
#define ROXLU_STEREO_CAMERAH

/*
#include "Constants.h"
#include "Mat4.h"
#include "Vec3.h"
#include "OpenGL.h"
*/
#include <roxlu/core/Constants.h>
#include <roxlu/math/Mat4.h>
#include <roxlu/math/Vec3.h>
#include <roxlu/opengl/OpenGLInit.h>

// http://quiescentspark.blogspot.nl/2011/05/rendering-3d-anaglyph-in-opengl.html

/* Example
if(anaglyphic) {
	scam.view_matrix = cam.view_matrix;
	
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	drawScene(cam.pm(), cam.vm());
	
	
	glColorMask(true, false, false, false);
	drawScene(scam.pml(), scam.vm());
	
	glClear(GL_DEPTH_BUFFER_BIT) ;
	glColorMask(false, true, true, false);
	drawScene(scam.pmr(), scam.vm());
	
	glColorMask(true, true, true, true);
}
*/

using namespace roxlu;

class StereoCam {
public:
	StereoCam();
	void setNear(const float nn);
	void setFar(const float ff);
	void setAspect(const float a);
	void setConvergence(const float c);
	void setEyeSeparation(const float s);
	
	void setupLeft();
	void setupRight(); 
	
	void placeLeft();
	void placeRight();
	
	void setPosition(const float x, const float y, const float z);
	void translate(const float x, const float y, const float z);

	const Mat4& vm();
	const Mat4& pml();
	const Mat4& pmr();
	
	void updateProjectionMatrices();
	void updateViewMatrix();
	
	float aspect;
	float convergence;
	float eye_separation;
	float fov;
	float n; // near (near is a win macro)
	float f; // far plane (plane is a win macro!)
	Vec3 position; // the camera position (10 units on z is "10" not -10)
	Mat4 view_matrix; // view matrix is inverse of i.e. position. So view.translate(0,0,10) is cam.position(0,0,-10)
	Mat4 l_projection_matrix;
	Mat4 r_projection_matrix;
	
};

// P-rojection M-atrix L-eft
inline const Mat4& StereoCam::pml() {
	return l_projection_matrix;
}

inline const Mat4& StereoCam::pmr() {
	return r_projection_matrix;
}

inline void StereoCam::setNear(const float nn) {
	n = nn;
}

inline void StereoCam::setFar(const float ff) {
	f = ff;
}

inline void StereoCam::setAspect(const float a) {
	aspect = a;
}

inline void StereoCam::setConvergence(const float c) {
	convergence = c;
}

inline void StereoCam::setEyeSeparation(const float s) {
	eye_separation = s;
}

inline const Mat4& StereoCam::vm() {
	return view_matrix;
}


#endif