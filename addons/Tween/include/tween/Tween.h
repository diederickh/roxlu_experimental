#ifndef ROXLU_TWEEN_H
#define ROXLU_TWEEN_H

#include <roxlu/core/Utils.h>
#include <roxlu/core/Log.h>
#include <vector>

enum TweenFunction {
  TWEEN_LINEAR,

  TWEEN_IN_QUAD,
  TWEEN_OUT_QUAD,
  TWEEN_INOUT_QUAD,
  
  TWEEN_IN_CUBIC,
  TWEEN_OUT_CUBIC,
  TWEEN_INOUT_CUBIC,

  TWEEN_IN_QUART,
  TWEEN_OUT_QUART,
  TWEEN_INOUT_QUART,

  TWEEN_IN_QUINT,
  TWEEN_OUT_QUINT,
  TWEEN_INOUT_QUINT,
  
  TWEEN_IN_EXPO,
  TWEEN_OUT_EXPO,
  TWEEN_INOUT_EXPO,
  
  TWEEN_IN_CIRC,
  TWEEN_OUT_CIRC,
  TWEEN_INOUT_CIRC,
  
  TWEEN_IN_SINE,
  TWEEN_OUT_SINE,
  TWEEN_INOUT_SINE,
  
  TWEEN_IN_BACK,
  TWEEN_OUT_BACK,
  TWEEN_INOUT_BACK,
  
  TWEEN_IN_ELASTIC,
  TWEEN_OUT_ELASTIC,
  TWEEN_INOUT_ELASTIC,
  
  TWEEN_IN_BOUNCE,
  TWEEN_OUT_BOUNCE,
  TWEEN_INOUT_BOUNCE /* @TODO */
  
};

template<class T>
class Tween {
 public: 
  Tween();
  ~Tween();

  void set(T from, T to, double duration, double delay, TweenFunction tf = TWEEN_LINEAR);
  void setType(TweenFunction tf);
  void start();
  void update();
  T getValue();

  double delay;
  double duration;
  double timeout;

  bool ready;             /* indicates if tween is ready */
  T value;                /* current value */
  T from;                 /* tween from */
  T dir;                  /* tween distance/range */
  T to;                   /* tween to, end result */ 
  TweenFunction type;     /* the tween type, linear, bounce, elastic etc.. */
};

// --------------------------------

template<class T>
class TweenChain {
 public:
  TweenChain();
  void push_back(Tween<T> tween);
  size_t size();
  void update();
  void start();
  T getValue();

 public:
  std::vector<Tween<T> > tweens;
  size_t dx;
  bool ready;
};

// --------------------------------

template<class T>
Tween<T>::Tween() 
  :ready(false)
  ,duration(0)
  ,timeout(0)
  ,delay(0)
{
}

template<class T>
Tween<T>::~Tween() {
  ready = false;
  timeout = 0;
  duration = 0;
}

template<class T>
void Tween<T>::set(T from, T to, double duration, double delay, TweenFunction tf) {
  
  this->from = from;
  this->value = from;
  this->to = to;
  this->dir = to - from;
  this->duration = duration;
  this->delay = delay;
  this->ready = true;
  this->timeout = 0;
  this->type = tf;

}

template<class T>
void Tween<T>::setType(TweenFunction tf) {
  type = tf;
}

template<class T>
void Tween<T>::start() {

  value = from;
  timeout = rx_millis() + duration + delay;
  ready = false;

}

template<class T>
void Tween<T>::update() {

  if(!timeout) {
    return;
  }

  double n = rx_millis();
  double t = duration - (timeout - n);
  if(t < 0.0) {
    return;
  }

  t = CLAMP(t, 0, duration);
  ready = (t == duration);
  
  switch(type) {

    case TWEEN_LINEAR: {
      value = from + (t / duration) * dir;
      return;
    }

      // Qaudratic
    case TWEEN_IN_QUAD: {
      t /= duration;
      value = dir * t * t + from;
      return;
    }
      
    case TWEEN_OUT_QUAD: {
      t /= duration;
      value = -dir * t * (t-2) + from;
      return;
    }
      
    case TWEEN_INOUT_QUAD: {
      t /= duration/2;
      if (t < 1) {
        value = dir/2 * t * t + from;
      }
      else {
        t--;
        value = -dir/2 * (t * (t-2) - 1) + from;
      }
      return;
    }
      // Cubic
    case TWEEN_IN_CUBIC: {
      t /= duration;
      value = dir * t * t * t + from;
      return;
    }
      
    case TWEEN_OUT_CUBIC: {
      t /= duration;
      t--;
      value = dir * (t * t * t + 1) + from;
      return;
    }
      
    case TWEEN_INOUT_CUBIC: {
      t /= duration/2;
      if (t < 1) {
        value = dir/2 * t * t * t + from;
      }
      else {
        t -= 2;
        value = dir/2 * (t * t * t + 2) + from;
      }
      return;
    }

      // Quartic
    case TWEEN_IN_QUART: {
      t /= duration;
      value = dir * t * t * t * t + from;
      return;
    }
      
    case TWEEN_OUT_QUART: {
      t /= duration;
      t--;
      value =  -dir * (t * t * t * t - 1) + from;
      return;
    }
      
    case TWEEN_INOUT_QUART: {
      t /= duration/2;
      if (t < 1) {
        value = dir/2 * t * t * t * t + from;
      }
      else {
        t -= 2;
        value = -dir/2 * ( t * t * t * t - 2) + from;
      }
      return;
    }

      // Quintic
    case TWEEN_IN_QUINT: {
      t /= duration;
      value = dir * t * t * t * t * t + from;
      return;
    }
      
    case TWEEN_OUT_QUINT: {
      t /= duration;
      t--;
      value = dir * (t * t * t * t * t + 1) + from;
      return;
    }
      
    case TWEEN_INOUT_QUINT: {
      t /= duration/2;
      if (t < 1) {
        value = dir/2 * t * t * t * t * t + from;
        
      }
      else {
        t -= 2;
        value = dir/2 * (t * t * t * t * t + 2) + from;
      }
      return;
    }

      // Exponential
    case TWEEN_IN_EXPO: {
      value = dir * powf(2, 10 * (t/duration - 1) ) + from;
      return;
    }
      
    case TWEEN_OUT_EXPO: {
      value = dir * ( -powf( 2, -10 * t/duration ) + 1 ) + from;
      return;
    }
      
    case TWEEN_INOUT_EXPO: {
      t /= duration/2;
      if (t < 1) {
        value = dir/2 * powf( 2, 10 * (t - 1) ) + from;
      }
      else {
        --t;
        value = dir/2 * (-powf( 2, -10 * t) + 2 ) + from;
      }
      return;
    }

      // Circular
    case TWEEN_IN_CIRC: {
      t /= duration;
      value = -dir * (sqrt(1 - t*t) - 1) + from;
      return;
    }
      
    case TWEEN_OUT_CIRC: {
      t /= duration;
      t--;
      value = dir * sqrt(1 - t*t) + from;
      return;
    }
      
    case TWEEN_INOUT_CIRC: {
      t /= duration/2;
      if (t < 1) {
        value = -dir/2 * (sqrt(1 - t*t) - 1) + from;
      }
      else {
        t -= 2;
        value = dir/2 * (sqrt(1 - t*t) + 1) + from;
      }
      return;
    }

      // Sinusoidal
    case TWEEN_IN_SINE: {
      value = -dir * cosf(t/duration * (HALF_PI)) + dir + from;
      return;
    }
      
    case TWEEN_OUT_SINE: {
      value = dir * sinf(t/duration * (HALF_PI)) + from;
      return;
    }
      
    case TWEEN_INOUT_SINE: {
      value = -dir/2 * (cosf(PI*t/duration) - 1) + from;
      return;
    }

      // Back
    case TWEEN_IN_BACK: {
      t /= duration;
      float s = 1.70158;
      value = dir * t * t * ((s+1)*t - s) + from;
      return;
    }
      
    case TWEEN_OUT_BACK: {
      float s = 1.70158;
      value = dir * ( (t=t/duration-1) * t * ( (s+1)*t + s) + 1) + from;
      return;
    }
      
    case TWEEN_INOUT_BACK: {
      float s = 1.70158; 
      if ((t /= duration/2) < 1) {
        value = dir/2 * (t * t * (((s*=(1.525))+1) * t - s)) + from;
      }
      else {
        value = dir/2 * ( (t-=2) * t * (((s*=(1.525))+1)*t + s) + 2) + from;
      }
      return;
    }

      // Elastic
    case TWEEN_IN_ELASTIC: {
      t /= duration;
      float p = duration * 0.3f;
      float a = dir;
      float s = p/4;
      float post_fix = a * pow(2,10 * (t-=1)); 
      value = -(post_fix * sin( (t * duration - s) * (2 * PI) / p )) + from;
      return;
    }
      
    case TWEEN_OUT_ELASTIC: {
      t /= duration;
      float p = duration *0.3f;
      float a = dir;
      float s = p/4;
      value = a * powf(2,-10 * t) * sin( (t * duration - s) * (2 * PI) / p ) + dir + from;
      return;
    }
      
    case TWEEN_INOUT_ELASTIC: {
      t /= duration / 2;
      float p = duration * (0.3f * 1.5f);
      float a = dir;
      float s = p/4;
      if (t < 1) {
        float pf = a * pow(2,10 * (t-=1));
        value = -0.5f * (pf * sin( (t * duration - s) * (2 * PI) / p )) + from;
      }
      else {
        t--;
        float pf = a * pow(2,-10 * t);
        value = pf * sin( (t * duration - s) * (2 * PI) / p ) * 0.5f + dir + from;
      }
      return;
    }
      
      // Bounce
    case TWEEN_IN_BOUNCE: {
      if ((t/=duration) < (1/2.75)) {
        value = dir * (7.5625 * t * t) + from;
      } 
      else if (t < (2/2.75)) {
        value = dir * (7.5625 * (t -= (1.5/2.75)) * t + 0.75f) + from;
      }
      else if (t < (2.5/2.75)) {
        value = dir * (7.5625 * (t -= (2.25/2.75)) * t + 0.9375f) + from;
      } 
      else {
        value = dir * (7.5625 * (t -= (2.625/2.75)) * t + 0.984375f) + from;
      }
      
      if(value >= (from+dir)) {
        value = (from+dir);
      }
      value = (dir - value) + from;
      return;
    }
      
    case TWEEN_OUT_BOUNCE: {
      if ((t/=duration) < (1/2.75)) {
        value = dir * (7.5625 * t * t) + from;
      } 
      else if (t < (2/2.75)) {
        value = dir * (7.5625 * (t -= (1.5/2.75)) * t + 0.75) + from;
      } 
      else if (t < (2.5/2.75)) {
        value = dir * (7.5625 * (t -= (2.25/2.75)) * t + 0.9375) + from;
      } 
      else {
        value = dir * (7.5625 * (t -= (2.625/2.75)) * t + 0.984375) + from;
      }
      
      if(value >= (from+dir)) {
        value = (from+dir);
      }
 
      return;
    }
      
    case TWEEN_INOUT_BOUNCE: {
      RX_ERROR("TODO TWEEN_INOUT_BOUNCE\n");
      return;
    }
      

    default:break;
  };


}

template<class T>
inline T Tween<T>::getValue() {
  return value;
}

// --------------------------------

template<class T>
TweenChain<T>::TweenChain() 
  :dx(0)
  ,ready(true)
{
}

template<class T>
void TweenChain<T>::push_back(Tween<T> tween) {
  tweens.push_back(tween);
}


template<class T>
void TweenChain<T>::start() {
  if(!ready) {
    return;
  }

  dx = 0;
  ready = false;

  tweens[dx].start();
}

template<class T>
void TweenChain<T>::update() {
  if(ready) {
    return;
  }

  tweens[dx].update();

  if(tweens[dx].ready) {
    ++dx;
    if(dx >= tweens.size()) {
      ready = true;
      dx = tweens.size() - 1; // set to the last; we return this value is getValue;
    }
    else {
      tweens[dx].start();
    }
  }
}
 
template<class T>
T TweenChain<T>::getValue() {
  return tweens[dx].getValue();
}


#endif



