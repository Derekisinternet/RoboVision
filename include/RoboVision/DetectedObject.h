#include <opencv2/opencv.hpp>

#ifndef DETECTEDOBJECT_h
#define DETECTEDOBJECT_h


// A standard, agnostic data structure for identified objects in an image
struct DetectedObject {
  cv::Rect location;
  std::string classLabel;
  float confidence;
};

#endif