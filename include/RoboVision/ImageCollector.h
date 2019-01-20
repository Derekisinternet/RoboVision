#include <opencv2/opencv.hpp>

#ifndef IMAGECOLLECTOR_h
#define IMAGECOLLECTOR_h

class ImageCollector {
public:
	ImageCollector();
	int videoCollectorLoop(std::string folderName);
	void mouseCallback(int event, int x, int y, int flags);

private:
	// methods
	int getLastImageIndex(std::string directory);
	void saveImage(std::string filePath, cv::Mat image);
	void redraw();
	bool getReady(std::string dirName);
	cv::Mat crop();
	int recordVideo(char fileName[], cv::VideoCapture cap);
	int processFootage(char inFile[]);
	void saveCNNImage(std::string filePath, cv::Mat image);
	std::string buildClassLabel(struct DetectedObject object);
	// variables
	int _fileIndex;
	cv::Mat _currentFrame;
	cv::Point _pt1;
	cv::Point _pt2;
	bool _established;
	bool _drawBound;
	const char* _WINDOW_NAME;
	std::string _className;
	std::vector<struct DetectedObject> _boxes;
};

#endif