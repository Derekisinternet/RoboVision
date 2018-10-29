#include <opencv2/opencv.hpp>

#ifndef IMAGECOLLECTOR_h
#define IMAGECOLLECTOR_h

class ImageCollector {
public:
	ImageCollector();
	int collectorLoop(std::string folderName);
	void mouseCallback(int event, int x, int y, int flags);

private:
	void setBoxDimensions(std::string dirName);
	void drawCenteredBox(int width, int height);
	int getLastImageIndex(std::string directory);
	void saveImage(std::string dir, cv::Mat image);
	void redraw();
	bool getReady(std::string dirName);
	cv::Mat crop();
	int _fileIndex;
	cv::Mat _currentFrame;
	cv::Point _pt1;
	cv::Point _pt2;
	bool _established;
	bool _drawBound;
	bool _subFrameBox;
	const char* _WINDOW_NAME;
};

#endif