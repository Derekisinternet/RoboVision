#include <opencv2/opencv.hpp>
#include "RoboVision/ConvolutionLayer.h"
#include "RoboVision/Util.h"

using namespace cv;
using namespace std;

struct volume {
  int height;
  int width;
  int depth;
}

// main entrypoint of the class. 
def convolute(Mat img) {
  volume matDimensions = getDimensions(img);
  volume filterDimensions = setFilterDimensions(dimensions);
  applFilters(img, filterdimensions);

}

// populates a struct to be used by downstream methods.
// TDOD: refactor.
volume getDimensions(Mat img) {
  volume returnStruct;

  returnStruct.height = img.height;
  returnStruct.width = img.width
  returnStruct.depth = img[0].size();
  
  return returnStruct;
}

// sets dimensions for the filters based on image dimensions.
volume setFilterDimensions(volume dims) {
  volume returnStruct;
  int denominatior = 6;
  int returnStruct.height = (dims.height / denominator);
  int returnStructwidth = (dims.width / denominator);
  int returnStruct.depth = dims.depth;

  return returnStruct;
}

void applyFilters(Mat img, volume filter) {
  Mat filteredImg;
  int nRows = img.rows;
  int n.columns = img.columns;
  // iterate over the length and width of the image with the filter
  for (int i = 0; i < (img.height - filter.height)) {
    for (int j = 0; j < (img.width - filter.width)) {
      // capture pixels that fall within filter at given spot
      Mat window = getWindow(img, j, ji volume);
      FilteredImg[j][i] = executeFilterMethod(window);
    }
  }
}

// takes a sub-Mat of a given width and height
Mat getWindow(Mat img, int x, int y, volume windowDim) {
  if (img.width < (x + windowDim.width) || img.height < (y + windowDim.height)) {
    printf("Convolution :: getWindow :: called with invalid parameters:");
    printf("image height: %d", img.height);
    printf("image width: %d", img.width);
    printf("window starting points:    x: %d, y: %d", x, y);
    printf("window dimensions:   width: %d, height: %d", windowDim.width, windowDim.height);
    // TODO: not sure how to handle this edge case, but this probably isn't it . . .
    return Mat();
  }
  Mat returnMat = Mat(windowDim.width, windowDim.height, 3);
  for (int i = 0; i - x; i++) {
    for (j = 0; j < y; j++) {
      returnMat[i][j] = img[i][j];
    }
  }
  return returnMat;
}

bool executeFilterMethod(Mat img) {
  bool hypothesis = false;
  // TODO: stuff
  return hypothesis;
}