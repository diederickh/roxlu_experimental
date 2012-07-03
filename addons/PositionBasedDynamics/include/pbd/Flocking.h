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

- Alignement energy: 180 
- Separation energy: 160
- Attraction energy: 120
- Flocking zone radius SQUARED: 3700
- Low threshold (see cinder tutorial): 0.1333
- High threshold: 06867


+ Make sure to check the bounds and reposition the boids when they go out 
  of the bounds, w/o the simulation will not look good.
*/


template<class T, class P>
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
};

template<class T, class P>
Flocking<T,P>::Flocking(vector<P*>& ps, const float zoneRadius, const float maxSpeed) 
	:ps(ps)
	,zone_radius_sq(zoneRadius * zoneRadius)
	,max_speed_sq(maxSpeed * maxSpeed)
	,max_speed(maxSpeed)
	,low(0.4)
	,high(0.6)
	,align_energy(1.0f)
	,separate_energy(1.0f)
	,attract_energy(1.0f)
{
}

template<class T, class P>
void Flocking<T, P>::update() {
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
				a.addForce(dir);
				b.addForce(-dir);
			}
			else if(perc < high) {
				// align
				f = 1.0f/ls;
				f *= align_energy;
				a.addForce(b.velocity.getNormalized() * f);
				b.addForce(a.velocity.getNormalized() * f);
				
			}
			else {
				// attract
				f = 1.0f/ls;
				dir.normalize();
				dir *= f;
				dir *= attract_energy;
				a.addForce(-dir);
				b.addForce(dir);
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

template<class T, class P>
inline void Flocking<T, P>::setMaxSpeed(const float max) {
	max_speed = max;
	max_speed_sq = max * max;
}

	

#endif