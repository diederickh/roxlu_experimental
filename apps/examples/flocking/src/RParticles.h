#ifndef ROXLU_PARTICLES_H
#define ROXLU_PARTICLES_H

#include <vector>
#include <roxlu/Roxlu.h>

struct IState { // Integration State
  Vec3 vel;
  Vec3 accel;
};

struct RParticle {
  RParticle();
  Vec3 pos;
  Vec3 vel;
  Vec3 vel_norm;
  Vec3 forces;

  float inverse_mass;
};

class RParticles {
 public:
  RParticles();
  ~RParticles();

  void addForce(const Vec3 f);
  void update(const float dt);

 public:
  std::vector<RParticle> particles;
  std::vector<RParticle> predators;
  float max_speed;
  float max_speed_sq;
  float min_speed; 
  float min_speed_sq;
  float flee_strength;
  float repel_strength;
  float align_strength;
  float attract_strength;
  float center_strength;
  float zone_radius;
  float lower_threshold;
  float higher_threshold;
};
#endif
