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

      size_t size();                         /* the number of points */
      void clear();                          /* remove all points */
      float length();                        /* get the length of the polyline */
      T at(float t);                         /* interpolate using catmull rom */
      T get(float t);                        /* get a point which is exactly on the line, at this time step, t is between 0 and 1 */
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
    inline float Spline<T>::length() {
    float l = 0.0f;
    for(size_t i = 0; i < points.size() - 1; ++i) {
      T a = points[i];
      T b = points[i + 1];
      l += (b - a).length();
    }
    return l;
  }

  // get the position at the line using linear interpolation
  // this will only work for vector classes! If you have a long
  // line and you want to divide it in equal parts you can use this
  // function to get the correct position on the line. 
  //
  // `t = 0` means the start of the line
  // `t = 1` means end of the line
  template<class T>
   inline T Spline<T>::get(float t) {

    if(!points.size()) {
      return T();
    }

    if(points.size() == 1) {
      return points[0];
    }

    if(t > 0.9999f) {
      t = 1.0f;
      return points.back();
    }

    if(t < 0.0f) {
      t = 0.0f;
    }

    // -------
    float line_length = length();
    float curr_l = 0.0;
    float sample_at = line_length * t;
    float curr_dist = 0;
    float prev_dist = 0;
    size_t start_dx = 0;
    size_t end_dx = 0;

    for(size_t i = 0; i < points.size()-1; ++i) {
      T a = points[i];
      T b = points[i + 1];
      curr_l = (b-a).length();
      curr_dist += curr_l;
      start_dx = i;
      end_dx = i + 1;

      if(curr_dist >= sample_at) {
        break;
      }
      prev_dist = curr_dist;
    }


    T segment_start = points[start_dx];
    T segment_end = points[end_dx];
    float segment_length = sample_at - prev_dist;
    T segment_dir = (segment_end - segment_start).normalize();
    if(segment_length <= 0.01) {
      return segment_start;
    }

    T result = segment_start + segment_dir * segment_length;
    return result;
  }

  template<class T>
    inline T Spline<T>::at(float t) {
    if(points.size() < 4) {
      return T();
    }
    if(t > 0.999f) {
      t = 0.99f;
    }
    else if(t < 0) {
      t = 0;
    }

    T result;

    // get local "t" (also mu)	
    float curve_p = t * (points.size()-1);
    int curve_num = curve_p;
    t = curve_p - curve_num; // local t
	
    // get the 4 points
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
	
    return result;
  }

  typedef Spline<Vec2> Spline2;


} // roxlu

#endif
