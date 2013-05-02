/*
 Protractor gesture recognition

 ------------------------------
 Implementation of the amazingly fast and simple but extremely acurate gesture
 recognizer as described in: https://www.lri.fr/~anab/teaching/M2R-2011/5-gestures/protractor-chi2010.pdf

 [ ] Todo: Implement rotation variant 

 */
#ifndef ROXLU_PROTRACTOR_H
#define ROXLU_PROTRACTOR_H

#include <roxlu/math/Math.h>
#include <vector>
#include <deque>
#include <string>

#define ERR_PROT_VECTOR_SIZE_MISMATCH "The size of rotated_points is not the same as our size"
#define ERR_PROT_NO_GESTURES "No gestures"
#define ERR_PROT_SAVE_OPEN "Cannot open the file for writing"
#define ERR_PROT_LOAD_OPEN "Cannot open the file for reading"

namespace protractor {

  class Gesture {

  public:
    Gesture(std::string name);
    void addPoint(float x, float y);                         /* add a raw input point */
    void resample(int n = 16);                               /* simplifies the path and reduces the number of points */
    void vectorize(bool angleSensitive = true);              /* generate a vector representation from the gesture. It uses the reampled points to generate a normalized vector with length 2n */
    void reset();                                            /* resets all members */
    float length();                                          /* returns the path length */
    Vec2 calculateCentroid();                                /* calculates the centroid of the gesture or returns the previously calculated centroid. */
    void translate(Vec2 point);                              /* translates every point byt `point`. */
    float calculateCosineDistance(Gesture* other);           /* calculates the cosine distance between `this` and the given gesture */

  private:
    friend std::ostream& operator<<(std::ostream& os, const Gesture& g); /* output stream operator, used to save the gesture to a file */
    friend std::istream& operator>>(std::istream& is, Gesture& g); /* output stream operator, used to save the gesture to a file */
    
  public:
    std::string name;                                         /* name of the gesture */
    Vec2 centroid;                                            /* centroid of the gesture */
    std::deque<Vec2> points;                                  /* we use a std::deque here, instead of a std::vector, because this makes it possible (aka. faster) to do continuous testing with a source gesture. */
    std::vector<Vec2> resampled_points;                       /* the input `points` member is the complete stroke, but the `resampled_points` is a shortened (just 16 by default) vector */
    std::vector<Vec2> rotated_points;                         /* rotated towards indicative angle */
  };

  class Protractor {

  public:
    Protractor();
    ~Protractor();                                            /* destructor, will free all added gestures */
    void addGesture(Gesture* gesture);                        /* This will first resample the gesture + vectorize, then it will be added */
    Gesture* match(Gesture* in, float* score);                /* match an input gesture against all added gestures. */

    bool save(std::string filename, bool datapath = false);
    bool load(std::string filename, bool datapath = false);

  public:
    std::vector<Gesture*> gestures;
  
  };

} // namespace 

#endif
