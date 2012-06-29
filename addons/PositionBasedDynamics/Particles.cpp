#include "Particles.h"
namespace pbd {

Particles::Particles() 
	:drag(0.99f)
{
}

Particles::~Particles() {
}

Particle* Particles::createParticle(const Vec3& pos, float mass) {
	Particle* p = new Particle(pos, mass);
	return p;
}

Particle* Particles::addParticle(Particle* p) {
	p->dx = particles.size();
	particles.push_back(p);
	return p;
}

Spring* Particles::createSpring(Particle* a, Particle* b) {
	Spring* s = new Spring(*a, *b);
	return s;
}

Spring* Particles::addSpring(Spring* spring) {
	springs_ab.insert(std::pair<Particle*, Particle*>(&spring->a, &spring->b));
	springs_ba.insert(std::pair<Particle*, Particle*>(&spring->b, &spring->a));
	
	++spring->a.num_springs;
	++spring->b.num_springs;
	springs.push_back(spring);
	return spring;
}

BendingConstraint* Particles::createBendingConstraint(Particle* a, Particle* b, Particle* c, float max) {
	BendingConstraint* bc = new BendingConstraint(*a,*b,*c,max);
	return addBendingConstraint(bc);
}

BendingConstraint* Particles::addBendingConstraint(BendingConstraint* bc){
	bending_constraints.push_back(bc);
	return bc;
}

void Particles::addForce(const float& x, const float& y, const float& z) {
	addForce(Vec3(x,y,z));
}

void Particles::addForce(const Vec3& f) {
	vector<Particle*>::iterator it = particles.begin();
	while(it != particles.end()) {
		(*it)->addForce(f);
		++it;
	}
}

void Particles::removeDeadParticles() {
	vector<Particle*>::iterator it = particles.begin();
	while(it != particles.end()) {
		Particle& p = *(*it);
		if(p.age > p.lifespan) {
			it = particles.erase(it);
		}
		else {
			++it;
		}
	}
}

	
void Particles::update(const float& dt) {
	float fps = 1/dt;
	//float dt = 1.0/fps;
	
	// predict new locations.
	vector<Particle*>::iterator it = particles.begin();
	while(it != particles.end()) {
		Particle& p = *(*it);
		
		if(!p.enabled) {
			p.tmp_position = p.position;
			++it;
			continue;
		}
		
		p.velocity = p.velocity + (dt * p.forces); // add external forces, like gravity.
		p.velocity *= drag; // damping if necessary 
		p.tmp_position = p.position + (p.velocity * dt); // explicit euler.
		
		p.forces = 0; // reset forces.
		++it;
	}
	
	// constraints
	vector<Spring*>::iterator sit;
	vector<BendingConstraint*>::iterator bit;
	int k = 3;
	for(int i = 0; i < k; ++i) {
		sit = springs.begin();
		while(sit != springs.end()) {
			Spring& s = *(*sit);
			if(s.isEnabled()) {
				s.update(dt);
			}
			++sit;
		}
		bit = bending_constraints.begin();
		while(bit != bending_constraints.end()) {
			(*bit)->update(dt);
			++bit;
		}
		
	}
	
	// update velocity and positions
	it = particles.begin();
	while(it != particles.end()) {
		Particle& p = *(*it);
		
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

// not correct just testing...
void Particles::repel(float f) {
	for(int i = 0; i < size(); ++i) {
		for(int j = i+1; j < size(); ++j) {
			Vec3 dir = particles[j]->position - particles[i]->position;
			float dist_sq = dir.lengthSquared();
			//if(dist_sq > 0.01 && dist_sq < 200) {
			if(dist_sq > 0.1) {
				float e = f * (1.0f/dist_sq);
				dir.normalize();
				dir *= e;
				particles[i]->addForce(-dir * particles[i]->inv_mass * particles[i]->energy);
				particles[j]->addForce(dir * particles[j]->inv_mass  * particles[j]->energy);
//				printf("e: %f\n", e);
//				particles[i]->tmp_position += dir * e;
//				particles[j]->tmp_position -= dir * e;
//				particles[i]->addForce(e*dir);
//				particles[j]->addForce(e*-dir);
			}
		}
	}
	
}

void Particles::draw() {
	vector<Particle*>::iterator it = particles.begin();
	while(it != particles.end()) {
		(*it)->draw();
		++it;
	}
	
	vector<Spring*>::iterator sit = springs.begin();
	while(sit != springs.end()) {
		(*sit)->draw();
		++sit;
	}
}

bool Particles::springExists(Particle* a, Particle* b) {
  	vector<Spring*>::iterator sit = springs.begin();
	while(sit != springs.end()) {
		Spring& s = *(*sit);
		if(&s.a == a && &s.b == b || &s.a == b && &s.b == a) {
			return true;
		}
		++sit;
	}
	return false;

}

void Particles::removeSpring(Particle* a, Particle* b) {
	vector<Spring*>::iterator sit = springs.begin();
	while(sit != springs.end()) {
		Spring& s = *(*sit);
		if(&s.a == a && &s.b == b || &s.a == b && &s.b == a) {
			--a->num_springs;
			--b->num_springs;
			delete (*sit);
			sit = springs.erase(sit);
			return;
		}
		++sit;
	}
}

void Particles::removeAllSprings() {
	vector<Spring*>::iterator it = springs.begin();
	while(it != springs.end()) {
		delete (*it);
		it = springs.erase(it);
	}	
}

} // pbd

