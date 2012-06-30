#ifndef ROXLU_BINNING_2DH
#define ROXLU_BINNING_2DH

#include <math.h>
#include <vector>
using std::vector;


/*

We assume a particle has this structure:

struct Vec2 {
	float x;
	float y;
};

struct Particle {
	Vec2 position;
	Vec2 forces	
};

Binning2D<Particle<Vec2> > bins; 
bins(ps.particles, ofGetWidth(), ofGetHeight(), 10)
*/

#define BIN2D_DX(i,j) (((j)*cols) + (i))
#define BIN2D_TO_J(y) (y/size)
#define BIN2D_TO_I(x) (x/size)

template<class T>
class Binning2D {
public:
	Binning2D(vector<T*>& particles, const unsigned int& w, const unsigned int& h, const int& size  );
	void update();
	bool getNeighbors(int i, int j, vector<T*>& result);
	void repel(const float& energy);
	void print();
private:

	vector<T*>& particles;
	vector<vector<T*> > bins;
	vector<int> nparticles;
	unsigned int w;
	unsigned int h;
	unsigned int size;
	unsigned int cols;
	unsigned int rows;
	unsigned int nbins;
};


template<class T>
Binning2D<T>::Binning2D(vector<T*>& particles,  const unsigned int& w, const unsigned int& h, const int& size)
	:particles(particles)
	,w(w)
	,h(h)
	,size(size)
{
	cols = (unsigned int)(ceilf(float(w)/float(size)));
	rows = (unsigned int)(ceilf(float(h)/float(size)));
	nbins = cols * rows;
	nparticles.assign(nbins,0);
	bins.resize(nbins);
}

template<class T>
void Binning2D<T>::update() {
	for(unsigned int i = 0; i < nbins; ++i) {
		nparticles[i] = 0;
		bins[i].clear();
	}
	unsigned int i = 0;
	unsigned int j = 0;
	unsigned int dx = 0;
	float* pos = NULL;
	for(typename vector<T*>::iterator it = particles.begin(); it != particles.end(); ++it) {
		T& particle = *(*it);
		pos = &particle.position.x;
		if(pos[0] < 0 || pos[1] < 0 || pos[1] > h || pos[0] > w) {
			continue;
		}
		i = BIN2D_TO_I(particle.position.x);
		j = BIN2D_TO_J(particle.position.y);
		dx = BIN2D_DX(i,j);
		++nparticles[dx];
		bins[dx].push_back(*it);
	}
}

template<class T>
void Binning2D<T>::repel(const float& energy) {
	T* a = NULL;
	T* b = NULL;
	vector<T*> neighbors;
	size_t n = 0;
	
	int dx = 0;	
	int i = 0;
	int j = 0;
	int k = 0;
	int l = 0;
	float sq = 0.0f;
	float xdir = 0;
	float ydir = 0;
	
	for(i = 0; i < cols; ++i) {
		for(j = 0; j < rows; ++j) {
			dx = BIN2D_DX(i,j);
			if(nparticles[dx] <= 0) {
				continue;
			}
			neighbors.clear();
			if(!getNeighbors(i,j,neighbors)) {
				continue;
			}

			n = neighbors.size();
			for(k = 0; k < n; ++k) {
				a = neighbors[k];
				for(l = k+1; l < n; ++l) {
					b = neighbors[l];
					
					xdir = b->position.x - a->position.x;
					ydir = b->position.y - a->position.y;
					sq = (xdir * xdir + ydir * ydir);
					if(sq > 0.01f) {
						float f = 1.0f/sq;
						f *= energy;
						xdir *= f;
						ydir *= f;
						a->forces.x -= xdir;
						a->forces.y -= ydir;
						b->forces.x += xdir;
						b->forces.y += ydir;
					}
				}
			}
		}
	}
}


template<class T>
bool Binning2D<T>::getNeighbors(int i, int j, vector<T*>& result) {
	int dx = 0;
	std::back_insert_iterator<vector<T*> > back = back_inserter(result);
	
	// top
	if(j-1 >= 0) {
		dx = BIN2D_DX(i, j-1);
		if(nparticles[dx] > 0) {
			copy(bins[dx].begin(), bins[dx].end(), back);
		}
	}


	// right column
	if(i+1 < cols) {
		// right
		dx = BIN2D_DX(i+1, j);
		if(nparticles[dx] > 0) {
			copy(bins[dx].begin(), bins[dx].end(), back);
		}		


		if(j-1 >= 0) {
			// top right
			dx = BIN2D_DX(i+1, j-1);
			if(nparticles[dx] > 0) {
				copy(bins[dx].begin(), bins[dx].end(), back);
			}
		}

		if(j+1 < rows) {
			// bottom right
			dx = BIN2D_DX(i+1, j+1);
			if(nparticles[dx] > 0) {
				copy(bins[dx].begin(), bins[dx].end(), back);
			}
		}
	}

	// left column
	if(i-1 >= 0) {
		// left
		dx = BIN2D_DX(i-1, j);
		if(nparticles[dx] > 0) {
			copy(bins[dx].begin(), bins[dx].end(), back);
		}

		if(j-1 >= 0) {
			// top left
			dx = BIN2D_DX(i-1, j-1);
			if(nparticles[dx] > 0) {
				copy(bins[dx].begin(), bins[dx].end(), back);
			}
		}

		if(j+1 < rows) {
			// bottom left
			dx = BIN2D_DX(i-1, j+1);
			if(nparticles[dx] > 0) {
				copy(bins[dx].begin(), bins[dx].end(), back);
			}	
		}
	}


	// bottom	
	if(j+1 < rows) {
		dx = BIN2D_DX(i,j+1);
		if(nparticles[dx] > 0) {
			copy(bins[dx].begin(), bins[dx].end(), back);
		}
	}
	return result.size();
}


template<class T>
void Binning2D<T>::print() {
	for(int i = 0; i < nbins; ++i) {
		if(nparticles[i] == 0) {
			continue;
		}
		printf("Bin (%d) = %d\n", i, nparticles[i]);
	}
	printf("Cols: %d\n", cols);
	printf("Rows: %d\n", rows);
	printf("Bin size: %d\n", size);
}

#endif