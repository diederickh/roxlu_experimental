#include <onedollar/OneDollar.h>
#include <roxlu/core/Log.h>
#include <cmath>

Gesture::Gesture(std::string name) 
  :name(name)
{
  reset();
}

Gesture::Gesture() {
  reset();
}

void Gesture::setName(std::string n) {
  name = n;
}
	
void Gesture::addPoint(double x, double y) {
  points.push_back(Vec2(x, y));
}

void Gesture::resample(int n) {
  double I = length()/(n - 1);
  double D = 0;

  for(int i = 1; i < points.size(); ++i) {

    Vec2 curr = points[i];
    Vec2 prev = points[i-1];
    Vec2 dir = prev - curr;
    double d = dir.length();

    if( (D + d) >= I) {

      double qx = prev.x + ((I-D)/d) * (curr.x - prev.x);
      double qy = prev.y + ((I-D)/d) * (curr.y - prev.y);
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
	
Vec2 Gesture::centroid() {	
  double x = 0;
  double y = 0;
  std::vector<Vec2>::iterator it = resampled_points.begin();

  while(it != resampled_points.end()) {
    x += (*it).x;
    y += (*it).y;
    ++it;
  }

  x /= resampled_points.size();
  y /= resampled_points.size();

  Vec2 tmp(x,y);
  return tmp;
}
	
double Gesture::length() {
  double d = 0;
  for(int i = 1; i < points.size(); ++i) {
    d += (points[i-1] - points[i]).length();
  }
  return d;
}	
	
double Gesture::indicativeAngle() { 
  Vec2 c = centroid(); // TODO: optimize
  double angle = atan2(c.y - resampled_points[0].y, c.x - resampled_points[0].x);
  return angle;
}

void Gesture::rotateToZero() {
  double angle = indicativeAngle();
  resampled_points = rotateBy(resampled_points, -angle);
}
	
std::vector<Vec2> Gesture::rotateBy(std::vector<Vec2> pts, double radians) {
  std::vector<Vec2> rotated;
  Vec2 c = centroid(); // TODO: optimize (we only need to set the centroid once);
  center = c; // TODO: optimize
  double cosa = cos(radians);
  double sina = sin(radians);

  std::vector<Vec2>::iterator it = pts.begin();
  while(it != pts.end()) {
    Vec2 v = (*it);
    double dx = v.x - c.x;
    double dy = v.y - c.y;
    v.x = dx * cosa - dy * sina + c.x;
    v.y = dx * sina + dy * cosa + c.y;
    rotated.push_back(v);
    ++it;
  }
  return rotated;
}
	
void Gesture::scaleTo(double size) {
  OneDollarRect rect = boundingBox();
  std::vector<Vec2>::iterator it = resampled_points.begin();
  while(it != resampled_points.end()) {
    Vec2& v = (*it);
    v.x = v.x * (size/rect.w);
    v.y = v.y * (size/rect.h);
    ++it;
  };
}
	
// translates to origin.
void Gesture::translate() {
  Vec2 c = centroid(); //TODO: optimize
  std::vector<Vec2>::iterator it = resampled_points.begin();
  while(it != resampled_points.end()) {
    Vec2& v = (*it);
    v.x = v.x - c.x;
    v.y = v.y - c.y;
    ++it;
  };
}
	
OneDollarRect Gesture::boundingBox() {

  double min_x = FLT_MAX, min_y = FLT_MAX, max_x = FLT_MIN, max_y = FLT_MIN;

  std::vector<Vec2>::const_iterator it = resampled_points.begin();

  while(it != resampled_points.end()) {
    Vec2 v = (*it);

    if(v.x < min_x) {
      min_x = v.x;
    }

    if(v.x > max_x) {
      max_x = v.x;
    }

    if(v.y < min_y) {
      min_y = v.y;
    }

    if(v.y > max_y) {
      max_y = v.y;
    }
    ++it;
  }

  OneDollarRect rect;
  rect.x = min_x;
  rect.y = min_y;
  rect.w = (max_x - min_x);
  rect.h = (max_y - min_y);
  return rect;
}
	
double Gesture::distanceAtBestAngle(Gesture* gesture) {
		double angle_range = PI;
		double start_range = -angle_range;
		double end_range = angle_range;
		double x1 = golden_ratio * start_range + (1.0 - golden_ratio) * end_range;
		double f1 = distanceAtAngle(x1, gesture);
		double x2 = (1.0 - golden_ratio) * start_range + golden_ratio * end_range;
		double f2 = distanceAtAngle(x2, gesture);

		while(abs(end_range - start_range) > angle_precision) {	
			if(f1 < f2) {
				end_range = x2;
				x2 = x1;
				f2 = f1;
				x1 =  golden_ratio * start_range + (1.0 - golden_ratio) * end_range;
				f1 = distanceAtAngle(x1, gesture);
			}
			else {
				start_range = x1;
				x1 = x2;
				f1 = f2;
				x2 = (1.0 - golden_ratio) * start_range + golden_ratio * end_range;
				f2 = distanceAtAngle(x2, gesture); 
			}
		}

		return std::min(f1, f2);
	}
	
double Gesture::distanceAtAngle(double radians, Gesture* gesture) {
  std::vector<Vec2> points_tmp = resampled_points;
  points_tmp = rotateBy(points_tmp, radians);
  return pathDistance(points_tmp, gesture);
}
	
// distance between two paths.
double Gesture::pathDistance(std::vector<Vec2> pts, Gesture* gesture) {

  if(pts.size() != gesture->resampled_points.size()) {
    RX_WARNING(WARN_DOLLAR_SIZE_NOT_SAME, pts.size(), gesture->resampled_points.size());
    return -1.0;
  }

	double d = 0;
  for(int i = 0; i < resampled_points.size(); ++i) {
			 d += (pts[i] - gesture->resampled_points[i]).length();
  }

  return d/pts.size();
}
	
	
void Gesture::normalize(int numSamples) {
  resample(numSamples);
  rotateToZero();
  scaleTo();
  translate();
}
	
void Gesture::reset() {
  golden_ratio = 0.5 * (-1.0 + sqrt(5.0));
  angle_precision = 1.0;
  resampled_points.clear();
  points.clear();
  center.set(0,0);
}

std::ostream& operator <<(std::ostream& os, const Gesture& gesture) {

  os << gesture.name.c_str() << " ";

  std::vector<Vec2>::const_iterator it = gesture.resampled_points.begin();
  while(it != gesture.resampled_points.end()) {
    os << (*it).x << ';' << (*it).y << ';';
    ++it;
  };

  return os;
}
	
std::istream& operator >>(std::istream& is, Gesture& gesture) {

  double x,y;
  std::istringstream ss;

  is >> gesture.name;

  while(is) {	
    is >> x; is.ignore(1); is >> y; is.ignore(1);
    if(is) {
      gesture.resampled_points.push_back(Vec2(x,y));
    }
  }

  return is;
}

// -------------------------------------------------------------------------

OneDollar::OneDollar() 
		:num_samples(64)
		,square_size(250.0)
		,angle_precision(1.0)
{
		half_diagonal = 0.5 * sqrt((square_size*square_size) + (square_size*square_size));
}

OneDollar::~OneDollar() {
  for(std::vector<Gesture*>::iterator it = gestures.begin(); it != gestures.end(); ++it) {
    delete *it;
  }
}
	
bool OneDollar::addGesture(Gesture* gesture) {
  if(!gesture->points.size()) {
    RX_ERROR(ERR_DOLLAR_NO_POINTS);
    return false;
  }

  gesture->angle_precision = angle_precision;
  gesture->normalize(num_samples);
  gestures.push_back(gesture);

  return true;
}

	
bool OneDollar::save(std::string filename, bool datapath) {

  if(datapath) {
    filename = rx_to_data_path(filename);
  }

  std::ofstream out_file(filename.c_str(), std::ios::out | std::ios::trunc);
  if(!out_file.is_open()) {
    RX_ERROR(ERR_DOLLAR_SAVE_FILE);
    return false;
  }

  std::vector<Gesture*>::iterator it = gestures.begin();
  while(it != gestures.end()) {
    Gesture* g = *it;
    out_file << *g << std::endl;
    ++it;
  }
  out_file.close();
  return true;
}

	
bool OneDollar::load(std::string filename, bool datapath) {

  if(datapath) {
    filename = rx_to_data_path(filename);
  }

  std::ifstream in_file(filename.c_str());
  if(!in_file) {
    RX_ERROR(ERR_DOLLAR_LOAD_FILE);
    return false;
  }

  std::stringstream ss;
  ss << in_file.rdbuf();
  in_file.close();

  std::string line;

  while(getline(ss, line)) {	

    std::stringstream iss;
    iss << line;

    Gesture* gesture = new Gesture();
    iss >> *(gesture);

    gestures.push_back(gesture);
  }

  return true;
}
	


Gesture* OneDollar::match(Gesture* gesture, double* score) {
  // TODO: add a flag to check if the gesture is already normalized
  gesture->normalize(num_samples);
  
  double min_dist = FLT_MAX;
  Gesture* found_gesture = NULL;
  std::vector<Gesture*>::const_iterator it = gestures.begin();

  while(it != gestures.end()) {
    double dist = (*it)->distanceAtBestAngle(gesture);
    if(dist > 0 && dist < min_dist) {
      min_dist = dist;
      found_gesture = (*it);
    }
    ++it;
  }

  *score = 1.0 - (min_dist/half_diagonal);
  return found_gesture;
}
