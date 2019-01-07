#include <opencv2/opencv.hpp>
#include <stdlib.h>
#include <fstream>
#include "RoboVision/ObjectDetector.h"
#include "RoboVision/Yolo.h"
#include "RoboVision/DetectedObject.h"

using namespace cv;
using namespace std;

ObjectDetector::ObjectDetector(){}

// Performs object classification and returns a vector of found images.
vector<DetectedObject> ObjectDetector::scan(Mat image){
  //Load classifier
  string classFilePath = string("robovisionClasses.txt");
  vector<string> classes = loadClassesFromFile(classFilePath);
  Yolo yo = Yolo(classes);

  //classify
  vector<DetectedObject> output;
  // TODO: literally anything
  return output;
}

vector<string> ObjectDetector::loadClassesFromFile(string filepath) {
  vector<string> output;
  ifstream infile(filepath);
  string str;
  while (getline(infile, str)) {
    output.push_back(str);
  }
  return output;
}