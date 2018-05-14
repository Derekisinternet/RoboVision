#include <opencv2/opencv.hpp>
#include "RoboVision/Hand.h"

using namespace cv;
using namespace std;


Hand::Hand(){}

Hand::Hand(vector<Point> contour,
           vector<int> convexIndexes,
           vector<Vec4i> defects)
           : _contour(contour),
            _convexIndexes(convexIndexes),
            _convexDefects(defects) {}

vector<Point> Hand::getFingertips(Mat mat) {
    vector<Point> validPoints;
    Point indexFinger;
    Point thumb;
    float biggestDefectArea = 0;

    for (size_t i = 0; i < _convexDefects.size(); i++) {
        Point p1 = _contour[_convexDefects[i][0]]; //start
        Point p2 = _contour[_convexDefects[i][1]]; //end
        Point p3 = _contour[_convexDefects[i][2]]; // pt furthest from convex
        double convexLength = getDistance(p1, p3);
        float depth = _convexDefects[i][3]/256.0; // dist. from edge to p3
        double inAngle = innerAngle(p1, p2, p3);
       
        if (inAngle < 120
            // TODO: calibrate these thresholds
            // && convexLength > 1
            // && depth > (0.1 * convexLength)
            )
        {
            validPoints.push_back(p1);
            if (!mat.empty()) {
                line(mat, p1, p3, Scalar(255,0,0), 2);
                line(mat, p3, p2, Scalar(255, 0, 0), 2);
                circle(mat, p1, 6, (0,255,0), 3);
            }
        }
    }

    return validPoints;
}

float Hand::innerAngle (Point p1, Point p2, Point defect) {
    float dist1 = getDistance(p1, defect);
    float dist2 = getDistance(p2, defect);
    float Ax, Ay;
    float Bx, By;
    float Cx, Cy; 
    Cx = defect.x;
    Cy = defect.y;
    if (dist1 < dist2) {
        Bx = p1.x;
        By = p1.y;
        Ax = p2.x;
        Ay = p2.y;
    } else {
        Bx = p2.x;
        By = p2.y;
        Ax = p2.x;
        Ay = p2.y;
    }
        
    float Q1 = Cx - Ax;
    float Q2 = Cy - Ay;
    float P1 = Bx - Ax;
    float P2 = By - Ay;
 
    float A = acos((P1*Q1) / (sqrt(P1*P1 + P2*P2) * sqrt(Q1*Q1 + Q2*Q2)) );
    A = A*180/CV_PI;
    return A;
}

float Hand::getDistance(Point p1, Point p2) {
    float p1x = float(p1.x);
    float p1y = float(p1.y);
    float p2x = float(p2.x);
    float p2y = float(p2.y);
    float distance = sqrt( pow((p1x - p2x), 2) + pow((p1y - p2y), 2));
    return distance;
}

float Hand::getTriangleArea(Point p1, Point p2, float defectDepth) {
    float base = getDistance(p1, p2);
    return (pow((base * defectDepth), 2)/2);
}

void Hand::debugPrint(Mat mat) {
    //colors
    const Scalar GREEN = Scalar(0,255,0);
    const Scalar BLUE = Scalar(255,0,0);
    const Scalar RED = Scalar(0,0,255);
    
    //draw a box around the convex
    Rect boundingBox = boundingRect(_contour);
    rectangle(mat, boundingBox, GREEN);

    //draw circles at each fingertip
    vector<Point> fingers = getFingertips();
    for (size_t i = 0; i < fingers.size(); i++) {
        circle(mat, fingers[i], 1, BLUE);
    }
}
