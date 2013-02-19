#ifndef ROXLU_EMITTERH
#define ROXLU_EMITTERH

#include <roxlu/Roxlu.h>
#include <pbd/PBD.h>

/* 

   EMITTER
   --------

   The Emitter class is a simple helper to spawn particles at a 
   certain pace. You can control several different spawn parameters
   which are described below. We're trying to be flexible by 
   adding a so called "Emitter Helper" which calculates a couple
   of things needed by the emitter. See below for more info.

   You can se the following emitter params:
   ---------------------------------------
   - min_particle_size:       The minimum size of a particle
   - max_particle_size:       The maximum size of a particle
   - min_particle_lifetime:   The minimum particle lifetime
   - max_particle_lifetime:   The maximum particle lifetime
   - min_particle_mass:       The minimum mass of a particle
   - max_particle_mass:       The maximum mass of a particle
   - min_particle_random_x_vel: The minimum random x velocity
   - max_particle_random_x_vel: The maximum random x velocity
   - min_particle_random_y_vel: The minimum random y velocity
   - max_particle_random_y_vel: The maximum random y velocity
   - min_particle_random_z_vel: The minumum random z velocity
   - max_particle_random_z_vel: The maximum random z velocity
   - particle_x_vel:          The X-velocity of a particle when created
   - particle_y_vel:          The Y-velocity of a particle when created
   - particle_z_vel:          The Z-velocity of a particle when created
   - min_emission:            Minimum particles to spawn per second
   - max_emission:            Maximum allowed particles (per second)
   - min_particle_lifetime:   The minimum lifetime of a particle (in frame numbers)
   - max_particle_lifetime:   The maximum lifetime of a particle (in frame numbers)


*/

// -----------------------------------------------------------
// P = Particle type: Particle<Vec2> or Particle<Vec3>
// C = Particle container: Particles
// V = Vector class
// H = Emitter Helper (e.g. at what position are the particles spawn)
template<class P, class C, class V, class H>
  class Emitter {
 public:
  Emitter(C& particles, H& helper);
  void setParticleSize(const float min, const float max);
  void setParticleLifeTime(const int min, const int max);
  void setParticleVelocity(const float x, const float y, const float z);
  void setParticleRandomVelocity(const float minx, const float maxx, const float miny, const float maxy, const float minz, const float maxz);
  void setParticleMass(const float minMass, const float maxMass);
  void setEmission(const int min, const int max);
  void enable();
  void disable();
  void update();

  H& getHelper();

  C& particles;
  H& helper;
  rx_uint64 time_to_emit;
  int emit_time;
  bool enabled;

  // Particle settings:
  float min_particle_size; 
  float max_particle_size;
  int min_particle_lifetime;
  int max_particle_lifetime;
  float min_particle_mass;
  float max_particle_mass;
  float particle_x_vel;
  float particle_y_vel;
  float particle_z_vel;
  float min_particle_random_x_vel;
  float max_particle_random_x_vel;
  float min_particle_random_y_vel;
  float max_particle_random_y_vel;
  float min_particle_random_z_vel;
  float max_particle_random_z_vel;
  int min_emission;
  int max_emission;
};


template<class P, class C, class V, class H>
  Emitter<P, C, V, H>::Emitter(C& particles, H& helper)
  :particles(particles)
  ,helper(helper)
  ,min_particle_size(0.1f)
  ,max_particle_size(2.0f)
  ,min_particle_lifetime(60)
  ,max_particle_lifetime(5 * 60)
  ,min_particle_mass(0.1f)
  ,max_particle_mass(3.0f)
  ,min_particle_random_x_vel(-2.0f)
  ,max_particle_random_x_vel(6.0f)
  ,min_particle_random_y_vel(0.0f)
  ,max_particle_random_y_vel(-4.0f)
  ,min_particle_random_z_vel(0.0f)
  ,max_particle_random_z_vel(0.0f)
  ,min_emission(100)
  ,max_emission(200)
  ,time_to_emit(0)
  ,enabled(true)
  ,particle_x_vel(0.0f)
  ,particle_y_vel(-2.8f)
  ,particle_z_vel(0.0f)
  {
    emit_time = 1000/(max_emission - min_emission);
    time_to_emit = Timer::now() + emit_time;
  }

template<class P, class C, class V, class H>
  void Emitter<P, C, V, H>::update() {
	
  if(!enabled) {
    return;
  }

  // how often do we need to emit (values can change during runtime)
  int emission_range = 0;
  if(max_emission == min_emission) {
    emission_range = max_emission;
  }
  else {
    emission_range = std::max<int>(1.0f, (max_emission - min_emission));
  }
  if(emission_range == 0) {
    return;
  }
  emit_time = 1000/emission_range;
        
  helper.update();
  particles.removeDeadParticles();

  // Do we need to emit?
  rx_uint64 now = Timer::now();
  if(now < time_to_emit) {
    return;
  }

  // Did we reach the limit?
  size_t num_particles = particles.size();
  if(num_particles > max_emission) {
    return;
  }
	

  time_to_emit = now + emit_time;

  float	size = rx_random(min_particle_size, max_particle_size);
  float	lifetime = rx_random(min_particle_lifetime, max_particle_lifetime);
  float mass = rx_random(min_particle_mass, max_particle_mass);
  V pos = helper.getPosition();
  V vel = helper.getVelocity(particle_x_vel, particle_y_vel, particle_z_vel);
  vel += helper.getRandomVelocity(min_particle_random_x_vel
                                  ,max_particle_random_x_vel
                                  ,min_particle_random_y_vel
                                  ,max_particle_random_y_vel
                                  ,min_particle_random_z_vel
                                  ,max_particle_random_z_vel
                                  ); 

  P* p = helper.createParticle(pos, mass);
  if(p != NULL) {
    p->lifespan = lifetime;
    p->size = size;
    p->velocity = vel;
    particles.addParticle(p);
  }
}

template<class P, class C, class V, class H> 
  inline H& Emitter<P, C, V, H>::getHelper() {
  return helper;
}

template<class P, class C, class V, class H> 
  inline void Emitter<P, C, V, H>::enable() {
  enabled = true;
}

template<class P, class C, class V, class H> 
  inline void Emitter<P, C, V, H>::disable() {
  enabled = false;
}

template<class P, class C, class V, class H> 
  inline void Emitter<P, C, V, H>::setParticleSize(const float mins, const float maxs) {
  min_particle_size = mins;
  max_particle_size = maxs;
}

template<class P, class C, class V, class H> 
  inline void Emitter<P, C, V, H>::setParticleVelocity(const float vx, const float vy, const float vz) {
  particle_x_vel = vx;
  particle_y_vel = vy;
  particle_z_vel = vz;
}

template<class P, class C, class V, class H> 
  inline void Emitter<P, C, V, H>::setParticleMass(const float minMass, const float maxMass) {
  min_particle_mass = minMass;
  max_particle_mass = maxMass;
}

template<class P, class C, class V, class H>
  inline void Emitter<P, C, V, H>::setParticleRandomVelocity(
                                                             const float minx
                                                             ,const float maxx
                                                             ,const float miny
                                                             ,const float maxy
                                                             ,const float minz
                                                             ,const float maxz
                                                             )
{
  min_particle_random_x_vel = minx;
  max_particle_random_x_vel = maxx;
  min_particle_random_y_vel = miny;
  max_particle_random_y_vel = maxy;
  min_particle_random_z_vel = minz;
  max_particle_random_z_vel = maxz;
}

template<class P, class C, class V, class H> 
  inline void Emitter<P, C, V, H>::setParticleLifeTime(const int minlt, const int maxlt) {
  min_particle_lifetime = minlt;
  max_particle_lifetime = maxlt;
}

template<class P, class C, class V, class H> 
  inline void Emitter<P, C, V, H>::setEmission(const int mine, const int maxe) {
  min_emission = mine;
  max_emission = maxe;
  emit_time = 1000/(std::max<int>(1.0f, max_emission - min_emission));
}


// --------------------------------------------------------------------
/**
 * The emitter class takes care of:
 * - calculating the position where to spawn new particles (getPosition)
 * - calculating the start velocity (getVelocity)
 * - applying random forces if necessary
 */
class EmitterHelper {
 public:
  //EmitterHelper(const float x, const float width, const float minY, const float maxY);
  EmitterHelper(const float x0, const float y0, const float x1, const float y1); // top left and bottom right
  Particle<Vec2>* createParticle(Vec2 pos, float mass);
  Vec2 getPosition();
  Vec2 getVelocity(const float velX, const float velY, const float velZ);
  Vec2 getRandomVelocity(const float minx, const float maxx, const float miny, const float maxy, const float minz, const float maxz);
  void setPosition(const float x, const float y); // if you want to emit from one point
  void update();
  float area[4];
};

inline EmitterHelper::EmitterHelper(const float x0, const float y0, const float x1, const float y1) {
  area[0] = x0;
  area[1] = y0;
  area[2] = x1;
  area[3] = y1;
}

inline Vec2 EmitterHelper::getPosition() {
  return Vec2(rx_random(area[0], area[2]), rx_random(area[1], area[3]));
}


inline void EmitterHelper::setPosition(const float x, const float y) {
  area[0] = x;
  area[1] = y;
  area[2] = x;
  area[3] = y;
}


inline Vec2 EmitterHelper::getVelocity(const float vx, const float vy, const float vz) {
  return Vec2(vx, vy);
}

inline Vec2 EmitterHelper::getRandomVelocity(const float minx, const float maxx, const float miny, const float maxy, const float minz, const float maxz) {
  return Vec2(rx_random(minx, maxx), rx_random(miny, maxy));
}

inline Particle<Vec2>* EmitterHelper::createParticle(Vec2 pos, float mass) {
  return new Particle<Vec2>(pos, mass);
}

inline void EmitterHelper::update() {


}
// -----------------------------------------------------------

// --------------------------------------------------------------------
/**
 * The emitter class takes care of:
 * - calculating the position where to spawn new particles (getPosition)
 * - calculating the start velocity (getVelocity)
 * - applying random forces if necessary
 */
class EmitterHelper3 {
 public:
  //EmitterHelper(const float x, const float width, const float minY, const float maxY);
  EmitterHelper3(const float x0, const float y0, const float x1, const float y1); // top left and bottom right
  Particle<Vec3>* createParticle(Vec3 pos, float mass);
  Vec3 getPosition();
  Vec3 getVelocity(const float velX, const float velY, const float velZ);
  Vec3 getRandomVelocity(const float minx, const float maxx, const float miny, const float maxy, const float minz, const float maxz);
  void update();
  float area[4];
};

inline EmitterHelper3::EmitterHelper3(const float x0, const float y0, const float x1, const float y1) {
  area[0] = x0;
  area[1] = y0;
  area[2] = x1;
  area[3] = y1;
}

inline Vec3 EmitterHelper3::getPosition() {
  return Vec3(rx_random(area[0], area[2]), rx_random(area[1], area[3]), 0.0f);
}

inline Vec3 EmitterHelper3::getVelocity(const float vx, const float vy, const float vz) {
  return Vec3(vx, vy, vz);
}

inline Vec3 EmitterHelper3::getRandomVelocity(const float minx, const float maxx, const float miny, const float maxy, const float minz, const float maxz) {
  return Vec3(rx_random(minx, maxx), rx_random(miny, maxy), rx_random(minz, maxz));
}

inline Particle<Vec3>* EmitterHelper3::createParticle(Vec3 pos, float mass) {
  return new Particle<Vec3>(pos, mass);
}

inline void EmitterHelper3::update() {
}


#endif
