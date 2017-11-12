// Histogram of Oriented Gradients Feature Extractor
#include <opencv2/opencv.hpp>
#include "RoboVision/HOGFeature.h"

using namespace cv;

HOGFeature::HOGFeature() {
	_histSize = 9; // number of histogram bins
	_cellSize = 8; // block size for normalization
}

Mat HOGFeature::extract(Mat img) {
	Mat mat = Mat(img);
    preProcess(mat);

	// Calculate gradients gx, gy
	Mat gx, gy; 
	Sobel(mat, gx, CV_32F, 1, 0, 1);
	Sobel(mat, gy, CV_32F, 0, 1, 1);
    // C++ Calculate gradient magnitude and direction (in degrees)
	Mat mag, angle; 
	cartToPolar(gx, gy, mag, angle, 1);

	//TODO: create histogram of image.

	return mat;
}

void HOGFeature::preProcess(Mat mat) {
    //TODO: resize image to standard dimensions

    mat.convertTo(mat, CV_32F, 1/255.0);

    
}

