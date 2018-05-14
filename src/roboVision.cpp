// an app to detect and respond to hand gestures using OpenCV

#include <opencv2/opencv.hpp>
#include <opencv2/imgproc/imgproc.hpp>
#include <opencv2/highgui/highgui.hpp>
#include <ctype.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/types.h>
#include "RoboVision/CategoryTester.h"
#include "RoboVision/Hand.h"
#include "RoboVision/HandFeatureExtractor.h"
#include "RoboVision/ImageCollector.h"

using namespace std;
using namespace cv;

// Global Variables
Mat rawImage;
HandFeatureExtractor handFeatExt;
bool debugFlag = false;
bool testFlag = false;
char* _testPath1 = NULL;
char* _testPath2 = NULL;
const char* windowName = "RoboVision";

// Method Headers:
void parseFlags(int argc, char** argv);
void debugLoop();
void testLoop(char* filePath1, char* filePath2);
vector<string> getFileNames(char* path);
void printHelp();

// MAIN LOOP
int main(int argc, char** argv) {
    parseFlags(argc, argv);
    namedWindow(windowName, WINDOW_AUTOSIZE);
    
    //flag-specific setup
    if (debugFlag) {   
      debugLoop(); 
    } else if (testFlag) {
      testLoop(_testPath1, _testPath2);
    }
}

void parseFlags(int argc, char** argv) {
    int args = 0;
    while ((args = getopt(argc, argv, "c:dn:t:")) != -1) {
        switch(args) {
            // collect sample images for a Classifier
            case 'c': {
                string arg = string(optarg);
                if (arg.size() > 0) {
                    ImageCollector collector = ImageCollector();
                    collector.collectorLoop(string(optarg));
                } else {
                    fprintf(stderr, "ERROR: '%s' is not a valid file path\n", optarg);
                }
                break;
            }
            // run in debug mode
            case 'd': {
                debugFlag = true;
                break;
            }
            // add negative samples to test
            case 'n': {
                _testPath2 = optarg;
                break;
            }
            // run tests;
            case 't': {
                testFlag = true;
                _testPath1 = optarg;
            }
                break;
            case '?':
                fprintf(stderr, "Unknown option: %s\n", args);
                printHelp();
            default:
                printHelp();
                break;
        }
    }
}

void debugLoop() {
    VideoCapture cap(0);

    if (!cap.isOpened()) {
        printf("Camera not operational\n");
        exit (EXIT_FAILURE);
    }
    printf("Camera operational\n");
    // handFeatExt = HandFeatureExtractor();
    Classifier classy = Classifier("HOGDescriptor_Saved");
    
    for (;;) {    
        cap >> rawImage;
        // if (handFeatExt.detect(rawImage)) {
        //     // TODO: something useful?
        //     printf("Found a hand!\n");
        //     handFeatExt.dump(rawImage);
        // }
        vector< Rect > detections;
        vector< double > foundWeights;
        classy.detectMultiScale(rawImage, detections, foundWeights);
        for ( size_t j = 0; j < detections.size(); j++ ) {
            Scalar color = Scalar( 0, foundWeights[j] * foundWeights[j] * 200, 0 );
            rectangle( rawImage, detections[j], color, rawImage.cols / 400 + 1 );
        }

        imshow(windowName, rawImage);
        if (waitKey(30) == 27) {
            exit (0);
        }
    }
}

void testLoop(char* filePath1, char* filePath2) {
    printf("RoboVision initialized in test mode with positive sample path: %s\n", filePath1);
    CategoryTester tester = CategoryTester();
    if ((filePath1 != NULL) & (filePath2 != NULL)) {
        printf("Negative sample path: %s\n", filePath2);
        tester.test(filePath1, filePath2);
    }
    else if (filePath1 != NULL) {
        tester.test(filePath1);
    } 
}

void printHelp() {
    printf("supported parameters:\n");
    printf(" -c <name>           collect training images\n");
    printf(" -d                  debug mode\n");
    printf(" -t <file/dir path>  test mode\n");
    
}
