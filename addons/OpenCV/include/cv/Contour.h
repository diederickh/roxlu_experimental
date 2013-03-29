#ifndef ROXLU_CV_CONTOUR_H
#define ROXLU_CV_CONTOUR_H

#include <opencv2/core/core.hpp>
#include <opencv2/imgproc/imgproc.hpp>
#include <vector>
#include <string>
#include <roxlu/math/Math.h>

namespace roxlu { 
  class Contour {
  public:
    Contour();
    ~Contour();
    bool loadImage(std::string filename, bool datapath = false);
    bool setPixels(unsigned char* pixels, int imageW, int imageH, int imageComponents); 
    void blur(int sizeX = 3, int sizeY = 3);
    void findContours();
    cv::Point getCentroid();
    std::vector<Vec2> getContourPoints(); /* returns all contour points */
    void reset(); /* resets all state as if we were just created */
  public:
    cv::Mat mat_original;
    cv::Mat mat_gray;
    cv::Mat mat_bw;
    int image_w; 
    int image_h;
    int image_components;

    std::vector<std::vector<cv::Point> > contours;
    std::vector<cv::Vec4i> hierarchy;
  };

  inline void Contour::blur(int sizeX, int sizeY) {
    cv::Size s(sizeX, sizeY);
    cv::blur(mat_gray, mat_gray, s);
  }

} // namespace
#endif
