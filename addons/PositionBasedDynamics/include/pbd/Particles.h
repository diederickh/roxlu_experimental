#ifndef ROXLU_PBD_PARTICLESH
#define ROXLU_PBD_PARTICLESH

#include <pbd/Particle.h>
#include <pbd/Spring.h>

//#include "Particle.h"
//#include "Spring.h"

enum ParticleCallbacks {
  CB_REPEL
  ,CB_REPEL_PARTICLE
  ,CB_ATTRACT_PARTICLE
};


// C A L L B A C K    F U N C T O R 
// +++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
template<class T, class P>
  struct NOP {
    void operator()(P& a, P& b, T& dir, const float& f, const float& lengthSQ, int type) const {}
  };


// P A R T I C L E S
// +++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
template<class T, class P, class S>
  class Particles {
 public:
  typedef typename std::vector<P*>::iterator iterator;
  typedef T Vec;
	
  Particles();
  ~Particles();
	
  P* createParticle(const T& pos, float mass = 1.0f);
  P* addParticle(const T& pos, float mass = 1.0f);
  P* addParticle(P* p);
	
  S* createSpring(P* a, P* b);
  S* addSpring(S* s);
		
  void addForce(const T& f); 
	
  // attract to another particle
  template<class F>
  void attract(P* p, const float radius, const float energy, F& cb);
  void attract(P* p, const float radius, const float energy);
  void attract(const Vec3& pos, const float radius, const float energy);
	
  // repel from each other, with callback when repelled
  template<class F>
  void repel(const float f, F& cb);
  void repel(const float f);
	
  // repel from particle, with callback when repelled
  template<class F>
  void repel(P* p, const float radius, const float energy, F& cb);
  void repel(P* p, const float radius, const float energy);
	
  void limitSpeed(const float speed);
	
  void update(const float dt);
  void removeDeadParticles();
	
  void clear(); 
	
  typename vector<P*>::iterator begin();
  typename vector<P*>::iterator end();
	
  P* operator[](const unsigned int& dx);
  size_t size();
		
  vector<S*> springs;
  vector<P*> particles;
  float drag;
};


template<class T, class P, class S>
  Particles<T,P,S>::Particles() 
  :drag(0.99f)
  {
  }

template<class T, class P, class S>
  Particles<T, P, S>::~Particles() {
  for(typename vector<P* >::iterator it = particles.begin(); it != particles.end(); ++it) {
    delete (*it);
  }
}

template<class T, class P, class S>
  inline typename vector<P*>::iterator Particles<T,P,S>::begin() {
  return particles.begin();
}

template<class T, class P, class S>
  inline typename vector<P*>::iterator Particles<T,P,S>::end() {
  return particles.end();
}

template<class T, class P, class S>
  inline size_t Particles<T, P, S>::size() {
  return particles.size();
}

template<class T, class P, class S>
  P* Particles<T,P,S>::operator[](const unsigned int& dx) {
  return particles[dx];
}

template<class T, class P, class S>
  inline P* Particles<T, P, S>::addParticle(const T& pos, float mass) {
  P* p = createParticle(pos, mass);
  return addParticle(p);
}

template<class T, class P, class S>
  inline P* Particles<T, P, S>::createParticle(const T& pos, float mass) {
  P* p = new P(pos, mass);
  return p;
}

template<class T, class P, class S>
  inline P* Particles<T, P, S>::addParticle(P* p) {
  p->dx = particles.size();
  particles.push_back(p);
  return p;
}

template<class T, class P, class S>
  void Particles<T, P, S>::update(const float dt) {
  float fps = 1.0f/dt;
	
  // PREDICT NEW LOCATIONS
  typename vector<P*>::iterator it = particles.begin();
  while(it != particles.end()) {
    P& p = *(*it);
		
    if(!p.enabled) {
      p.tmp_position = p.position;
      ++it;
      continue;
    }
    p.velocity = p.velocity + (dt * p.forces);
    p.velocity *= drag; 
		
    p.tmp_position = p.position +(p.velocity * dt);
    p.forces = 0;
    ++it;
  }
	
  // CONSTRAINTS
  typename vector<S*>::iterator sit = springs.begin();
  const int k = 3;
  for(int i = 0; i < k; ++i) {
    sit = springs.begin();
    while(sit != springs.end()) {
      S& s = *(*sit);
      if(s.isEnabled()) {
        s.update(dt);
      }
      ++sit;
    }
  }
	
  // UPDATE VELOCITY AND POSITIONS
  it = particles.begin();
  while(it != particles.end()) {
    P& p = *(*it);
		
    if(!p.enabled) {
      ++it;
      continue;
    }
			
    p.velocity = (p.tmp_position - p.position) * fps;
		
    p.position = p.tmp_position;
		
    if(p.aging) {
      p.age += dt;
      p.agep = (p.age / p.lifespan);
    }
		
    p.tmp_position = 0;

    p.update(dt);

    ++it;
  }
}

template<class T, class P, class S>
  void Particles<T, P, S>::addForce(const T& f) {
  typename vector<P*>::iterator it = particles.begin();
  while(it != particles.end()) {
    (*it)->addForce(f);
    ++it;
  }
}

template<class T, class P, class S>
  template<class F>
  void Particles<T, P, S>::repel(const float f, F& cb) {
  float dist_sq;
  T dir;
  for(int i = 0; i < size(); ++i) {
    for(int j = i+1; j < size(); ++j) {
      dir = particles[j]->position - particles[i]->position;
      dist_sq = dir.lengthSquared();
      if(dist_sq > 0.1) {
        float e = f * (1.0f/dist_sq);
        dir.normalize();
        dir *= e;
        particles[i]->addForce(-dir * particles[i]->inv_mass * particles[i]->energy);
        particles[j]->addForce(dir * particles[j]->inv_mass  * particles[j]->energy);

        cb(*particles[i], *particles[j], dir, e, dist_sq, CB_REPEL);
      }
    }
  }
}

template<class T, class P, class S>
  template<class F>
  void Particles<T, P, S>::repel(P* p, const float radius, const float energy, F& cb) {
  T& pos = p->position;
  float radius_sq = radius * radius;
  T dir;
  float ls, f;
  P& sourcep = *p;
	
  typename vector<P*>::iterator it = particles.begin();
  while(it != particles.end()) {
    P& other = *(*it);
    T& other_pos = other.position;
    dir = pos - other_pos;
    ls = dir.lengthSquared();

    if(ls < radius_sq) {
      dir.normalize();
      f = 1.0f/ls;
      f *= energy;
      dir *= f;
      other.addForce(-dir);
			
      cb(sourcep, other, dir, f, ls, CB_REPEL_PARTICLE);
    }
		
    ++it;
  }
}


// attract to a particle
template<class T, class P, class S>
  template<class F>
  void Particles<T, P, S>::attract(P* p, const float radius, const float energy, F& cb) {
  typename vector<P*>::iterator it = particles.begin();
  P& sourcep = *p;
  T dir;
  T& pos = p->position;
  float ls;
  float f;
  float dist_sq = radius * radius;
	
  while(it != particles.end()) {
    P& other = *(*it);
    dir = pos - other.position;
    ls = dir.lengthSquared();

    if(ls > dist_sq) {
      dir.normalize();
      f = (1.0-(1.0f/ls)) * energy;
      dir *= f;
      other.addForce(dir);
      cb(sourcep, other, dir, f, ls, CB_ATTRACT_PARTICLE);
    }
    ++it;
  }
}

template<class T, class P, class S>
void Particles<T, P, S>::attract(const Vec3& pos, const float radius, const float energy) {
  typename vector<P*>::iterator it = particles.begin();
  T dir;
  float ls;
  float f;
  float dist_sq = radius * radius;
	
  while(it != particles.end()) {
    P& other = *(*it);
    dir = pos - other.position;
    ls = dir.lengthSquared();
    if(ls > dist_sq) {
      dir.normalize();
      f = (1.0-(1.0f/ls)) * energy;
      dir *= f;
      other.addForce(dir);
    }
    ++it;
  }
}

template<class T, class P, class S>
  void Particles<T, P, S>::limitSpeed(const float speed) {
  float sq = speed * speed;
  typename vector<P*>::iterator it = particles.begin();
  while(it != particles.end()) {
    P& p = **it;
    if(p.velocity.lengthSquared() > sq) {
      p.velocity = p.velocity.getNormalized() * speed;
    }
    ++it;
  }
}

template<class T, class P, class S>
  void Particles<T, P, S>::removeDeadParticles() {
  typename vector<P*>::iterator it = particles.begin();
  while(it != particles.end()) {
    P& p = *(*it);
    if(p.age > p.lifespan) {
      delete &p;
      it = particles.erase(it);
    }
    else {
      ++it;
    }
  }
}

template<class T, class P, class S>
  S* Particles<T, P, S>::addSpring(S* s) {
  springs.push_back(s);
  return s;
}


template<class T, class P, class S>
  S* Particles<T, P, S>::createSpring(P* a, P* b) {
  S* s = new S(*a, *b);
  return s;
}

template<class T, class P, class S>
  void Particles<T, P, S>::clear() {
  for(int i = 0; i < particles.size(); ++i) {
    delete particles[i];
  }
	
  for(int i = 0; i < springs.size(); ++i) {
    delete springs[i];
  }
	
  springs.clear();
  particles.clear();
}

// -----------------------------------------------------------------------------
// C++ quirks... 

template<class T, class P, class S>
  void Particles<T, P, S>::repel(const float f) { 
  NOP<T, P> n;
  this->repel<NOP<T, P> >(f, n); 
}


template<class T, class P, class S>
  void Particles<T, P, S>::repel(P* p, const float radius, const float energy) {
  NOP<T, P> n;
  this->repel<NOP<T, P> >(p, radius, energy, n);
}

template<class T, class P, class S>
  void Particles<T, P, S>::attract(P* p, const float minDist, const float energy) {
  NOP<T, P> n;
  this->attract<NOP<T, P> >(p, minDist, energy, n);
}




#endif
