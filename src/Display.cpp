#include <opencv2/opencv.hpp>
#include "RoboVision/Display.h"
#include "RoboVision/Util.h"

using namespace cv;
using namespace std;

Display::Display() {}

// returns a vector of color scalars
std::vector<cv::Scalar> Display::colorVector(int i) {
  vector<Scalar> output;
  if ( i <= 0 || i > 255 ) {
    string msg = "input out of bounds: " + std::to_string(i);
    Util::errorPrint("Display::permutate", msg.c_str());
  } else {
    int step = 255/i;
    int j = 0;
    while (output.size() < i) {
      Scalar a(j,i,j);
      output.push_back(a);

      Scalar b(i,j,j);
      output.push_back(b);
    
      Scalar d(j,j,i);
      output.push_back(d);  

      Scalar c(j,i,i);
      output.push_back(c);  

      Scalar e(i,i,j);
      output.push_back(e);  

      Scalar f(i,j,i);
      output.push_back(f);

      j+= step;
      // just so I don't cause trouble later.
      if (j > 255) { j = 255;}
    }
  }
  return output;
}

std::vector<cv::Scalar> Display::permutate(int i, int j) {
  if ( i <= 0 || i > 255 ) {
    string msg = "input out of bounds: " + std::to_string(i);
    Util::errorPrint("Display::permutate", msg.c_str());
  } else {
    vector<Scalar> output;
    Scalar a(j,i,0);
    output.push_back(a);

    Scalar b(i,j,0);
    output.push_back(b);
  
    Scalar d(j,0,i);
    output.push_back(d);

    Scalar c(i,0,j);
    output.push_back(c);

    Scalar e(0,j,i);
    output.push_back(e);

    Scalar f(0,i,j);
    output.push_back(f);

    // TODO: add more colors
    return output;
  }
}
