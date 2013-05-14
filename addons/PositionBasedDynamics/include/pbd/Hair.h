#ifndef ROXLU_HAIR_H
#define ROXLU_HAIR_H

#include <roxlu/Roxlu.h>
#include <pbd/Particle.h>


template<class T>
class Hair {
 public:
  Hair();
  void setup(T pos, int num, float d);
  void addForce(T f);
  void update(float dt = 0.016);
  void draw();
 public:
  float len;
  std::vector<Particle<T>* > particles;
  Vec3 color;
};

template<class T>
Hair<T>::Hair() 
:len(10.0f)
{
}
  
template<class T>
void Hair<T>::setup(T pos, int num, float d) {
  len = d;

  float mass = rx_random(1.0f, 10.0f);
  for(int i = 0; i < num; ++i) {
    Particle<T>* p = new Particle<T>(pos, mass);
    p->tmp_position = p->position;
    particles.push_back(p);
    pos.y += len;
  }

  particles[0]->enabled = false;
}

template<class T>
void Hair<T>::addForce(T f) {
  for(typename std::vector<Particle<T>* >::iterator it = particles.begin(); it != particles.end(); ++it) {
    Particle<T>* p = *it;
    if(p->enabled) {
      p->forces += f;
    }
  }
}

template<class T>
void Hair<T>::update(float dt) {

  // update velocities
  for(typename std::vector<Particle<T>* >::iterator it = particles.begin(); it != particles.end(); ++it) {
    Particle<T>* p = *it;
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
  T dir;
  T curr_pos;
  for(size_t i = 1; i < particles.size(); ++i) {
    Particle<T>* pa = particles[i - 1];
    Particle<T>* pb = particles[i];
    curr_pos = pb->tmp_position;
      
    dir = pb->tmp_position - pa->tmp_position;
    dir.normalize();

    pb->tmp_position = pa->tmp_position + dir * len;
    pb->d = curr_pos - pb->tmp_position;
  }    

  for(size_t i = 1; i < particles.size(); ++i) {
    Particle<T>* pa = particles[i-1];
    Particle<T>* pb = particles[i];
    if(!pa->enabled) {
      continue;
    }
    pa->velocity = ((pa->tmp_position - pa->position) / dt) + 0.9 *  (pb->d / dt);
    pa->position = pa->tmp_position;
  }

  Particle<T>* last = particles.back();
  last->position = last->tmp_position;

}

#endif
