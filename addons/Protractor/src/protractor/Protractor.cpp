#include <assert.h>
#include <fstream>
#include <roxlu/core/Utils.h>
#include <roxlu/core/Log.h>
#include <protractor/Protractor.h>

namespace protractor {

  Gesture::Gesture(std::string name) 
    :name(name)
  {
  }

  void Gesture::resample(int n) {
    assert(points.size());

    float I = length()/(n - 1);
    float D = 0;

    resampled_points.clear();
    resampled_points.push_back(points[0]);

    for(int i = 1; i < points.size(); ++i) {

      Vec2 curr = points[i];
      Vec2 prev = points[i-1];
      Vec2 dir = prev - curr;
      float d = dir.length();

      if( (D + d) >= I) {
        float qx = prev.x + ((I-D)/d) * (curr.x - prev.x);
        float qy = prev.y + ((I-D)/d) * (curr.y - prev.y);
        Vec2 resampled(qx, qy);
        resampled_points.push_back(resampled);
        points.insert(points.begin() + i, resampled); 
        D = 0.0;
      }
      else {
        D += d;
      }
    }

    while(resampled_points.size() <= (n - 1)) {
      resampled_points.push_back(points.back());
    }

    if(resampled_points.size() > n) {
      resampled_points.erase(resampled_points.begin(), resampled_points.begin()+n);
    }
  }

  Vec2 Gesture::calculateCentroid() {
    assert(resampled_points.size());

    centroid.set(0.0, 0.0);

    std::vector<Vec2>::iterator it = resampled_points.begin();
    while(it != resampled_points.end()) {
      centroid += *it;
      ++it;
    }

    centroid /= resampled_points.size();
    return centroid;
  }

  void Gesture::translate(Vec2 p) {
    assert(resampled_points.size());

    std::vector<Vec2>::iterator it = resampled_points.begin();
    while(it != resampled_points.end()) {
      Vec2& v = *it;
      v += p;
      ++it;
    }
  }

  void Gesture::vectorize(bool angleSensitive) {
    assert(resampled_points.size());

    rotated_points.clear();

    Vec2 center = calculateCentroid();
    translate(-center);
    float indicative_angle = atan2(resampled_points[0].y, resampled_points[0].x);
    float delta = -indicative_angle;
    
    if(angleSensitive) {
      float base_orientation = (PI * 0.25f) * floor( (indicative_angle + PI / 8.0f) / (PI / 4.0f));
      delta = base_orientation - indicative_angle;
    }
      
    Vec2 rotated_point;
    float cosa = cos(delta);
    float sina = sin(delta);
    float sum = 0.0f;

    for(std::vector<Vec2>::iterator it = resampled_points.begin(); it != resampled_points.end(); ++it) {
      Vec2& p = *it;
      
      rotated_point.x = p.x * cosa - p.y * sina;
      rotated_point.y = p.y * cosa + p.x * sina;
      rotated_points.push_back(rotated_point);

      sum += (rotated_point.x * rotated_point.x) + (rotated_point.y * rotated_point.y);
    }

    sum = sqrt(sum);
    for(std::vector<Vec2>::iterator it = rotated_points.begin(); it != rotated_points.end(); ++it) {
      (*it) /= sum;
    }
  }

  float Gesture::calculateCosineDistance(Gesture* other) {
    assert(rotated_points.size());

    if(other->rotated_points.size() != rotated_points.size()) {
      RX_ERROR(ERR_PROT_VECTOR_SIZE_MISMATCH);
      return -1.0f;
    }
    
    float a = 0.0f;
    float b = 0.0f;
    
    for(size_t i = 0; i < rotated_points.size(); ++i) {
      Vec2& va = rotated_points[i];
      Vec2& vb = other->rotated_points[i];
      
      a += va.x * vb.x + va.y * vb.y;
      b += va.x * vb.y - va.y * vb.x;
    }

    float angle = atan(b/a);
    return acos(a * cos(angle) + b * sin(angle));
  }

  float Gesture::length() {
    float d = 0;
    for(int i = 1; i < points.size(); ++i) {
      d += (points[i-1] - points[i]).length();
    }
    return d;
  }
   
  void Gesture::reset() {
    points.clear();
    resampled_points.clear();
    rotated_points.clear();
  }

  void Gesture::addPoint(float x, float y) {
    points.push_back(Vec2(x,y));
  }

  std::ostream& operator <<(std::ostream& os, const Gesture& gesture) {
    os << gesture.name.c_str() << " ";
    for(std::vector<Vec2>::const_iterator it = gesture.rotated_points.begin(); it != gesture.rotated_points.end(); ++it) {
      os << (*it).x << ";" << (*it).y << ";";
    }
    return os;
  }

  std::istream& operator >>(std::istream& is, Gesture& gesture) {
    Vec2 p; 
    is >> gesture.name;
    while(is) {
      is >> p.x; is.ignore(1); 
      is >> p.y; is.ignore(1);
      if(is) {
        gesture.rotated_points.push_back(p);
      }
    }
    return is;
  }

  // ----------------------------------------------------

  Protractor::Protractor() {
  }

  Protractor::~Protractor() {
    for(std::vector<Gesture*>::iterator it = gestures.begin(); it != gestures.end(); ++it) {
      delete *it;
    }
    gestures.clear();
  }

  void Protractor::addGesture(Gesture* g) {
    g->resample();
    g->vectorize();
    gestures.push_back(g);
  }

  Gesture* Protractor::match(Gesture* in, float* score) {
    Gesture* matched = NULL;
    float max_score = 0.0f;
    float s = 0.0f;
    float distance = 0.0f;

    // @todo add a flag if the input gesture needs to be vectorized
    in->resample();
    in->vectorize();

    for(std::vector<Gesture*>::iterator it = gestures.begin(); it != gestures.end(); ++it) {
      Gesture* other = *it;
      distance = in->calculateCosineDistance(other);
      //RX_VERBOSE("DIST: %f (%s)", distance, other->name.c_str());
      s = 1.0f/distance;

      if(s > max_score) {
        max_score = s;
        matched = other;
      }
    }

    *score = s;
    return matched;
  }

  bool Protractor::save(std::string filename, bool datapath) {
    if(!gestures.size()) {
      RX_ERROR(ERR_PROT_NO_GESTURES);
      return false;
    }
    
    if(datapath) {
      filename = rx_to_data_path(filename);
    }

    std::ofstream ofs(filename.c_str(), std::ios::out | std::ios::trunc);
    if(!ofs.is_open()) {
      RX_ERROR(ERR_PROT_SAVE_OPEN);
      return false;
    }

    for(std::vector<Gesture*>::iterator it = gestures.begin(); it != gestures.end(); ++it) {
      Gesture& g = **it;
      ofs << g << std::endl;
    }

    ofs.close();
    return true;
  }

  bool Protractor::load(std::string filename, bool datapath) {
    if(datapath) {
      filename = rx_to_data_path(filename);
    }

    std::ifstream ifs(filename.c_str());
    if(!ifs.is_open()) {
      RX_ERROR(ERR_PROT_LOAD_OPEN);
      return false;
    }

    std::string line;
    while(std::getline(ifs, line)) {
      std::stringstream ss;
      ss << line;
      Gesture* g = new Gesture("");
      ss >> *g;
      gestures.push_back(g);
    }
    
    return true;
  }

};
