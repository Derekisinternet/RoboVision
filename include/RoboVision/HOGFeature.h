#include <opencv2/opencv.hpp>

#ifndef HOGFEATURE_h
#define HOGFEATURE_h

using namespace cv;

class HOGFeature {
    public:
        HOGFeature();
        Mat extract(Mat mat);

    private:
        void preProcess(Mat mat);
        Mat deskew(Mat& img);
        int _histSize
        int _cellSize;
};

#endif
