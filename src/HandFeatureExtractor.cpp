#include <opencv2/opencv.hpp>
#include "RoboVision/HandFeatureExtractor.h"

using namespace cv;
using namespace std;
 
HandFeatureExtractor::HandFeatureExtractor(){

	// TODO: pull out threshold values into config file.
	_minY = 84;
	_maxY = 230;
	_minCr = 89;
	_maxCr = 165;
	_minCb = 104;
	_maxCb = 119;
	_blurSize = 3;
	_elementSize = 2;

    _hand = Hand();
    _validHand = false;
}

bool HandFeatureExtractor::detect(Mat image) {
	Mat processed = preProcess(image);
	
	// Contour detection:
    vector<Point> handContour = getLargestContour(processed);

    // Convex hull:
    if (!handContour.empty()) {
        vector<vector<Point>> hull(1);
        convexHull(Mat(handContour), hull[0], false);

        // get Convex defects to detect fingertips:
        if (hull[0].size() > 2) {
            
            vector<int> hullIndexes;
            convexHull(Mat(handContour), hullIndexes, true);
            vector<Vec4i> convexDefects;
            cv::convexityDefects(Mat(handContour),
                            hullIndexes, convexDefects);
            
            //TODO: test if features are actually of a hand.

            //store it in case I want to do something else with it.
            _hand = Hand(handContour, 
                                 hullIndexes,
                                 convexDefects);
            _validHand = true;
            return true;
        } else {
            // now's a good time to clear old data
            _hand = Hand();
        }
        return false;
    } else {
        return false;
    }
}

Mat HandFeatureExtractor::preProcess(Mat mat) {
    Mat yccImage;
    
    // convert raw RGB image to YCrCb and apply thresholds:
    cvtColor(mat, yccImage, CV_BGR2YCrCb);
    inRange(yccImage, Scalar(_minY, _minCr, _minCb), 
            Scalar(_maxY, _maxCr, _maxCb), yccImage);
    
    // apply blur to reduce noise:
    medianBlur(yccImage, yccImage, _blurSize);

    //apply dilate to fill in gaps:
    Mat element = getStructuringElement(MORPH_ELLIPSE,
                    Size(2 * _elementSize + 1, 2 * _elementSize +1),
                    Point(_elementSize, _elementSize));
    dilate(yccImage, yccImage, element);
    return yccImage;
}

vector<Point> HandFeatureExtractor::getLargestContour(Mat mat) {
    vector< vector<Point> > contours;
    vector<Vec4i> hierarchy;
    findContours(mat, contours, hierarchy, CV_RETR_EXTERNAL,
                    CV_CHAIN_APPROX_SIMPLE, Point(0, 0));
        
    size_t largestContour = getLargestContourIndex(contours);       
    if (!contours.empty()) { 
        return contours[largestContour];
    } else {
        vector<Point> empty;
        return empty;
    }
}

size_t HandFeatureExtractor::getLargestContourIndex(vector<vector<Point>> contours) {
    size_t largestContour = 0;
    for (size_t i = 1; i < contours.size(); i++) {
        if (contourArea(contours[i]) > contourArea(contours[largestContour])) {
            largestContour = i;
        }
    }
    return largestContour;
}

vector<Point> HandFeatureExtractor::getFingerTips(vector<Point> contour,
                            vector<Vec4i> defects, Rect bound) {
    vector<Point> validPoints; 
    Point center = Point(bound.x + bound.width/2,
                         bound.y + bound.height/2);
    Point indexFinger;
    Point thumb;
    float biggestDefectArea = 0;

    for (size_t i = 0; i < defects.size(); i++) {
        Point p1 = contour[defects[i][0]]; //start
        Point p2 = contour[defects[i][1]]; //end
        Point p3 = contour[defects[i][2]]; // pt furthest from convex edge
        float depth = defects[i][3]/256.0; // dist. from edge to p3
        
        double inAngle = innerAngle(p1.x, p1.y, p2.x, p2.y, p3.x, p3.y);
        double length = getDistance(p1, p3);
        if (inAngle < 120 
            && depth > 0.1 * length
            && length > 0.2 * bound.height)
        {
            validPoints.push_back(p1);
            // the defect between the thumb and index should have
            // the most Area.
            float triangleArea = getTriangleArea(p1, p2, depth);
            if ( triangleArea > biggestDefectArea) {
                biggestDefectArea = triangleArea;
                if (getDistance(p1, p3) > getDistance(p2, p3)) {
                    indexFinger = p1;
                    thumb = p2;
                } else {
                    indexFinger = p2;
                    thumb = p1;
                }
            }
        }
    }
    return validPoints;
}

float HandFeatureExtractor::getTriangleArea(Point p1, Point p2, float defectDepth) {
    float base = getDistance(p1, p2);   
    return (pow((base * defectDepth), 2)/2);
}

void HandFeatureExtractor::dump(Mat mat) {
    _hand.debugPrint(mat);
}


float HandFeatureExtractor::getDistance(Point p1, Point p2) {
    float p1x = float(p1.x);
    float p1y = float(p1.y);
    float p2x = float(p2.x);
    float p2y = float(p2.y);
    float distance = sqrt( pow((p1x - p2x), 2) + pow((p1y - p2y), 2));
    return distance;
}


//TODO: refactor to use getDistance(). Make it look nicer.
float HandFeatureExtractor::innerAngle (float px1, float py1, float px2, float py2,
                  float cx1, float cy1) {
    float dist1 = sqrt( (px1 - cx1)*(px1 - cx1) + (py1-cy1)*(py1 - cy1) );
    float dist2 = sqrt( (px2 - cx1)*(px2 - cx1) + (py2 - cy1)*(py2 - cy1) );

    float Ax, Ay;
    float Bx, By;
    float Cx, Cy;

    Cx = cx1;
    Cy = cy1;
    if (dist1 < dist2) {
        Bx = px1;
        By = py1;
        Ax = px2;
        Ay = py2;
    } else {
        Bx = px2;
        By = py2;
        Ax = px1;
        Ay = px1;
    }

    float Q1 = Cx - Ax;
    float Q2 = Cy - Ay;
    float P1 = Bx - Ax;
    float P2 = By - Ay;
    
    float A = acos((P1*Q1) / (sqrt(P1*P1 + P2*P2) * sqrt(Q1*Q1 + Q2*Q2) ) );

    A = A*180/CV_PI;

    return A;
}
