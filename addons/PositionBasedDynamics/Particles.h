
#ifndef ROXLU_PBD_PARTICLESH
#define ROXLU_PBD_PARTICLESH

#include "Particle.h"
#include "Spring.h"

enum ParticleCallbacks {
	 CB_REPEL
	,CB_REPEL_PARTICLE
	,CB_ATTRACT_PARTICLE
};


// C A L L B A C K    F U N C T O R 
// +++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
template<class T>
struct NOP {
	void operator()(Particle<T>& a, Particle<T>& b, T& dir, const float& f, const float& lengthSQ, int type) const {}
};


// P A R T I C L E S
// +++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
template<class T>
class Particles {
public:
	Particles();
	~Particles();
	
	Particle<T>* createParticle(const T& pos, float mass = 1.0f);
	Particle<T>* addParticle(const T& pos, float mass = 1.0f);
	Particle<T>* addParticle(Particle<T>* p);
	
	Spring<T>* createSpring(Particle<T>* a, Particle<T>* b);
	Spring<T>* addSpring(Spring<T>* s);
		
	void addForce(const T& f); 
	
	// attract to another particle
	template<class F>
	void attract(Particle<T>* p, const float& energy, F& cb = NOP<T>());
	void attract(Particle<T>* p, const float& energy);
	
	// repel from each other, with callback when repelled
	template<class F>
	void repel(const float& f, F& cb = NOP<T>());
	void repel(const float& f);
	
	// repel from particle, with callback when repelled
	template<class F>
	void repel(Particle<T>* p, const float& radius, const float& energy, F& cb = NOP<T>());
	void repel(Particle<T>* p, const float& radius, const float& energy);
	
	void update(const float& dt);
	void removeDeadParticles();
	
	Particle<T>* operator[](const unsigned int& dx);
	size_t size();
		
	vector<Spring<T>* > springs;
	vector<Particle<T>* > particles;
	float drag;
};


template<class T>
Particles<T>::Particles() 
	:drag(0.99f)
{
}

template<class T>
Particles<T>::~Particles() {
}

template<class T>
inline size_t Particles<T>::size() {
	return particles.size();
}

template<class T>
Particle<T>* Particles<T>::operator[](const unsigned int& dx) {
	return particles[dx];
}

template<class T>
inline Particle<T>* Particles<T>::addParticle(const T& pos, float mass) {
	Particle<T>* p = createParticle(pos, mass);
	return addParticle(p);
}

template<class T>
inline Particle<T>* Particles<T>::createParticle(const T& pos, float mass) {
	Particle<T>* p = new Particle<T>(pos, mass);
	return p;
}

template<class T>
inline Particle<T>* Particles<T>::addParticle(Particle<T>* p) {
	particles.push_back(p);
	return p;
}

template<class T>
void Particles<T>::update(const float& dt) {
	float fps = 1.0f/dt;
	
	// PREDICT NEW LOCATIONS
	typename vector<Particle<T>* >::iterator it = particles.begin();
	while(it != particles.end()) {
		Particle<T>& p = *(*it);
		
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
	typename vector<Spring<T>* >::iterator sit = springs.begin();
	const int k = 3;
	for(int i = 0; i < k; ++i) {
		sit = springs.begin();
		while(sit != springs.end()) {
			Spring<T>& s = *(*sit);
			if(s.isEnabled()) {
				s.update(dt);
			}
			++sit;
		}
	}
	
	// UPDATE VELOCITY AND POSITIONS
	it = particles.begin();
	while(it != particles.end()) {
		Particle<T>& p = *(*it);
		
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
				
		++it;
	}
}

template<class T>
void Particles<T>::addForce(const T& f) {
	typename vector<Particle<T>* >::iterator it = particles.begin();
	while(it != particles.end()) {
		(*it)->addForce(f);
		++it;
	}
}

template<class T>
template<class F>
void Particles<T>::repel(const float& f, F& cb) {
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

template<class T>
template<class F>
void Particles<T>::repel(Particle<T>* p, const float& radius, const float& energy, F& cb) {
	T& pos = p->position;
	float radius_sq = radius * radius;
	T dir;
	float ls, f;
	Particle<T>& sourcep = *p;
	
	typename vector<Particle<T>* >::iterator it = particles.begin();
	while(it != particles.end()) {
		Particle<T>& other = *(*it);
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
template<class T>
template<class F>
void Particles<T>::attract(Particle<T>* p, const float& energy, F& cb) {
	typename vector<Particle<T>* >::iterator it = particles.begin();
	float ls;
	T dir;
	float f;
	Particle<T>& sourcep = *p;
	T& pos = p->position;
	while(it != particles.end()) {
		Particle<T>& other = *(*it);
		dir = pos - other.position;
		ls = dir.lengthSquared();
		if(ls > 0.01) {
			f = 1.0f/ls;
			dir.normalize();
			dir *= energy;
			dir *= f;
			other.addForce(dir);

			cb(sourcep, other, dir, f, ls, CB_ATTRACT_PARTICLE);
		}
		++it;
	}
}

template<class T>
void Particles<T>::removeDeadParticles() {
	typename vector<Particle<T>* >::iterator it = particles.begin();
	while(it != particles.end()) {
		Particle<T>& p = *(*it);
		if(p.age > p.lifespan) {
			it = particles.erase(it);
		}
		else {
			++it;
		}
	}
}

template<class T>
Spring<T>* Particles<T>::addSpring(Spring<T>* s) {
	springs.push_back(s);
	return s;
}


template<class T>
Spring<T>* Particles<T>::createSpring(Particle<T>* a, Particle<T>* b) {
	Spring<T>* s = new Spring<T>(*a, *b);
	return s;
}

// -----------------------------------------------------------------------------
// C++ quirks... 
template<class T>
void Particles<T>::repel(const float& f) { 
	NOP<T> n;
	this->repel<NOP<T> >(f, n); 
}

template<class T>
void Particles<T>::repel(Particle<T>* p, const float& radius, const float& energy) {
	NOP<T> n;
	this->repel<NOP<T> >(p, radius, energy, n);
}

template<class T>
void Particles<T>::attract(Particle<T>* p, const float& energy) {
	NOP<T> n;
	this->attract<NOP<T> >(p, energy, n);
}



#endif