#include <opencv2/opencv.hpp>

#include "RoboVision/DetectedObject.h"

#ifndef OBJECTDETECTOR_h
#define OBJECTDETECTOR_h

class ObjectDetector {
  public:
    ObjectDetector();
    std::vector<DetectedObject> scan(cv::Mat image);

  private:
    std::vector<std::string> loadClassesFromFile(std::string filePath);
};

#endif