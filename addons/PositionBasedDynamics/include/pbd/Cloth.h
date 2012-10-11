#ifndef ROXLU_PBD_CLOTH
#define ROXLU_PBD_CLOTH


#include <roxlu/math/Vec3.h>
#include <roxlu/math/Vec2.h>
#include <roxlu/3d/Triangle.h>
#include <pbd/PBD.h>

#define CLOTH_DX(i,j) (((j)*(cols+1))+(i))

// T = Vec2 / Vec3
// P = Particle
// S = Spring
template<class T, class P, class S>
void createCloth(
	 Particles<T, P, S>& ps
	,int cols
	,int rows
	,const float width
	,const float height
	,vector<Triangle>& triangles
	,float k = 1.0f
)
{
	float h = float(height)/rows;
	float w = float(width)/cols;

	// create particles.
	vector<P*> particles;
	P* p;
	particles.assign((cols+1)*(rows+1),p);
	
	for(int i = 0; i <= cols; ++i) {
		for(int j = 0; j <= rows; ++j) {
			float x = i * w;
			float y = j * h;
			int dx = CLOTH_DX(i,j);
			T ppos(x,y);
			P* particle = ps.createParticle(ppos);
			particles[dx] = particle;
		}
	}
	
	// add paticles (in correct order) to the system.
	for(int i = 0; i < particles.size(); ++i) {
		ps.addParticle(particles[i]);
	}
	
	// create springs.
	for(int i = 0; i < cols; ++i) {
		for(int j = 0; j < rows; ++j) {
			
			P* a = ps[CLOTH_DX(i,j)];
			P* b = ps[CLOTH_DX(i+1,j)];
			P* c = ps[CLOTH_DX(i+1,j+1)];
			P* d = ps[CLOTH_DX(i,j+1)];
		
			ps.addSpring(ps.createSpring(a,b))->k = k;
			ps.addSpring(ps.createSpring(b,c))->k = k;
			ps.addSpring(ps.createSpring(c,d))->k = k;

			ps.addSpring(ps.createSpring(a,d))->k = k;
			ps.addSpring(ps.createSpring(a,c))->k = k;
			ps.addSpring(ps.createSpring(b,d))->k = k;
		
			Triangle tri_a(a->dx,b->dx,c->dx);
			Triangle tri_b(c->dx,d->dx,a->dx);
			
			triangles.push_back(tri_a);
			triangles.push_back(tri_b);
		}
	}
} 


// e.g. createCloth3(ps, cols, rows, 0.41, 0.1, tris, 0.9);
// USE ps.update(0.1f) with 60fp
inline void createCloth3(Particles<Vec3, Particle<Vec3>, Spring<Vec3> >& ps, int rows, int cols, const float width, const float height, vector<Triangle>& triangles, float k = 1.0f) {
	createCloth<Vec3, Particle<Vec3>, Spring<Vec3> >(ps, rows, cols, width, height, triangles, k);
}

inline void createCloth2(Particles<Vec2, Particle<Vec2>, Spring<Vec2> >& ps, int rows, int cols, const float width, const float height, vector<Triangle>& triangles, float k = 1.0f) {
	createCloth<Vec2, Particle<Vec2>, Spring<Vec2> >(ps, rows, cols, width, height, triangles, k);
}
#endif
