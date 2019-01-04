#include <opencv2/opencv.hpp>

#ifndef OBJECTDETECTOR_h
#define OBJECTDETECTOR_h

class ObjectDetector {
  public:
    ObjectDetector();
    std::vector<cv::Rect>scan(cv::Mat image);

  private:
};

#endif