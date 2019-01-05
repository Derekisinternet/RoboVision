#include <opencv2/opencv.hpp>
#include "RoboVision/Yolo.h"

using namespace std;
using namespace cv;

Yolo::Yolo(){}

// implements the YOLO algorithm, outputs a vector of bounding boxes.
vector<Rect> Yolo::yolo(Mat input); {
  vector<Rect> grid = makeGrid(input);
  for (vector::iterator it = grid.begin)
}

// divides a mat into a grid
vector<Rect> Yolo::makeGrid(Mat input) {
  int gridSize = 13;
  vector<Rect> output;
  int rowStep = (input.rows / gridSize);
  int colStep = (input.cols / gridSize);
  for (int i = 0; i < gridSize; i++) {
    int x1 = colStep * i;
    int x2 = colStep * (i + 1);
    int y1 = rowStep * i;
    int y2 = rowStep * (i + 1);
    Rect rect(x1, x2, y1, y2);
    output.push_back(rect); 
  }
}
