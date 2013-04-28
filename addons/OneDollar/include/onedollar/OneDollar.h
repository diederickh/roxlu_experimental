// TODO - see Gesture::Length() this can be dramatically speed up

#ifndef ROXLU_ONE_DOLLAR_H
#define ROXLU_ONE_DOLLAR_H

#include <roxlu/math/Math.h>
#include <string>
#include <vector>
#include <fstream>

#define ERR_DOLLAR_NO_POINTS "The gesture doesn't have any points so we can't add it"
#define ERR_DOLLAR_LOAD_FILE "Error while trying to load the gestures file"
#define ERR_DOLLAR_SAVE_FILE "Error while trying to save the gestures file"
#define WARN_DOLLAR_SIZE_NOT_SAME "Cannot calculate the path distance as the number of points are not the same: %ld <> %ld"

struct OneDollarRect {
	double x, y, w, h;
};

struct Gesture {
  Gesture();
	Gesture(std::string name);
	
	void setName(std::string sName);
	void addPoint(double x, double y);
	void resample(int n);
	void scaleTo(double nSize = 250.0);
  void translate();
	void normalize(int numSamples);
	void reset();
	void rotateToZero();

	Vec2 centroid();
	OneDollarRect boundingBox();
  std::vector<Vec2> rotateBy(std::vector<Vec2> pts, double rad );
  
	double length();
	double indicativeAngle();	
	double distanceAtBestAngle(Gesture* gesture);
	double distanceAtAngle(double radians, Gesture* gesture);
	double pathDistance(std::vector<Vec2> pts, Gesture* gesture);

private:
  friend std::ostream& operator<<(std::ostream& os, const Gesture& g);
  friend std::istream& operator>>(std::istream& is, Gesture& g);

public:
  Vec2 center;
	std::string name;
	std::vector<Vec2> points;
	std::vector<Vec2> resampled_points;
	double golden_ratio;
	double angle_precision;
};


class OneDollar {
 public:
	OneDollar();
  ~OneDollar();
	
	bool addGesture(Gesture* gesture);
	bool save(std::string filename, bool datapath = false);
	bool load(std::string filename, bool datapath = false);
	
	Gesture* match(Gesture* Gesture, double* score);
	
 public:
	std::vector<Gesture*> gestures;
	double square_size;
	double half_diagonal;
	int num_samples;
	double angle_precision;
};



#endif
