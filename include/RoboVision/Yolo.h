#include <opencv2/opencv.hpp>

#ifndef YOLO_h
#define YOLO_h

class Yolo {
  private:
    int subSections;
    int boundingBoxes;
    float minConfidence;
    float nonMaxSuppression;
    std::vector<std::string> classNames;
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
    struct object {
      cv::Rect location;
      char classLabel;
    };
    // divides the input image into a grid of (gridSize x gridSize) cells
    std::vector<gridCell> makeGrid(cv::Mat input, int gridSize);
    // predicts bounding boxes
    std::vector<cv::Rect> predictBoundingBoxes(cv::Rect input, std::vector<cv::Rect> output);

  public:
    Yolo(std::vector<std::string> classList);
    std::vector<object> yolo(cv::Mat input);
};

#endif