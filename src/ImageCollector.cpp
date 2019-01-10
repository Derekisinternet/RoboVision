// opens a webcam and saves images for object detection.
// to keep things organized, it creates a folder in the 
// local directory, then numbers the images.
#include <opencv2/opencv.hpp>
#include <opencv2/imgproc/imgproc.hpp>
#include <opencv2/highgui/highgui.hpp>
#include <iostream>
#include <sys/stat.h>
#include <unistd.h>
#include <errno.h>
#include <dirent.h>
#include <stdlib.h>
#include <regex>
#include "RoboVision/ImageCollector.h"
#include "RoboVision/Util.h"
#include "RoboVision/DetectedObject.h"

using namespace cv;
using namespace std;

void mouseCallbackWrapper(int one, int two, int three, int four, void* ptr) {
    ImageCollector* ic = reinterpret_cast<ImageCollector*> (ptr);
    ic->mouseCallback(one, two, three, four);
}

ImageCollector::ImageCollector() {
    _fileIndex = 0;
    _pt1.x = 0;
    _pt1.y = 0;
    _pt2.x = 0;
    _pt2.y = 0;
    _established = false;
    _drawBound = false;
    _currentFrame = Mat();
    _WINDOW_NAME = "RoboVision Image Collector";
    _boxes = vector<DetectedObject>();
}

void ImageCollector::drawCenteredBox(int width, int height) {
    printf("Frame dimensions: %d x %d\n", _currentFrame.cols, _currentFrame.rows);
    if (width < _currentFrame.cols && height < _currentFrame.rows) {
        _pt1.x = ((_currentFrame.cols - width)/2);
        _pt1.y = ((_currentFrame.rows - height)/2);
        _pt2.x = _pt1.x + width;
        _pt2.y = _pt1.y + height;
    }
}

// int ImageCollector::collectorLoop(string folderName){
//     // TODO: get rid of setting global var like this. It's because of the mouseCallback.
//     _className = folderName;
//     getReady(folderName);

//     //open webcam
//     VideoCapture cap(0);
//     if (!cap.isOpened()) {
//         cout << "camera not operational" << endl;
//         return -1;
//     } else {
//         printf("Camera operational\n");
//     }

//     //create window
//     namedWindow(_WINDOW_NAME, 1);

//     //set callback
//     cv::setMouseCallback(_WINDOW_NAME, mouseCallbackWrapper,
//                          (void *) this);

//     const Scalar GREEN = Scalar(0,255,0);

//     if (_established) {
//         cap >> _currentFrame;
//         setBoxDimensions(folderName);
//     }

//     for (;;) {
//         cap >> _currentFrame;
//         // imshow("RoboVision Image Collector", _currentFrame);
//         redraw();

//         // don't iterate until keypress
//         switch(waitKey(30)) {
//             // save image by pressing spacebar
//             case 32: {
//                 if (_boxes.size() > 0) {
//                     saveImage(folderName, _currentFrame);
//                 }
//                 break;
//             }
//             // break the loop by pressing 'Esc'
//             case 27:
//                 break;
//         }   
//     }
//     return 0;
// }

int ImageCollector::videoCollectorLoop(string folderName){
    getReady(folderName);
    _WINDOW_NAME = "RoboVision Video collector";

    //open webcam
    VideoCapture cap(0);
    if (!cap.isOpened()) {
        cout << "Camera not operational" << endl;
        return -1;
    } else {
        printf("Camera operational\n");
    }
    // create window
    namedWindow(_WINDOW_NAME, 1);
    // set callback
    cv::setMouseCallback(_WINDOW_NAME, mouseCallbackWrapper,(void *) this);
    // create a VideoWriter object.
    double fps = cap.get(CAP_PROP_FPS);
    int frame_width = cap.get(CV_CAP_PROP_FRAME_WIDTH); 
    int frame_height = cap.get(CV_CAP_PROP_FRAME_HEIGHT); 
    VideoWriter video("output.avi", VideoWriter::fourcc('M','J','P','G'),fps, 
        Size(frame_width, frame_height),true);

    for (;;) {
        bool recording;
        cap >> _currentFrame;
        if (recording) {
            video.write(_currentFrame);
        }
        imshow(_WINDOW_NAME, _currentFrame);
        redraw();

        switch(waitKey(30)) {
            //press spacebar to stop recording
            case 32: {
                Util::debugPrint("videoCollector", "spacebar event");
                if (recording) { 
                    recording = false; 
                } else {
                    recording = true;
                }
            }
            // press Esc to move to next step
            case 27:
                Util::debugPrint("videoCollector", "Escape button event");
                break; 
        }
    }
    // Load the video you just recorded and classify objects frame by frame
    VideoCapture vid("output.avi");
    if (!vid.isOpened()) {
        cout << "unable to load saved output file.\n";
        return -1;
    }

    for(;;) {
        Mat frame;
        // ::read(Mat) returns bool of operation success
        if (!vid.read(frame)){
            break;
        }
        // rebuild window
        destroyWindow(_WINDOW_NAME);
        _WINDOW_NAME = "Training Data Labelling Stage";
        namedWindow(_WINDOW_NAME);
        imshow(_WINDOW_NAME, frame);

        //set up mouse stuff
        cv::setMouseCallback(_WINDOW_NAME, mouseCallbackWrapper,(void *) this);
        const Scalar GREEN = Scalar(0,255,0);

        switch (waitKey(0)) {
            case 32: {
                Util::debugPrint("videoCollector", "spacebar event");
                break;
            }
            case 27:
                Util::debugPrint("videoCollector", "Escape Button");
                break;
        }
    }

    //TODO: Delete output.avi before exiting
    return 0;
}

bool ImageCollector::getReady(string dirName) {
    // if user didn't specify a classifier name, exit:
    if(dirName == ""){
        printf("No Object name specified.\n");
        printf("Usage: imageCapture <name for image>");
        return false;
    }
   
    //create a new folder with the argument name:
    if ( mkdir(dirName.c_str(), S_IRWXU) == -1) {
        if (errno == EEXIST) {
            // dir already exists, get the index of the last image:
            int newIndex = getLastImageIndex(dirName);
            printf("folder already exists. Starting current index at %d\n", newIndex);
            _fileIndex = newIndex;
            _established = true;
            return true;         
        } else {
            printf("Error creating folder: %s",  dirName.c_str());
            return false;
        }
    } else {
        printf("Created folder: %s\n",  dirName.c_str());
        return true;
    }
}

void ImageCollector::setBoxDimensions(string dirName) {
    DIR *dir;
    struct dirent *entry;
    if ((dir = opendir(dirName.c_str())) != NULL) {
        char* name;
        while ((entry = readdir(dir))) {
            name = entry->d_name;
            if (name[0] != '.') {
                break;
            }
        }
        printf("retrieving class dimensions from sample image: %s\n", name);
        //append file name to folder to give us a filepath:
        dirName.insert(dirName.end(), '/');
        dirName.append(name);
        Mat sample = imread(dirName, IMREAD_COLOR);
        int width = sample.cols;
        int height = (sample.rows + 1); // account for how crop() works. Look it up.
        printf("sample image dimensions: %d x %d\n", width, height);
        drawCenteredBox(width, height);
    }
}

// get the name of the last file in a directory
int ImageCollector::getLastImageIndex(string directory) {
    DIR *dir ;
    struct dirent *entry;
    int newIndex = 0;
    if ((dir = opendir(directory.c_str())) != NULL) {
             
        while ((entry = readdir(dir))) {
            char* name = entry->d_name;
            string s = name;
            // extract int from filename string:
            int number = atoi(s.substr(0,-3).c_str());
            if (number > newIndex) {
                newIndex = number;
            }
        }
        // printf("final index: %d\n", newIndex);
        // printf("closing dir\n");
        closedir(dir);
        newIndex += 1;
        return newIndex;
    } 
    else {
        printf("getLastImageIndex(): cannot open directory %s", directory.c_str());
        return -1;
    }
}

void ImageCollector::saveImage(string dir, Mat image) {
    stringstream ss;
    ss << dir << "/" << _fileIndex << ".bmp";
    imwrite(ss.str(), image);
    printf("saving file: %d\n", _fileIndex);
    _fileIndex +=1;
}

// controls mouse actions:
// Left Button: First click starts a bounding box, second click completes it
// Right Button: If clicked inside a bounding box, that bounding box is dismissed.
void ImageCollector::mouseCallback(int event, int x, int y, int flags) {
    if (event == EVENT_LBUTTONDOWN) {
        // first click starts a bounding box, resets one if exists
        if (!_drawBound) {
            _pt1.x = x;
            _pt1.y = y;
            // setting a small difference so that rectangle doesn't get weird.
            _pt2.x = x + 1;
            _pt2.y = y + 1;
            printf("point 1:\n");
            printf("   x = %d\n", _pt1.x);
            printf("   y = %d\n", _pt1.y);
            _drawBound = true;
        } else { // second click completes drawing bound
            _pt2.x = x;
            _pt2.y = y;
            printf("point 2\n");
            printf("   x = %d\n", _pt2.x);
            printf("   y = %d\n", _pt2.y);
            _drawBound = false;

            DetectedObject object;
            object.location = Rect(_pt1, _pt2);
            object.classLabel = _className;
            _boxes.push_back(object);
        }

    } else if (event == EVENT_RBUTTONDOWN) {
        printf("rightbutton click\n");
        // remove box if button click was inside it
        for ( int i = 0; i < _boxes.size(); i++) {
            if (_boxes[i].location.contains(Point(x,y)) ) {
                vector<DetectedObject>::iterator iter(_boxes.begin() + i);
                _boxes.erase(iter);
            }
        }
        redraw();
    } else if (event == EVENT_MOUSEMOVE) {
        // if boundingbox in progress
        // redraw box with new posish
        if (_drawBound) {
            _pt2.x = x;
            _pt2.y = y;
        }
    }
    redraw();
}

void ImageCollector::redraw() {
    const Scalar GREEN = Scalar(0,255,0);
    // draw the rectangles
    for( DetectedObject box : _boxes) {
        rectangle(_currentFrame, box.location, GREEN);
    }
    // draw the in-progress bounding box
    if (_drawBound) {
        rectangle(_currentFrame, Rect(_pt1, _pt2), GREEN);
    }
    imshow(_WINDOW_NAME, _currentFrame);
}

Mat ImageCollector::crop() {
    // rectangle overlaps the top and left sides of the Mat,
    // so I gotta trim those sides:
    // Also, copy the points so that we don't mess with the originals.
    Point pt1 = _pt1;
    Point pt2 = _pt2;

    if (_pt1.x < _pt2.x) {
        if (_pt1.y < _pt2.y) {
            pt1.x +=1;
            pt1.y +=1;
        } else {
            pt1.x +=1;
            pt2.y +=1;
        }
    } else {
        if (_pt1.y < _pt2.y) {
            pt1.y +=1;
            pt2.x +=1;
        } else {
            pt2.y +=1;
            pt2.x +=1;
        }
    }
    Rect_<int> roi = Rect(pt1, pt2);
    Mat output = Mat(_currentFrame, roi);
    return output;
}
