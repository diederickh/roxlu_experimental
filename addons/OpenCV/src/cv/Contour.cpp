#include <cv/Contour.h>
#include <roxlu/core/Utils.h>
#include <roxlu/core/Log.h>

namespace roxlu {
  Contour::Contour()
    :image_w(0)
    ,image_h(0)
    ,image_components(0)
  {
  }

  Contour::~Contour() {
    reset();
  }
  
  bool Contour::loadImage(std::string filename, bool datapath) {
    if(datapath) {
      filename = rx_to_data_path(filename);
    }
    /*
    mat = cv::imread(filename.c_str(), CV_LOAD_IMAGE_COLOR);
    if(!mat.data) {
      return false;
    }
    */
    return true;
  }

  bool Contour::setPixels(unsigned char* pixels, int imageW, int imageH, int imageComponents) {
    image_w = imageW;
    image_h = imageH;
    image_components = imageComponents;

    if(image_components == 3) {
      cv::Mat tmp(image_h, image_w, CV_8UC3, pixels); // CV_8U
      mat_original = tmp.clone();
      cvtColor(mat_original, mat_gray, CV_BGR2GRAY);
    }
    else if(image_components == 4) {
      cv::Mat tmp(image_h, image_w, CV_8UC4, pixels); // CV_8U
      mat_original = tmp.clone();
      cvtColor(mat_original, mat_gray, CV_RGBA2GRAY);
    }
    else {
      RX_ERROR("CANNOT HANDLE COMPONENTS! %d", imageComponents);
      return false;
    }
    
    cv::Mat tmp(image_h, image_w, mat_gray.type());
    cv::threshold(mat_gray, tmp, 100,255, cv::THRESH_BINARY);
    mat_bw = tmp.clone();

    return true;
  }

  void Contour::findContours() {
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
    image_w = 0;
    image_h = 0;
    image_components = 0;    
    contours.clear();
    hierarchy.clear();
  }
} // roxlu

