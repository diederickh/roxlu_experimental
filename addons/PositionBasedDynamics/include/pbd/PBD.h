#ifndef ROXLU_PBDH
#define ROXLU_PBDH

/*
#include "Roxlu.h"
#include "Particles.h"
#include "Particle.h"
#include "Flocking.h"
#include "Cloth.h"
*/

#include <roxlu/Roxlu.h>
#include <pbd/Particles.h>
#include <pbd/Particle.h>
#include <pbd/Flocking.h>
#include <pbd/Cloth.h>

using namespace roxlu;

typedef Particle<Vec2> Particle2;
typedef Particle<Vec3> Particle3;
typedef Flocking<Vec2, Particle<Vec2>, FlockingForceAdder<Vec2, Particle<Vec2> > > Flocking2;

typedef Particles<Vec2, Particle<Vec2>, Spring<Vec2> > Particles2;
typedef Particles<Vec3, Particle<Vec3>, Spring<Vec3> > Particles3;
typedef Flocking<Vec3, Particle<Vec3>, FlockingForceAdder<Vec3, Particle<Vec3> > > Flocking3;


inline void drawVertex(Vec3 p) {
	glVertex3fv(p.getPtr());
}

inline void drawVertex(Vec2 p) {	
	glVertex2fv(p.getPtr());
}

template<class T, class P, class S>
inline void draw(Particles<T, P, S>& ps) {
	glBindTexture(GL_TEXTURE_2D, 0);
	
	glColor3f(1.0f,1.0f,1.0f);	
	glBegin(GL_LINES);
	typename vector<S* >::iterator sit = ps.springs.begin();
	while(sit != ps.springs.end()) {
		S& s = *(*sit);
		drawVertex(s.a.position);
		drawVertex(s.b.position);
		++sit;
	}
	glEnd();

	
	typename vector<P* >::iterator it = ps.particles.begin();
	while(it != ps.particles.end()) {
		P& p = *(*it);
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







#endif