#include <opencv2/opencv.hpp>
#include "Hand.h"

#ifndef HANDFEATUREEXTRACTOR_h
#define HANDFEATUREEXTRACTOR_h

// A way to detect hands using only Computer Vision.
class HandFeatureExtractor {
	public:
		HandFeatureExtractor();
    	void dump(Mat mat);
		bool detect(Mat image);

	private:
		Mat preProcess(Mat mat);
		vector<Point> getLargestContour(Mat mat);
		size_t getLargestContourIndex(vector<vector<Point>> countours);
		vector<Point> getFingerTips(vector<Point> contour, vector<Vec4i> defects, Rect bound);
		float getDistance(Point p1, Point p2);
		float getTriangleArea(Point p1, Point p2, float defectDepth);
		float innerAngle (float px1, float py1, float px2, float py2, float cx1, float cy1);
		Mat _rawImage;
    	Hand _hand;
		bool _validHand;


		// TODO: pull out threshold values into config file.
		int _minY;
		int _maxY;
		int _minCr;
		int _maxCr;
		int _minCb;
		int _maxCb;
		int _blurSize;
		int _elementSize;
};

#endif
