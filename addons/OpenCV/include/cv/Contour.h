#ifndef ROXLU_CV_CONTOUR_H
#define ROXLU_CV_CONTOUR_H

#include <opencv2/core/core.hpp>
#include <opencv2/imgproc/imgproc.hpp>
#include <vector>
#include <string>
#include <image/Image.h>
#include <roxlu/core/Constants.h>
#include <roxlu/math/Math.h>

#define RX_CV_UNSUPPORTED_PIX_FMT "Unsupported pixel format"

namespace roxlu { 
  class Contour {
  public:
    Contour();
    ~Contour();
    //    bool loadImage(std::string filename, bool datapath = false);
    
    //bool setPixels(unsigned char* pixels, int imageW, int imageH, int pixelFormat);  /* set the pixels on which we need to find the contours, pixelformat: RX_FMT_RGB24, RX_FMT_RGB32, etc.., see roxlu/core/Constants.h */
    // void blur(int sizeX = 3, int sizeY = 3);
    
    bool findContours(Image& img);
    bool findContours(unsigned char* pixels, unsigned int w, unsigned int h, unsigned int pixfmt);
    bool findContours();
    bool setPixels(unsigned char* pixels, unsigned int w, unsigned int h, unsigned int pixfmt);

    unsigned int getWidth();
    unsigned int getHeight();
    unsigned char* getPixels();
    unsigned int getPixelFormat() const;  

    cv::Point getCentroid();
    std::vector<Vec2> getContourPoints(); /* returns all contour points */
    void reset(); /* resets all state as if we were just created */
  public:
    //cv::Mat mat_original;
    cv::Mat thresholded_img;
    cv::Mat gray_img;
    //cv::Mat mat_bw;
    /*
    int image_w; 
    int image_h;
    int image_components;
    */

    std::vector<std::vector<cv::Point> > contours;
    std::vector<cv::Vec4i> hierarchy;
  };

  /*
  inline void Contour::blur(int sizeX, int sizeY) {
    cv::Size s(sizeX, sizeY);
    cv::blur(mat_gray, mat_gray, s);
  }
  */

  inline unsigned int Contour::getWidth() {
    return thresholded_img.size().width;
  }

  inline unsigned int Contour::getHeight() {
    return thresholded_img.size().height;
  }

  inline unsigned char* Contour::getPixels() {
    return thresholded_img.data;
  }

  inline unsigned int Contour::getPixelFormat() const {
    return RX_FMT_GRAY8;
  }

} // namespace
#endif
