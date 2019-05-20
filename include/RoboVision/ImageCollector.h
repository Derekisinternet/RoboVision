#include <opencv2/opencv.hpp>

#ifndef IMAGECOLLECTOR_h
#define IMAGECOLLECTOR_h

class ImageCollector {
public:
	ImageCollector();
	int videoCollectorLoop(std::string folderName);
	void mouseCallback(int event, int x, int y, int flags);
	// structs
	struct VidCap {
		cv::VideoCapture cap;
		cv::Rect crop;
	};

private:
	// methods
	ImageCollector::VidCap newVidCap(int device, int cropWidth, int cropHeight);
	void saveImage(std::string filePath, cv::Mat image);
	void redraw();
	bool getReady(std::string dirName);
	cv::Mat crop();
	int recordVideo(char fileName[], ImageCollector::VidCap viddy);
	int processFootage(char inFile[]);
	void saveCNNImage(std::string filePath, cv::Mat image);
	std::string buildClassLabel(struct DetectedObject object);
	void setNumber(int number);
	// variables
	cv::Mat _currentFrame;
	int _currentClass;
	std::string _workingDir;
	cv::Point _pt1;
	cv::Point _pt2;
	bool _drawBound;
	const char* _WINDOW_NAME;
	std::vector<std::string> _classNames;
	std::vector<struct DetectedObject> _boxes;
	std::vector<cv::Scalar> _colors;
};

#endif