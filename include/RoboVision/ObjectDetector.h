#include <opencv2/opencv.hpp>

#ifndef OBJECTDETECTOR_h
#define OBJECTDETECTOR_h

class ObjectDetector {
  public:
    vector<cv::Rectangle>scan(cv::Mat image);

  private:
};

#endif