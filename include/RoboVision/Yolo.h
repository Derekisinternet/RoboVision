#include <opencv2/opencv.hpp>

#ifndef YOLO_h
#define YOLO_h

class Yolo {
  public:
    Yolo();
    std::vector<cv::Rect> yolo(cv::Mat input);

  private:
    int subSections;
    int boundingBoxes;
    struct boundingBox {
      cv::Point center;
      int width;
      int height;
      double confidence;
    };
    struct gridCell {
      cv::Rect location;
      std::vector<struct boundingBox> boundingBoxes;
      std::vector<double> conditionalClassProbs;
    };

    std::vector<cv::Rect> makeBoundingBoxes(cv::Rect input, std::vector<cv::Rect> output);
    std::vector<gridCell> makeGrid(cv::Mat input, int gridSize);
};

#endif