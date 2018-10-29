#include <opencv2/opencv.hpp>

#ifndef CLASSIFIER_h
#define CLASSIFIER_h

using namespace std;

class Classifier {
	public:
		Classifier();
		Classifier(char* configFile);
		float classify(cv::Mat input);
		float classify(vector<float> input);
		vector<cv::Point> detect(cv::Mat mat);
		void detectMultiScale(cv::Mat rawImage, vector< cv::Rect > detections, vector<double> foundWeights);
		void train(cv::Mat data, vector<int> labels);
		bool isTrained();
		void generateHOGs(const vector< cv::Mat > & imageList, vector< cv::Mat > & gradient_lst);

	private:
		bool _hogTrained;
		cv::HOGDescriptor _hog;
		cv::Ptr<cv::ml::SVM> _svm;
		void initializeHOG(cv::Mat mat);
		vector< float > get_svm_detector( const cv::Ptr< cv::ml::SVM >& svm );
};

#endif