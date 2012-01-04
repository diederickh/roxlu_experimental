#pragma once
#include <vector>
#include "Vec3.h"

// with thanks to pbourke !
using namespace std;

template <typename T>
class Interpolator {
public:
	inline T getAt(float t);
	void push_back(const T& point);
private:
	vector<T> points;

	inline T interpolateCubic(const T& y0, const T& y1, const T& y2, const T& y3, float mu);
	inline T interpolateCatmull(const T& p0, const T& p1, const T& p2, const T& p3, float t);
};

#define BOUNDS(pp) { if (pp < 0) pp = 0; else if (pp >= (int)points.size()-1) pp = points.size() - 1; }
template<typename T>
inline T Interpolator<T>::getAt(float t) {

	if(points.size() == 0) {
		return Vec3(0,0,0);
	}
	float  delta_t = (float)1 / (float)points.size();
    int p = (int)(t / delta_t);

    // Compute local control point indices
    int dx0 = p - 1;     BOUNDS(dx0);
    int dx1 = p;         BOUNDS(dx1);
    int dx2 = p + 1;     BOUNDS(dx2);
    int dx3 = p + 2;     BOUNDS(dx3);

    // Relative (local) time 
	float lt = (t - delta_t*(float)p) / delta_t;
	return interpolateCatmull(points[dx0], points[dx1], points[dx2], points[dx3], t);
}

template<typename T>
void Interpolator<T>::push_back(const T& point) {
	points.push_back(point);
}


//see: http://local.wasp.uwa.edu.au/~pbourke/miscellaneous/interpolation/
template<typename T>
inline T Interpolator<T>::interpolateCubic(const T& y0, const T& y1, const T& y2, const T& y3, float mu) {
	float mu2 = mu * mu;
	T a0 = y3 - y2 - y0 + y1;
	T a1 = y0 - y1 - a0;
	T a2 = y2 - y0;
	T a3 = y1;

	return (a0 * mu * mu2 + a1 * mu2 + a2 * mu + a3);
}

template<typename T>
inline T Interpolator<T>::interpolateCatmull(const T& p0, const T& p1, const T& p2, const T& p3, float t) {
	float t2 = t * t;
    float t3 = t2 * t;

    float b1 = .5 * (  -t3 + 2*t2 - t);
    float b2 = .5 * ( 3*t3 - 5*t2 + 2);
    float b3 = .5 * (-3*t3 + 4*t2 + t);
    float b4 = .5 * (   t3 -   t2    );

    return (p0*b1 + p1*b2 + p2*b3 + p3*b4); 
}

typedef Interpolator<Vec3>	Interpolator3f;	