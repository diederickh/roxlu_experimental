#ifndef ROXLU_PBDH
#define ROXLU_PBDH

#include "Roxlu.h"
#include "Particles.h"
#include "Particle.h"
#include "Flocking.h"

using namespace roxlu;

typedef Particle<Vec2> Particle2;
typedef Particle<Vec3> Particle3;

typedef Particles<Vec2> Particles2;
typedef Particles<Vec3> Particles3;


template<class T>
inline void draw(Particles<T>& ps) {
	glColor3f(1.0f,1.0f,1.0f);	
	glBegin(GL_LINES);
	typename vector<Spring<T>* >::iterator sit = ps.springs.begin();
	while(sit != ps.springs.end()) {
		Spring<T>& s = *(*sit);
		drawVertex(s.a.position);
		drawVertex(s.b.position);
		++sit;
	}
	glEnd();

	
	typename vector<Particle<T>* >::iterator it = ps.particles.begin();
	while(it != ps.particles.end()) {
		Particle<T>& p = *(*it);
		if(!p.enabled) {
			glColor3f(1.0f,0.0f,0.0f);
		}
		else {
			glColor4fv(p.color);
		}
		glPointSize(p.size);
		glBegin(GL_POINTS);
			drawVertex(p.position);
		glEnd();
		++it;
	}	
	glColor3f(1.0f, 1.0f, 1.0f);
}

inline void drawVertex(Vec3 p) {
	glVertex3fv(p.getPtr());
}

inline void drawVertex(Vec2 p) {
	glVertex2fv(p.getPtr());
}





#endif