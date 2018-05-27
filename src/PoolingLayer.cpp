#include <opencv2/opencv.hpp>
#include "RoboVision/PoolingLayer.h"

Mat poolMax(Mat img, int divisor) {
  mat outMat;
  int boxWidth = (img.width / divisor);
  int boxHeight = (img.height / divisor);

  for (int j = 0; j < (img.height - boxHeight); j ++) {
    for (int i = 0; i < (img.width - boxwidth); i ++) {
      outMat[i][j] = getHighest(img, i, j, boxWidth, boxHeight);
    }
  }
}

// iterate over a group of pixels and determine the highest value among them
Mat getHighest(Mat img, int x, int y, int boxWidth, int boxHeight) {
  int highest = img[x][y];
  int channels = img.channels;

  for (int i = 1; i <= boxHeight; i++) {
    for (int j = 1; j <= boxWidth; j++) {
      if (img[j][i] > highest) {
        highest = img[j][i];
      }
    }
  }
  
  return highest;
}
