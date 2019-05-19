#include <opencv2/opencv.hpp>

#ifndef DISPLAY_h
#define DISPLAY_h

class Display {
  private:
  static std::vector<cv::Scalar> permutate(int i);
  static std::vector<cv::Scalar> permutate(int i, int j);

  public:
    Display();
    ~Display();
    static std::vector<cv::Scalar> colorVector(int length);
};

#endif