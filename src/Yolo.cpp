#include <opencv2/opencv.hpp>
#include "RoboVision/Yolo.h"

using namespace std;
using namespace cv;

Yolo::Yolo(){
  subSections = 13;
  boundingBoxes = 2;
}

// implements the YOLO algorithm, outputs a vector of bounding boxes.
vector<Rect> Yolo::yolo(Mat input) {
  vector<gridCell> grid = makeGrid(input, subSections);
  vector<Rect> gridBoundingBoxes;
  // makeBoundingBoxes()
  // TODO: lolwut
  return gridBoundingBoxes;
}

// divides a mat into a grid
vector<Yolo::gridCell> Yolo::makeGrid(Mat input, int gridSize) {
  vector<Yolo::gridCell> output;
  int rowStep = (input.rows / gridSize);
  int colStep = (input.cols / gridSize);
  for (int i = 0; i < gridSize; i++) {
    for (int j = 0; j < gridSize; j++) {
      gridCell cell;
      int x1 = colStep * i;
      int x2 = colStep * (i + 1);
      int y1 = rowStep * j;
      int y2 = rowStep * (j + 1);
      Rect rect(x1, x2, y1, y2);
      cell.location = rect;
      output.push_back(cell);
    } 
  }
  return output;
}
