#include <opencv2/opencv.hpp>

#ifndef SEGMENTER_h
#define SEGMENTER_h

class Segmenter {
  public:
    static st::vector<cv::Rect> yolo(cv::Mat input);

  private:
    static std::vector<cv::Rect> makeGrid(cv::Mat input, std::vector<cv::Rect> output);
};

#endif