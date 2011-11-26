#include "OpenGL.h"
#include "Light.h"

namespace roxlu {

Light::Light() 
	:is_enabled(true)
{
}

void Light::setAmbientColor(float nR, float nG, float nB, float nA) {
	ambient_col.set(nR,nG,nB,nA);
}

void Light::setDiffuseColor(float nR, float nG, float nB, float nA) {
	diffuse_col.set(nR,nG,nB,nA);
}

void Light::setSpecularColor(float nR, float nG, float nB, float nA) {
	specular_col.set(nR,nG,nB,nA);
}

void Light::setPosition(float nX, float nY, float nZ) {
	position.x = nX;
	position.y = nY;
	position.z = nZ;
}

void Light::debugDraw() {
	Vec3 to_center(-position.x, -position.y, -position.z);
	Vec3 help_axis(0,0,1);
	if(to_center.x == 0 && to_center.y == 0) {
		help_axis.x = 1;
		help_axis.z = 0;
	}
	
	// create plane.
	float length;
	Vec3 crossa, crossb;
	normalize3(to_center, length, to_center);
	cross3(to_center, help_axis, crossa);
	cross3(to_center, crossa, crossb);
	multiply3(crossa, 1.5, crossa);
	multiply3(crossb, 1.5, crossb);
		
	Vec3 a = position + crossa;
	Vec3 b = position - crossa;
	Vec3 c = position + crossb;
	Vec3 d = position - crossb;
	
	glBegin(GL_LINES);
		glVertex3fv(&a.x);
		glVertex3fv(&b.x);
		glVertex3fv(&c.x);
		glVertex3fv(&d.x);
	glEnd();
	
	glLineWidth(1.0);
	glColor3f(diffuse_col.r, diffuse_col.g, diffuse_col.b);
	glBegin(GL_LINE_LOOP);	
		glVertex3fv(&a.x);
		glVertex3fv(&c.x);
		glVertex3fv(&b.x);
		glVertex3fv(&d.x);
	glEnd();
	
	Vec3 to_center_dir = position+(to_center*0.4f);
	glLineWidth(2.0);
	glBegin(GL_LINES);
		glVertex3fv(&position.x);
		glVertex3fv(&to_center_dir.x);
	glEnd();
	glLineWidth(1.0);
	
	
	glColor3f(1.0, 1.0, 1.0);
}

void Light::enable() {
	is_enabled = true;
}

void Light::disable() {
	is_enabled = false;
}

}; // roxlu
