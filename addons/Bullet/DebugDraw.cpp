#include "DebugDraw.h"
#include "OpenGL.h"

namespace roxlu {
namespace bullet {

DebugDraw::DebugDraw()
	:debug_mode(0)
{
}

void DebugDraw::drawLine(const btVector3& from,const btVector3& to,const btVector3& fromColor, const btVector3& toColor) {
	glBegin(GL_LINES);
		glColor3f(fromColor.x(), fromColor.y(), fromColor.z());
		glVertex3d(from.x(), from.y(), from.z());
		glColor3f(toColor.x(), toColor.y(), toColor.z());
		glVertex3d(to.x(), to.y(), to.z());
	glEnd();
}

void DebugDraw::drawLine(const btVector3& from,const btVector3& to,const btVector3& color) {
	drawLine(from, to, color, color);
}

void DebugDraw::drawSphere(const btVector3& p, btScalar radius, const btVector3& color) {

	glColor4f(color.x(), color.y(), color.z(), btScalar(1.0f));
	glPushMatrix();
		glTranslatef(p.x(), p.y(), p.z());

		int lats = 5;
		int longs = 5;

		int i, j;
		for(i = 0; i <= lats; i++) {
			btScalar lat0 = SIMD_PI * (-btScalar(0.5) + (btScalar) (i - 1) / lats);
			btScalar z0 = radius*sin(lat0);
			btScalar zr0 = radius*cos(lat0);

			btScalar lat1 = SIMD_PI * (-btScalar(0.5) + (btScalar) i / lats);
			btScalar z1 = radius*sin(lat1);
			btScalar zr1 = radius*cos(lat1);

			glBegin(GL_QUAD_STRIP);
			for(j = 0; j <= longs; j++) {
				btScalar lng = 2 * SIMD_PI * (btScalar) (j - 1) / longs;
				btScalar x = cos(lng);
				btScalar y = sin(lng);

				glNormal3f(x * zr0, y * zr0, z0);
				glVertex3f(x * zr0, y * zr0, z0);
				glNormal3f(x * zr1, y * zr1, z1);
				glVertex3f(x * zr1, y * zr1, z1);
			}
			glEnd();
		}
	glPopMatrix();
}

void DebugDraw::drawBox(
		 const btVector3& boxMin
		,const btVector3& boxMax
		,const btVector3& color
		,btScalar alpha
)
{
	printf("Buller::DebugDraw::drawBox not implemented!\n");
//	btVector3 center = (boxMax + boxMin) * btScalar(0.5f);
//	//glEnable(GL_BLEND);     // Turn blending On
//	//glBlendFunc(GL_SRC_ALPHA, GL_ONE);
//	glColor4f (color.x(), color.y(), color.z(), alpha);
//	glPushMatrix ();
//	glTranslatef (center.x(), center.y(), center.z());
//	glScaled(2*halfExtent[0], 2*halfExtent[1], 2*halfExtent[2]);
////	glutSolidCube(1.0);
//	glPopMatrix ();
//	//glDisable(GL_BLEND);
}


void DebugDraw::drawTriangle(
			 const btVector3& a
			,const btVector3& b
			,const btVector3& c
			,const btVector3& color
			,btScalar alpha
)
{
	const btVector3	n = btCross(b-a,c-a).normalized();
	glBegin(GL_TRIANGLES);		
		glColor4f(color.x(), color.y(), color.z(),alpha);
		glNormal3d(n.x(),n.y(),n.z());
		glVertex3d(a.x(),a.y(),a.z());
		glVertex3d(b.x(),b.y(),b.z());
		glVertex3d(c.x(),c.y(),c.z());
	glEnd();
}

void DebugDraw::setDebugMode(int mode) {
	debug_mode = mode;
}

void DebugDraw::draw3dText(const btVector3& location,const char* textString) {
	printf("Bullet::DebugDraw::draw3dText not implemented.\n");
}

void DebugDraw::reportErrorWarning(const char* warningString) {
	printf("Bullet: %s\n", warningString);
}

void  DebugDraw::drawContactPoint(
			 const btVector3& pointOnB
			,const btVector3& normalOnB
			,btScalar distance
			,int lifeTime
			,const btVector3& color
)
{
	btVector3 to = pointOnB + normalOnB * 1; //distance;
	const btVector3&from = pointOnB;
	glColor4f(color.x(), color.y(), color.z(),1.f);
	glBegin(GL_LINES);
		glVertex3d(from.x(), from.y(), from.z());
		glVertex3d(to.x(), to.y(), to.z());
	glEnd();
}




}} // roxlu::bullet