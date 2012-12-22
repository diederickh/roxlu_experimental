#include "RParticles.h"

RParticle::RParticle() 
{
  inverse_mass = 1.0f; 
  float r = 0.1;
  vel.x = random(-r, r);
  vel.y = random(-r, r);
  vel.z = random(-r, r);
  vel_norm = 0.0f;
  forces = 0.0f;
}

// ----------------------------------------
RParticles::RParticles() {
  max_speed = 4.5;
  max_speed_sq = max_speed_sq * max_speed_sq;
  min_speed = 1.4;
  min_speed_sq = min_speed * min_speed;

  flee_strength = 1.2f;
  repel_strength = 0.01;
  align_strength = 0.04;
  attract_strength = 0.01;
  center_strength = 0.01;
  zone_radius = 50.0;
  lower_threshold = 0.50f;
  higher_threshold = 0.80f;
}

RParticles::~RParticles() { 
}

void RParticles::update(const float dt) {
  Vec3 accel;
  float ls;

  // FLOCKING
  float flee_dest = 40.0f;;
  float flee_dest_sq = flee_dest * flee_dest;
  float zone_radius_sq = zone_radius * zone_radius;
  float threshold_delta = higher_threshold - lower_threshold;
  float center_radius = 200;
  float center_radius_sq = center_radius * center_radius;
  Vec3 dir;
  float perc;
  float F;
  
  for(std::vector<RParticle>::iterator itp = predators.begin(); itp != predators.end(); ++itp) {
    RParticle& p = *itp;
    dir = -p.pos;
    ls = dir.lengthSquared();
    if(ls > center_radius_sq) {
      dir.normalize();
      dir *= (ls - center_radius_sq) * 0.0025f;
      p.forces += dir;
    }
  }

  for(std::vector<RParticle>::iterator ita = particles.begin(); ita != particles.end(); ++ita) {
    RParticle& a = *ita;

    // PREDATORS
    for(std::vector<RParticle>::iterator itp = predators.begin(); itp != predators.end(); ++itp) {
      RParticle& pred = *itp;
      dir = a.pos - pred.pos;
      ls = dir.lengthSquared();
      if(ls < flee_dest_sq) {
        F = (flee_dest_sq / ls - 1.0f) * 0.1f;
        dir.normalize();
        dir *= flee_strength;
        a.forces += dir;
      }
    }
    
    std::vector<RParticle>::iterator itb = ita;

    // ATTRACT TO CENTER
    Vec3 d = -a.pos;
    ls = d.lengthSquared();
    if(ls > center_radius_sq) {
      d.normalize();
      d *= ((ls - center_radius_sq) * 0.00025f);
      a.forces += d;
    }
    
    for(++itb; itb != particles.end(); ++itb) {
      RParticle& b = *itb;
      dir = a.pos - b.pos;
      ls = dir.lengthSquared();

      if(ls > zone_radius_sq ) {
        continue;
      }

      perc = ls / zone_radius_sq;
      if(perc < lower_threshold) {
        // SEPARATION        
        F = (lower_threshold / perc - 1.0f) * repel_strength;
        dir.normalize();
        dir *= F;
        a.forces += dir;
        b.forces -= dir;
      }
      else if(perc < higher_threshold) {
        // ALIGNMENT
        perc = (perc - lower_threshold) / threshold_delta;
        F = perc * align_strength;
        a.vel += b.vel_norm * F;
        b.vel += a.vel_norm * F;
      }
      else  {
        // COHESION
        perc = (perc - higher_threshold) / (1.0f - higher_threshold);
        dir.normalize();
        F = perc * attract_strength;
        dir *= F;
        a.forces -= dir;
        b.forces += dir;
      }
    }
  }

  // INTEGRATE 
  for(std::vector<RParticle>::iterator it = particles.begin(); it != particles.end(); ++it) {
    RParticle& p = *it;
    accel = p.inverse_mass * p.forces;
    p.vel += accel;
    p.vel_norm = p.vel;
    p.vel_norm.normalize();

    ls = p.vel.lengthSquared();
    if(ls > max_speed_sq) {
      p.vel = p.vel_norm * max_speed;
    }
    else if(ls < min_speed_sq) {
      p.vel = p.vel_norm * min_speed;
    }

    p.pos += p.vel; 
    p.forces = 0;
    p.vel *= 0.99f;
  }

  float pmax_speed = 16.0f;
  float pmax_speed_sq = pmax_speed * pmax_speed;
  float pmin_speed = 6.0f;
  float pmin_speed_sq = pmin_speed * pmin_speed;
  for(std::vector<RParticle>::iterator it = predators.begin(); it != predators.end(); ++it) {
    RParticle& p = *it;
    p.vel += (p.forces);
    p.vel_norm = p.vel;
    p.vel_norm.normalize();

    if(p.vel.lengthSquared() > pmax_speed_sq) {
      p.vel = p.vel_norm * pmax_speed;
    }
    else if(p.vel.lengthSquared() < pmin_speed_sq) {
      p.vel = p.vel_norm * pmin_speed;
    }

    p.pos += p.vel;
    p.vel *= 0.9900f;
    p.forces = 0;
  }
}

void RParticles::addForce(const Vec3 f) {
  for(std::vector<RParticle>::iterator it = particles.begin(); it != particles.end(); ++it) {
    (*it).forces += f;
  }
}
