#include <opencv2/opencv.hpp>
#include "RoboVision/Classifier.h"

#ifndef CATEGORYTESTER_h
#define CATEGORYTESTER_h

class CategoryTester {
	public:
		CategoryTester();
		void test(char* postives);
		void test(char* postives, char* negatives);
		void testClassifier(char* positives, char* negatives);
	private:
		bool testImageFile(std::string path);
		void testPath(char* path, bool match);
		void printResults(int ticks);
		int isFile(char* name);
		float getPercentage(int operand, int dividend);
		void populateVectors(char* folderPath, 
                             std::vector<cv::Mat> &trainOutput,
                             std::vector<cv::Mat> &testOutput);
		std::vector<std::string> getFileNames(char* dirPath);
		void sanitizeNegatives(std::vector<cv::Mat> & matVector, cv::Mat& reference);
		void convertToML(const vector< cv::Mat > & train_samples, 
								 cv::Mat& trainData);
		int _posTestSamples;
		int _negTestSamples;
		int _posTrainSamples;
		int _negTrainSamples;
		int _correct;
		int _incorrect;
		int _falsePos;
		int _falseNeg;
		Classifier _classifier;


};

#endif