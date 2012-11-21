#ifndef ROXLU_PBD_SPRINGH
#define ROXLU_PBD_SPRINGH

#include <roxlu/Roxlu.h>
#include <pbd/Particle.h>

template<class T>
class Spring {
 public:
  Spring(Particle<T>& a, Particle<T>& b);
  void update(const float& dt);
	
  void enable();
  void disable();
  bool isEnabled();

  Particle<T>& a;
  Particle<T>& b;
	
  float k;
  float rest_length;
  float curr_length;
  bool enabled;
};

template<class T>
Spring<T>::Spring(Particle<T>& a, Particle<T>& b)
:a(a)
,b(b)
  ,enabled(true)
{
  rest_length = (b.position - a.position).length();
  curr_length = rest_length;
  k = 1.0f;
  a.addSpring(this);
  b.addSpring(this);
}

template<class T>
void Spring<T>::update(const float& dt) {
  T dir = b.tmp_position - a.tmp_position;
  const float len = dir.length();
  const float inv_mass = a.inv_mass + b.inv_mass;
  const float f = ((rest_length - len)/inv_mass) * k;
  curr_length = len;
  dir /= len;
  dir *= f;
  dir *= dt;
	
  if(a.enabled) {
    a.tmp_position -= (dir * a.inv_mass);
  }
  if(b.enabled) {
    b.tmp_position += (dir * b.inv_mass);
  }
}

template<class T>
inline bool Spring<T>::isEnabled() {
  return enabled;
}

template<class T> 
inline void Spring<T>::enable() {
  enabled = true;
}

template<class T>
inline void Spring<T>::disable() {
  enabled = false;
}
#endif
