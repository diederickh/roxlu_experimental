#ifndef ROXLU_PBDH
#define ROXLU_PBDH

#include <roxlu/Roxlu.h>
#include <pbd/Particles.h>
#include <pbd/Particle.h>
#include <pbd/Flocking.h>
#include <pbd/Cloth.h>
#include <pbd/Emitter.h>
#include <pbd/Hair.h>

using namespace roxlu;

typedef Particle<Vec2> Particle2;
typedef Particle<Vec3> Particle3;
typedef Flocking<Vec2, Particle<Vec2>, FlockingForceAdder<Vec2, Particle<Vec2> > > Flocking2;
typedef Spring<Vec2> Spring2;

typedef Particles<Vec2, Particle<Vec2>, Spring<Vec2> > Particles2;
typedef Particles<Vec3, Particle<Vec3>, Spring<Vec3> > Particles3;
typedef Flocking<Vec3, Particle<Vec3>, FlockingForceAdder<Vec3, Particle<Vec3> > > Flocking3;
typedef Spring<Vec3> Spring3;

typedef Emitter<Particle2, Particles2, Vec2, EmitterHelper> Emitter2;
typedef Emitter<Particle3, Particles3, Vec3, EmitterHelper3> Emitter3;

typedef Hair<Vec2> Hair2;
typedef Hair<Vec3> Hair3;

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
