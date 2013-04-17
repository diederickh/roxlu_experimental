/*

  This code is just a test.. do not use :-) 

 */
#include <cv/Contour.h>
#include <roxlu/core/Utils.h>
#include <roxlu/core/Log.h>

namespace roxlu {
  Contour::Contour() {
  }

  Contour::~Contour() {
    reset();
  }
  
  bool Contour::findContours(Image& img) {

    if(!setPixels(img.getPixels(), img.getWidth(), img.getHeight(), img.getPixelFormat())) {
      return false;
    }

    return findContours();
  }

  bool Contour::findContours(unsigned char* pixels, unsigned int w, unsigned int h, unsigned int pixfmt) {

    if(!setPixels(pixels, w, h, pixfmt)) {
      return false;
    }

    return findContours();
  }

  bool Contour::setPixels(unsigned char* pixels, unsigned int w, unsigned int h, unsigned int pixfmt) {

    if(pixfmt == RX_FMT_RGB24) {
      cv::Mat tmp(h, w, CV_8UC3, pixels); 
      cvtColor(tmp, gray_img, CV_RGB2GRAY);
    }
    else if(pixfmt == RX_FMT_RGBA32) {
      cv::Mat tmp(h, w, CV_8UC4, pixels); 
      cvtColor(tmp, gray_img, CV_RGBA2GRAY);
    }
    else if(pixfmt == RX_FMT_GRAY8) {
      cv::Mat tmp(h, w, CV_8UC1, pixels); 
      gray_img = tmp.clone();
    }
    else {
      RX_ERROR(RX_CV_UNSUPPORTED_PIX_FMT);
      return false;
    }

    cv::threshold(gray_img, thresholded_img, 100, 255, cv::THRESH_BINARY);

    return true;
  }

  bool Contour::findContours() {
    reset();
    cv::findContours(thresholded_img, contours, hierarchy, CV_RETR_TREE, CV_CHAIN_APPROX_NONE, cv::Point(0, 0) );

    return true;
  }

  cv::Point Contour::getCentroid() {
    float xx = 0.0f;
    float yy = 0.0f;
    float count = 0.0f;

    for(std::vector<std::vector<cv::Point> >::iterator it = contours.begin(); it != contours.end(); ++it) {
      for(std::vector<cv::Point>::iterator pit = (*it).begin(); pit != (*it).end(); ++pit) {
        cv::Point p = *pit;
        xx += p.x;
        yy += p.y;
        count++;
      }
    }

    cv::Point centroid(xx/count, yy/count);
    return centroid;
  }

  std::vector<Vec2> Contour::getContourPoints() {
    std::vector<Vec2> points;

    for(std::vector<std::vector<cv::Point> >::iterator it = contours.begin(); it != contours.end(); ++it) {
      for(std::vector<cv::Point>::iterator pit = (*it).begin(); pit != (*it).end(); ++pit) {
        cv::Point& point = *pit;
        points.push_back(Vec2(point.x, point.y));
      }
    }

    return points;
  }

  void Contour::reset() {
    contours.clear();
    hierarchy.clear();
  }


    /*
    contours.clear();
    hierarchy.clear();


    bool use_canny = false;
    if(use_canny) {
      cv::Mat canny_output;
      int thresh = 5;

      cv::Canny(mat_gray, canny_output, thresh, thresh*2, 3 );

      //cv::findContours(canny_output, contours, hierarchy,  CV_RETR_EXTERNAL, CV_CHAIN_APPROX_SIMPLE, cv::Point(0, 0) );
      //cv::findContours(canny_output, contours, hierarchy,  CV_RETR_LIST, CV_CHAIN_APPROX_SIMPLE, cv::Point(0, 0) );
      //cv::findContours(canny_output, contours, hierarchy,  CV_RETR_LIST, CV_CHAIN_APPROX_NONE, cv::Point(0, 0) );
      cv::findContours(canny_output, contours, hierarchy, CV_RETR_TREE, CV_CHAIN_APPROX_SIMPLE, cv::Point(0, 0) );
    }          
    else {
      // awesome result for edge rotoscoping...
      cv::findContours(mat_bw, contours, hierarchy, CV_RETR_TREE, CV_CHAIN_APPROX_NONE, cv::Point(0, 0) );
    }
    */



} // roxlu

