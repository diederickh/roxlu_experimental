#ifndef ROXLU_SPLINEH
#define ROXLU_SPLINEH

#include <vector>
#include <roxlu/math/Vec2.h>

/**
 * Catmull Rom interpolation. 
 * --------------------------
 * Catmull Rom interpolation works with 4 points, there the 
 * local "t" value is used to interpolate between points B and C. The 
 * points A and D are used as "direction" (kind of). Therefore, for the first
 * and last point we have to choose the indices correctly. (see the index
 * checking for a,b,c,d below.). Basically for the first point, we use 0,0
 * for points A and Bs.
 *
 * Everything is normalized between [0,1]
 */
 
namespace roxlu {

inline void spline_interpolate_catmull(
	 Vec2& p0
	,Vec2& p1
	,Vec2& p2
	,Vec2& p3
	,float& t
	,float& t2
	,float& t3
	,Vec2& result
)
{
	result.x = 0.5 * ((2 * p1.x) + (-p0.x + p2.x) * t + (2 * p0.x - 5 * p1.x + 4 * p2.x - p3.x) * t2 + (-p0.x + 3 * p1.x - 3 * p2.x + p3.x) * t3);
	result.y = 0.5 * ((2 * p1.y) + (-p0.y + p2.y) * t + (2 * p0.y - 5 * p1.y + 4 * p2.y - p3.y) * t2 + (-p0.y + 3 * p1.y - 3 * p2.y + p3.y) * t3);	
}

 
// T: vector type
template<class T>
struct Spline {

	typedef T point_type;
	
	Spline();
	~Spline();
	size_t size();
	void clear();
	bool at(float t, T& result);
	void add(const T point);
	
	T& operator[](const unsigned int);
	
	std::vector<T> points;
	
	
};



template<class T>
inline Spline<T>::Spline() {
}

template<class T>
inline Spline<T>::~Spline() {
}

template<class T>
T& Spline<T>::operator[](const unsigned int dx) {
	return points[dx];
}

template<class T>
inline size_t Spline<T>::size() {
	return points.size();
}

template<class T>
inline void Spline<T>::clear() {
	return points.clear();
}


template<class T>
inline void Spline<T>::add(const T p) {
	points.push_back(p);
}

template<class T>
inline bool Spline<T>::at(float t, T& result) {
	if(points.size() < 4) {
		return false;
	}
	if(t > 0.999f) {
		t = 0.99f;
	}
	else if(t < 0) {
		t = 0;
	}

	// get local "t" (also mu)	
	float curve_p = t * (points.size()-1);
	int curve_num = curve_p;
	t = curve_p - curve_num; // local t
	
	// get the 4 point
	int b = curve_num;
	int a = b - 1;
	int c = b + 1;
	int d = c + 1;
	if(a < 0) {
		a = 0;
	}
	if(d >= points.size()) {
		d = points.size()-1;
	}
	
	T& p0 = points[a]; // a
	T& p1 = points[b]; // b 
	T& p2 = points[c]; // c
	T& p3 = points[d]; // d 

	
	float t2 = t*t;
	float t3 = t*t*t;
	spline_interpolate_catmull(p0, p1, p2, p3, t, t2, t3, result);

	// Catmull interpolation	
	//x = 0.5 * ((2 * p1.x) + (-p0.x + p2.x) * t + (2 * p0.x - 5 * p1.x + 4 * p2.x - p3.x) * t2 + (-p0.x + 3 * p1.x - 3 * p2.x + p3.x) * t3);
	//y = 0.5 * ((2 * p1.y) + (-p0.y + p2.y) * t + (2 * p0.y - 5 * p1.y + 4 * p2.y - p3.y) * t2 + (-p0.y + 3 * p1.y - 3 * p2.y + p3.y) * t3);	
	
	return true;
}

typedef Spline<Vec2> Spline2;


} // roxlu

#endif