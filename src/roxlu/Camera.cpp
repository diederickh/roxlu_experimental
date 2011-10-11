#include "Camera.h"
#include "Constants.h"

namespace roxlu {

Camera::Camera() 
:fov(45.0f)
,aspect(4.0f/3.0f)
,near(0.1f)
,far(1000.0f)
,screen_width(0)
,screen_height(0)
,projection_type(PERSPECTIVE)
{
	
	updateProjectionMatrix();
	
}

// Projection
// -----------------------------------------------------------------------------
void Camera::setFar(float nFar) {
	far = nFar;
	updateProjectionMatrix();
}

void Camera::setNear(float nNear) {
	near = nNear;
	updateProjectionMatrix();
}

void Camera::setFov(float nFovY) {
	fov = nFovY;
	updateProjectionMatrix();
}

void Camera::setUpVector(float nX, float nY, float nZ) {
	up.x = nX;
	up.y = nY;
	up.z = nZ;
}

void Camera::perspective(float nFov, float nAspect, float nNear, float nFar) {
	fov = nFov;
	aspect = nAspect;
	near = nNear;
	far = nFar;
	projection_type = PERSPECTIVE;
	updateProjectionMatrix();
}

void Camera::ortho(float nWidth, float nHeight, float nNear, float nFar) {
	ortho_width = nWidth;
	ortho_height = nHeight;
	near = nNear;
	far = nFar;
	projection_type = ORTHO_CENTER;
	updateProjectionMatrix();
}


void Camera::orthoTopLeft(float nWidth, float nHeight, float nNear, float nFar) {
	ortho_width = nWidth;
	ortho_height = nHeight;
	near = nNear;
	far = nFar;
	projection_type = ORTHO_TOP_LEFT;
	updateProjectionMatrix();
}

// Moving around.
// -----------------------------------------------------------------------------
void Camera::place() {
	glMatrixMode(GL_PROJECTION);
	glLoadMatrixf(projection_matrix.getPtr());

	glMatrixMode(GL_MODELVIEW);
	updateViewMatrix();
	glLoadMatrixf(view_matrix.getPtr());
}

void Camera::translate(float nX, float nY, float nZ) {
	position.x += nX;
	position.y += nY;
	position.z += nZ;
}

void Camera::setPosition(float nX, float nY, float nZ) {
	position.x = nX;
	position.y = nY;
	position.z = nZ;
}

void Camera::rotate(float nDegrees, float nX, float nY, float nZ) {
	rotation.rotate(nDegrees, nX, nY, nZ);
}

void Camera::clearRotation() {
	printf("TODO Camera.clearRotation!!\n");
}


// Updating the matrices.
// -----------------------------------------------------------------------------
void Camera::updateViewMatrix() {
	view_matrix.identity();
	view_matrix.translate(position);
	view_matrix *= rotation.getMat4();
}

// TODO: check this great tutorial: http://www.vb6.us/tutorials/using-mouse-click-3d-space-directx-8
void Camera::updateProjectionMatrix() {
	switch(projection_type) {
		case PERSPECTIVE: {
			fov_tan = tanf(fov * DEG_TO_RAD * 0.5);
			near_width = (fov_tan * near) * 2; 
			near_height = near_width / aspect;
			//projection_matrix = Mat4::perspective(fov, aspect, near, far);
			projection_matrix.perspective(fov, aspect, near, far);
			break;
		}
		case ORTHO_CENTER: {
			//projection_matrix = Mat4::orthoCenter(ortho_width, ortho_height, near, far);
			projection_matrix.orthoCenter(ortho_width, ortho_height, near,far);
			break;
		}
		case ORTHO_TOP_LEFT: {
			//projection_matrix = Mat4::orthoTopLeft(ortho_width, ortho_height, near, far);
			projection_matrix.orthoTopLeft(ortho_width, ortho_height, near, far);
			break;
		}
	}
}

// Utils
// Some links: 
// ***** http://www.songho.ca/opengl/gl_transform.html 
// **  http://www.vb6.us/tutorials/using-mouse-click-3d-space-directx-8 
// *** http://www.mvps.org/directx/articles/rayproj.htm
// -------------------------------------------------------------------------
Vec3 Camera::screenToWorld(float nX, float nY, float nZ) {
	float mouse_x = nX;
	float mouse_y = nY;

	// @todo
	printf("ERROR in CAMERA: ofGetWidth(), ofGetHeight() cannot be used! pass as param!");
	float screen_w = 100;
	float screen_h = 200;
	
//	float screen_w = ofGetWidth();
//	float screen_h = ofGetHeight();
	float aspect = screen_w/screen_h;
	
	float ndx = -1.0f + (mouse_x/screen_w) * 2.0f; // left: -1, right: 1
	float ndy = (1.0f - (mouse_y/(screen_h * 0.5))); // top: -1, bottom: 1
	float ndz = 2.0*nZ-1.0;
	//ndz = nZ;
	updateViewMatrix(); 
	updateProjectionMatrix();

	Vec4 ndc(ndx, ndy, ndz, 1.0);
//	cout << "ndc cam:" << ndc << endl;
//	Mat4 mvp = mvm() * pm() ;
	Mat4 mvp = pm() * vm(); 
	mvp.inverse();
	ndc = mvp * ndc;
		
//	cout << "out cam:" << ndc << endl;
//	ndc = affine_inverse(mvp) * ndc;
//	cout << "@@@@@@@@@@@@@@@@@ cam @@@@@@@@@@@@@@@@@@@@@\n"; 
	//Mat4 inv = affine_inverse(mvp);
	//cout << inv;
//	cout << mvp ;
//	cout << "######################################\n\n"; 
	ndc.w = 1.0f / ndc.w;
	Vec3 r(ndc.x * ndc.w, ndc.y * ndc.w, ndc.z * ndc.w);
	
	return r;
}

/*

//	cout << "pm:\n--------------\n";
//	cout << pm() <<endl << endl;
//	cout << "mvm:\n--------------\n";
//	cout << mvm() <<endl << endl;
//	cout << "mvp:\n--------------\n";
//	cout << mvp <<endl << endl;
//	cout << "+++++++++++++++++" << endl << endl;
	//ndc = mvp * ndc;




	// http://www.songho.ca/opengl/gl_transform.html
	float nx = (((float)nX/ofGetWidth()) * 2) - 1;
	float ny = 1.0 - ((float)nY/ofGetHeight() * 2);
	float half_fov = tan(fov * 0.5 * DEG_TO_RAD);
	float opp_h = half_fov * near;
	float opp_w = opp_h * aspect;
	float near_plane_x = nx * opp_w;
	float near_plane_y = ny * opp_h;
	
	Vec3 p1_n(near_plane_x, near_plane_y, nZ);
	p1_n = rotation.rotate(p1_n);
	return p1_n;
	 
	ndx = tanf(DEG_TO_RAD * fov * 0.5f) * ndx;
	ndy = tanf(DEG_TO_RAD * fov * 0.5f) * ndy;
	
	Vec3 p1_n(ndx*near, ndy*near, near);
	///Vec3 p1_n(near_plane_x, near_plane_y, nZ);
	p1_n = rotation.rotate(p1_n);
	//Vec3 ray_end(ndx*cam.far, ndy * cam.far, cam.far);
	*/
//	Mat4 inverse_projection = affine_inverse(pm());
//	Mat4 inverse_modelview = affine_inverse(mvm());

//	return p1_n;


	
void Camera::setScreenWidth(float screenWidth) {
	screen_width = screenWidth;
}

void Camera::setScreenHeight(float screenHeight) {
	screen_height = screenHeight;
}

Mat4 Camera::getViewMatrix() {
	return view_matrix;
}

Mat4 Camera::getViewProjectionMatrix() {
	return view_matrix * projection_matrix;
}

float* Camera::getInverseViewMatrixPtr() {
	printf("TODO: Camera::getInverseModelViewMatrixPtr\n");
	return NULL;
}

Mat4 Camera::getInverseViewMatrix() {
	updateViewMatrix();
	return affine_inverse(view_matrix);
}

Mat4 Camera::getInverseViewProjectionMatrix() {
	updateViewMatrix();
	updateProjectionMatrix();
	Mat4 mvp = projection_matrix * view_matrix;
	return affine_inverse(mvp);
}

} // roxlu