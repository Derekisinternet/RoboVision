#include <opencv2/opencv.hpp>
#include <stdlib.h>
#include "RoboVision/ObjectDetector.h"
#include "RoboVision/Yolo.h"

using namespace cv;
using namespace std;

ObjectDetector::ObjectDetector(){}

// moves a sliding window across the Mat and performs object classification
vector<cv::Rect> ObjectDetector::scan(Mat image){
  //Load classifier
  string classFile = "robovisionClasses.txt";
  Yolo yo = Yolo(classFile);

  //classify

}