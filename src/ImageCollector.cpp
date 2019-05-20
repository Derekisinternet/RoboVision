// opens a webcam and saves images for object detection.
// to keep things organized, it creates a folder in the 
// local directory, then numbers the images.
#include <opencv2/opencv.hpp>
#include <opencv2/imgproc/imgproc.hpp>
#include <opencv2/highgui/highgui.hpp>
#include <iostream>
#include <fstream>
#include <sys/stat.h>
#include <unistd.h>
#include <errno.h>
#include <dirent.h>
#include <stdlib.h>
#include <regex>
#include <ctime>
#include "RoboVision/ImageCollector.h"
#include "RoboVision/Util.h"
#include "RoboVision/DetectedObject.h"
#include "RoboVision/Display.h"

using namespace cv;
using namespace std;

void mouseCallbackWrapper(int one, int two, int three, int four, void* ptr) {
    ImageCollector* ic = reinterpret_cast<ImageCollector*> (ptr);
    ic->mouseCallback(one, two, three, four);
}

ImageCollector::ImageCollector() {
    _pt1.x = 0;
    _pt1.y = 0;
    _pt2.x = 0;
    _pt2.y = 0;
    _drawBound = false;
    _currentFrame = Mat();
    _WINDOW_NAME = "RoboVision Image Collector";
    _boxes = vector<DetectedObject>();
}

int ImageCollector::videoCollectorLoop(string folderName){
    bool ready = getReady(folderName);
    if (!ready) {
        return -1;
    }
    _WINDOW_NAME = "RoboVision Video collector";

    //open webcam
    VidCap vidya = newVidCap(0, 500, 333);
    
    // create window
    namedWindow(_WINDOW_NAME, 1);

    // record training footage
    char outFile[] = "output.avi";
    if (recordVideo(outFile, vidya) != 0 ) {
        return -1;
    }

    // go over each frame and draw bounding boxes
    if (processFootage(outFile) != 0 ) {
        return -1;
    }

    if (remove(outFile) !=0 ) {
        Util::errorPrint("ImageCollector :: videoCollectorLoop", "unable to delete temporary output file");
    }
    return 0;
}


ImageCollector::VidCap ImageCollector::newVidCap(int device, int cropWidth, int cropHeight) {
    ImageCollector::VidCap output;
    // open webcam
    output.cap = VideoCapture(device);
    if (!output.cap.isOpened()) {
         Util::errorPrint("ImageCollector::newVidCap", "Camera notoperational\n");
        return output;
    } else {
        printf("Camera operational\n");
    }
    //capture an image to set the crop frame
    Mat img;
    output.cap >> img;
    int x = (img.cols - cropWidth)/2;
    int y = (img.rows - cropHeight)/2;
    output.crop = Rect(x, y, cropWidth, cropHeight);
    return output;
}

int ImageCollector::recordVideo(char fileName[], VidCap viddy) {
    bool breakLoop;
    bool recording;

    // create a VideoWriter object.
    double fps = viddy.cap.get(CAP_PROP_FPS);
    int codec = VideoWriter::fourcc('M','J','P','G');
    int frame_width = viddy.cap.get(CAP_PROP_FRAME_WIDTH); 
    int frame_height = viddy.cap.get(CAP_PROP_FRAME_HEIGHT); 
    VideoWriter video;
    
    if ( video.open(fileName, codec,fps, Size(frame_width, frame_height),true) ) {
        Util::debugPrint("ImageCollector::videoLoop", "videoWriter initialized");
    } else {
        Util::errorPrint("ImageCollector::videoCollectorLoop", "Unable to open VideoWriter");
            return -1;
    }

    for (;;) {
        if (breakLoop) {
            break;
        }

        viddy.cap >> _currentFrame;
        if (recording) {
            video.write(_currentFrame);
        }
        imshow(_WINDOW_NAME, _currentFrame);

        switch(waitKey(30)) {
            //press spacebar to stop recording
            case 32: {
                if (recording) { 
                    recording = false;
                    Util::debugPrint("videoCollector", "Recording Stopped");
                } else {
                    recording = true;
                    Util::debugPrint("videoCollector", "Recording Started");
                }
                break;
            }
            // press Esc to move to next step
            case 27:
                Util::debugPrint("videoCollector", "Moving to Labelling stage");
                breakLoop = true;
                video.~VideoWriter();
                break; 
        }
    }
    return  0;
}

int ImageCollector::processFootage(char inFile[]) {
    // Load the video you just recorded and classify objects frame by frame
    VideoCapture vid(inFile, CAP_OPENCV_MJPEG);
    if (vid.isOpened()) {
        Util::debugPrint("ImageCollector::videoCollectorLoop", "Loaded stored video");
    } else {
        Util::errorPrint("ImageCollector::videoCollectorLoop", 
            "unable to load saved output file.\n");
        return -1;
    }

    // rebuild window
    destroyWindow(_WINDOW_NAME);
    _WINDOW_NAME = "Training Data Labelling Stage";
    namedWindow(_WINDOW_NAME);
    imshow(_WINDOW_NAME, _currentFrame);
    //set up mouse stuff
    cv::setMouseCallback(_WINDOW_NAME, mouseCallbackWrapper,(void *) this);

    bool breakLoop;
    bool next;
    string s = "number of classes: " + std::to_string(_classNames.size());
    Util::debugPrint("ImageCollector::videoCollectorLoop", s.c_str());
    _colors = Display::colorVector(_classNames.size());
    for(;;) {
        if (breakLoop) { break; }
        if (next) {
            vid >> _currentFrame;
            imshow(_WINDOW_NAME, _currentFrame);
            next = false;
        }
        redraw();
        switch (waitKey(0)) {
            case 32: {
                // Use a timestamp as a unique name
                time_t rawTime;
                // tm is a stuct with the different components of a date
                struct tm * timeInfo;
                char buffer[80];
                time(&rawTime);
                timeInfo = localtime(&rawTime);
                strftime(buffer, 80, "/%Y%m%d%H%M%S", timeInfo);
                string filePath(_workingDir + buffer);
                saveCNNImage(filePath, _currentFrame);

                _boxes.clear();
                next = true;
                break;
            }
            case 27: {
                Util::debugPrint("ImageCollector", "See You Space Cowboy");
                breakLoop = true;
                break;
            }
            case 48: { // Number 0
                setNumber(0);
                break;
            }
            case 49: { // Number 1
                setNumber(1);
                break;
            }
            case 50: { // Number 2
                setNumber(2);
                break;
            }
            case 51: { //etc.
                setNumber(3);
                break;
            }
            case 52: {
                setNumber(4);
                break;
            }
            case 53: {
                setNumber(5);
                break;
            }
            case 54: {
                setNumber(6);
                break;
            }
            case 55: {
                setNumber(7);
                break;
            }
            case 56: {
                setNumber(8);
                break;
            }
            case 57: {
                setNumber(9);
                break;
            }
        }
    }
    return 0;
}

void ImageCollector::setNumber(int number) {
    if (number < _classNames.size()) {
        string msg = "Setting current class to index " + std::to_string(number) + ": " + _classNames[number];
        Util::debugPrint("videoCollector", msg.c_str() );
        _currentClass = number;
    } else {
        Util::errorPrint("ImageCollector :: setNumber", "attempting to set class number greater than the number of classes. ");
    } 
}

bool ImageCollector::getReady(string dirName) {
    // if user didn't specify a classifier name, exit:
    if(dirName == ""){
        printf("No Folder specified.\n");
        printf("Usage: imageCapture <name for image>");
        return false;
    }
    _workingDir = dirName;
   
    // find class file
    string classFile = _workingDir + "/classes.names";
    ifstream fStream;
    fStream.open(classFile.c_str());
    if (fStream.is_open()) {
        Util::debugPrint("ImageCollector :: getReady", "loaded class file:");
        string line;
        int index = 0;
        while (getline(fStream, line) ) {
            _classNames.push_back(line);
            printf("%d %s\n", index, line.c_str());
            index++;
        }
        return true;
    } else {
        string error = "unable to open " + classFile;
        Util::errorPrint("ImageCollector :: getReady", error.c_str());
        return false;
    }
}

// default  way to save image
void ImageCollector::saveImage(string filePath, Mat image) {
    imwrite(filePath, image);
    string message = "saving file: " + filePath;
    Util::debugPrint("ImageCollectopr::saveImage", message.c_str());
}

// overloaded to also save CNN data
void ImageCollector::saveCNNImage(string filePath, Mat image) {
    if (_boxes.size() > 0) {
        // create a char string for the file name
        ofstream classFile;
        string classFileName = filePath + ".txt";
        string message = "saving file: " + classFileName;
        Util::debugPrint("ImageCollector :: saveCNNImage", message.c_str());
        classFile.open(classFileName, ios::out);
        vector<DetectedObject>::iterator it;
        for (it = _boxes.begin(); it != _boxes.end(); it++ ) {
            string line = buildClassLabel(*it);
            classFile << line;
        }
        classFile.close();
        filePath += ".jpg";
        saveImage(filePath, image);
    }
}

// Build a line for class file.
string ImageCollector::buildClassLabel(struct DetectedObject object) {
    stringstream ss;
    Point center = object.getCenter();
    float x = float (center.x) / float (_currentFrame.cols); // find center point, divide by width
    float y = float (center.y) / float (_currentFrame.rows);
    float  width = float (object.location.width) / float (_currentFrame.cols); // express size as percentage of whole image
    float height = float (object.location.height) /float (_currentFrame.rows);
    ss << object.classNumber << " " << x << " " << y << " " << width << " " << height << "\n";
    return ss.str();
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

            // TODO: is it right to give 100% confidence? IDK.
            string label = _classNames[_currentClass];
            DetectedObject object(label, _currentClass, Rect(_pt1, _pt2), 1.0);
            _boxes.push_back(object);
            Util::debugPrint("ImageCollector::MouseCallback", "added box. New count:");
            cout << _boxes.size() << "\n";
        }

    } else if (event == EVENT_RBUTTONDOWN) {
        // remove box if button click was inside it
        for ( int i = 0; i < _boxes.size(); i++) {
            if (_boxes[i].location.contains(Point(x,y)) ) {
                vector<DetectedObject>::iterator iter(_boxes.begin() + i);
                Util::debugPrint("ImageCollector::MouseCallback", "removed box.");
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
    Mat newFrame =_currentFrame.clone();
    // draw the rectangles
    for( DetectedObject box : _boxes) {
        rectangle(newFrame, box.location, _colors[box.classNumber]);
    }
    // draw the in-progress bounding box
    if (_drawBound) {
        rectangle(newFrame, Rect(_pt1, _pt2), _colors[_currentClass]);
    }
    imshow(_WINDOW_NAME, newFrame);
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
