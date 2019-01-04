#include <opencv2/opencv.hpp>
#include <stdlib.h>
#include "RoboVision/ObjectDetector.h"
#include "RoboVision/Classifier.h"

using namespace cv;
using namespace std;

ObjectDetector::ObjectDetector(){}

// moves a sliding window across the Mat and performs object classification
vector<cv::Rect> ObjectDetector::scan(Mat image){
  //Load classifier
  //TODO: get rid of hard-coded file reference
  Classifier classy = Classifier("HOGDescriptor_Saved");
  //create window the dimensions of the  classified object

  //classify

}