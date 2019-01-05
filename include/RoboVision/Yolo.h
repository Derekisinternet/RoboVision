#include <opencv2/opencv.hpp>

#ifndef YOLO_h
#define YOLO_h

class Yolo {
  public:
    static st::vector<cv::Rect> yolo(cv::Mat input);

  private:
    static std::vector<cv::Rect> makeGrid(cv::Mat input, std::vector<cv::Rect> output);
};

#endif