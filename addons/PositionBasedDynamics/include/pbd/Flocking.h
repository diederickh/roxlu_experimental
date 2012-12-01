#ifndef ROXLU_PBD_FLOCKINGH
#define ROXLU_PBD_FLOCKINGH

#include <vector>
using std::vector;

/*


  FLOCKING
  --------

  When creating a flocking system, tweaking the energy levels for
  aligment, separation and attraction are important to get a good looking
  behavior. 

  Without correct energy levels it will just look like a group of particles
  moving around. I've got a pretty nice behavior with these settings.. note btw,
  that I'm using exponential forces: (1.0f/length_squared)*force

  2D settings:
  ------------------
  - Alignement energy: 180 
  - Separation energy: 160
  - Attraction energy: 120
  - Flocking zone radius SQUARED: 3700
  - Low threshold (see cinder tutorial): 0.1333
  - High threshold: 06867

  3D settings
  ------------
  - Area 5x5x5 units (so simulation is done in this area (opengl units)
  - Flocking zone radius SQUARED:  1.7760
  - Flocking max speed: 0.1 (try w/o enabling this)
  - High threshold: 0.8267
  - Low threshold: 0.65
  - Alignment energy: 0.0009
  - Separation energy: 0.0020
  - Attraction energy: 0.0041
  - Also make sure to keep the flock in a sphere, I used a sphere with squared
  radius 25.0f and added an attractive force towards the center when boids fly
  out of this sphere. Forces was about 0.0867


  + Make sure to check the bounds and reposition the boids when they go out 
  of the bounds, w/o the simulation will not look good.
*/



template<class T, class P>
  class FlockingForceAdder {
 public:
  void operator()(const T& force, P& boid) {
    boid.addForce(force);
  }
};

// T - Vector Type
// P - Particle Type.
// FA - Force adder: by default we call p->addForce, but you can customize this.
template<class T, class P, class FA>
  class Flocking {
 public:
  Flocking(vector<P*>& ps, const float zoneRadius, const float maxSpeed);
  void update();
  void setMaxSpeed(const float max);
	
  vector<P*>& ps;
  float zone_radius_sq;
  float low;  // separate
  float high; // align
  float align_energy;
  float separate_energy;
  float attract_energy;
  float max_speed_sq;
  float max_speed;
  FA force_adder;
};

template<class T, class P, class FA>
  Flocking<T,P, FA>::Flocking(vector<P*>& ps, const float zoneRadius, const float maxSpeed) 
  :ps(ps)
  ,zone_radius_sq(zoneRadius * zoneRadius)
  ,max_speed_sq(maxSpeed * maxSpeed)
  ,max_speed(maxSpeed)
  ,low(0.1575)
  ,high(0.6925)
  ,align_energy(0.0003f)
  ,separate_energy(0.0003f)
  ,attract_energy(0.0003f)
  {
  }

template<class T, class P, class FA>
  void Flocking<T, P, FA>::update() {
  T dir;
  float ls;
  float f;
  float perc;
	
  for(typename vector<P*>::iterator ita = ps.begin(); ita != ps.end(); ++ita) {
    P& a = *(*ita);
    typename vector<P*>::iterator itb = ita;
		
    for(++itb; itb != ps.end(); ++itb) {
      P& b = *(*itb);
      dir = a.position - b.position;
      ls = dir.lengthSquared();
			
      if(ls > zone_radius_sq || ls < 0.01f) {
        continue;
      }
			
      perc = ls / zone_radius_sq;

			
      if(perc < low) {
        // separate
        f = 1.0f/ls;
        dir.normalize();
        dir *= f;
        dir *= separate_energy;
        force_adder(dir, a);
        force_adder(-dir, b);
      }
      else if(perc < high) {
        // align
        f = 1.0f/ls;
        f *= align_energy;
        force_adder(b.velocity.getNormalized() * f, a);
        force_adder(a.velocity.getNormalized() * f, b);
      }
      else {
        // attract
        f = 1.0f/ls;
        dir.normalize();
        dir *= f;
        dir *= attract_energy;
        force_adder(-dir, a);
        force_adder(dir, b);
      }
    }
  }
	
  for(typename vector<P*>::iterator ita = ps.begin(); ita != ps.end(); ++ita) {
    P& a = *(*ita);
    ls = a.velocity.lengthSquared();
    if(ls > max_speed_sq) {
      a.velocity = a.velocity.getNormalized() * max_speed;
    }
  }
	
}

template<class T, class P, class FA>
  inline void Flocking<T, P, FA>::setMaxSpeed(const float max) {
  max_speed = max;
  max_speed_sq = max * max;
}

	

#endif
