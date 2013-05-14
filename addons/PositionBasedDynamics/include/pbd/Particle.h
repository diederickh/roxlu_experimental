#ifndef ROXLU_PBD_PARTICLEH
#define ROXLU_PBD_PARTICLEH

template<class T>
class Spring;

template<class T>
class Particle {
 public:

  Particle(const T& pos, float mass = 1.0f);
  void update(const float dt);
  void draw();
  void addForce(const T& f);
  void enable();
  void disable();
  void addSpring(Spring<T>* s); 
  void setColor(const float r, const float g, const float b, float a = 1.0f);
  void setMass(const float m);
		
  T position;
  T forces;
  T tmp_position;
  T velocity;
  T d;  /* for the hair simulation */
	
  float mass;
  float size;
  float energy;
  float inv_mass;
  bool enabled;
  int dx;
  float color[4];
	
  float lifespan;
  float age;
  float agep;
  bool aging;
	
  vector<Spring<T>* > springs;
};


template<class T>
Particle<T>::Particle(const T& pos, float mass) 
:position(pos)
,mass(mass)
  ,enabled(true)
  ,aging(true)
  ,dx(0)
  ,size(1.0f)
  ,energy(1.0f)
  ,lifespan(0.0f)
  ,age(0.0f)
  ,agep(0.0f)
{
  color[0] = color[1] = color[2] = color[3] = 1.0f;
  setMass(mass);
}

template<class T>
inline void Particle<T>::setMass(const float m) {
  if(m < 0.01) {
    mass = 0.0f;
    inv_mass = 0.0f;
  }
  else {
    inv_mass = 1.0f / m;
    mass = m;
  }
}

template<class T>
inline void Particle<T>::addSpring(Spring<T>* s) {
  springs.push_back(s);
}

template<class T>
inline void Particle<T>::addForce(const T& f) {
  forces += f;
}

template<class T>
inline void Particle<T>::update(const float dt) {
}


template<class T>
inline void Particle<T>::enable() {
  enabled = true;
}

template<class T>
inline void Particle<T>::disable() {
  enabled = false;
}

template<class T>
inline void Particle<T>::setColor(const float r, const float g, const float b, float a) {
  color[0] = r;
  color[1] = g;
  color[2] = b;
  color[3] = a;
}


#endif
