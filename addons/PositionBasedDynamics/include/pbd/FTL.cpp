#include <glr/GL.h>
#include "FTL.h"

using namespace gl;

namespace ftl {

  Particle::Particle(Vec3 p, float m) 
    :position(p)
    ,tmp_position(p)
    ,enabled(true)
  {

    if(m < 0.001) {
      m = 0.001;
    }

    mass = m;
    inv_mass = 1.0 / mass;
  }

  // ------------------------

  FTL::FTL()
    :len(10)
  {
  }

  void FTL::setup(int num, float d) {
    len = d;
    Vec3 pos(300, 300, 0);
    float mass = rx_random(1.0f, 10.0f);
    for(int i = 0; i < num; ++i) {
      Particle* p = new Particle(pos, mass); 
      particles.push_back(p);
      pos.y += d;
    }

    particles[0]->enabled = false;
  }

  void FTL::addForce(Vec3 f) {
    for(std::vector<Particle*>::iterator it = particles.begin(); it != particles.end(); ++it) {
      Particle* p = *it;
      if(p->enabled) {
        p->forces += f;
      }
    }
  }

  void FTL::update() {
    float dt = 1.0f/20.0f;

    // update velocities
    for(std::vector<Particle*>::iterator it = particles.begin(); it != particles.end(); ++it) {
      Particle* p = *it;
      if(!p->enabled) {
        p->tmp_position = p->position;
        continue;
      }
      p->velocity = p->velocity + dt * (p->forces * p->inv_mass);
      p->tmp_position += (p->velocity * dt);
      p->forces = 0;
      p->velocity *= 0.99;
    }    

    // solve constraints
    Vec3 dir;
    Vec3 curr_pos;
    for(size_t i = 1; i < particles.size(); ++i) {
      Particle* pa = particles[i - 1];
      Particle* pb = particles[i];
      curr_pos = pb->tmp_position;
      dir = pb->tmp_position - pa->tmp_position;
      dir.normalize();
      pb->tmp_position = pa->tmp_position + dir * len;
      pb->d = curr_pos - pb->tmp_position; //  - curr_pos;
    }    

    for(size_t i = 1; i < particles.size(); ++i) {
      Particle* pa = particles[i-1];
      Particle* pb = particles[i];
      if(!pa->enabled) {
        continue;
      }
      pa->velocity = ((pa->tmp_position - pa->position) / dt) + 0.9 *  (pb->d / dt);
      pa->position = pa->tmp_position;
    }

    Particle* last = particles.back();
    last->position = last->tmp_position;

  }

  void FTL::draw() {
    
    glLineWidth(0.1f);
    glr_begin(GL_LINE_STRIP);
    for(std::vector<Particle*>::iterator it = particles.begin(); it != particles.end(); ++it) {
      Particle* p = *it;

      if(!p->enabled) {
        glr_color(217, 41, 41);
      }
      else {
        glr_color(251, 251, 251);
      }

      glr_vertex(p->position);
    }
    glr_end();
  }

}
