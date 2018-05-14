#include <opencv2/opencv.hpp>

#ifndef HAND_h
#define HAND_h

using namespace cv;
using namespace std;

class Hand {
    public:
        Hand();
        Hand(vector<Point> contour, vector<int> convexIndexes, vector<Vec4i> defects);
        vector<Point> getFingertips(Mat = Mat());
        void debugPrint(Mat mat);

    private:
        vector<Point> _contour;
        vector<int> _convexIndexes;
        vector<Vec4i> _convexDefects;
        float innerAngle (Point p1, Point p2, Point defect);
        float getDistance(Point p1, Point p2);
        float getTriangleArea(Point p1, Point p2, float defectDepth);

};

#endif
