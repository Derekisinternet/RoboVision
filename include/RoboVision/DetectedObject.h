#include <opencv2/opencv.hpp>

#ifndef DETECTEDOBJECT_h
#define DETECTEDOBJECT_h


// A standard, agnostic data structure for identified objects in an image
struct DetectedObject {
  cv::Rect location;
  std::string classLabel;
  float confidence;

  // constructors
  DetectedObject();
  DetectedObject(std::string label, cv::Rect rect, float conf) 
    : classLabel(label)
    , location(rect)
    , confidence(conf)
  { }

  // returns the center point of the object's bounding box
  cv::Point getCenter() {
    int x = location.x + (location.width / 2);
    int y = location.y + (location.height /2);
    return cv::Point(x,y);
  };

};

#endif