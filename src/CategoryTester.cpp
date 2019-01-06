#include <opencv2/opencv.hpp>
#include <sys/stat.h>
#include <dirent.h>
#include <stdlib.h>
#include <time.h>
#include "RoboVision/CategoryTester.h"
#include "RoboVision/Classifier.h"
#include "RoboVision/HandFeatureExtractor.h"
#include "RoboVision/Classifier.h"
#include "RoboVision/Util.h"

using namespace cv;
using namespace std;

CategoryTester::CategoryTester() {
	_posTrainSamples = 0;
	_negTrainSamples = 0;
    _posTestSamples = 0;
    _negTestSamples = 0;
    _falsePos = 0;
    _falseNeg = 0;
	_correct = 0;
	_incorrect = 0;
}

void CategoryTester::test(char* positives) {
    const clock_t start = std::clock();
	testPath(positives, true);
	const clock_t end = std::clock();
    int clockTicks = end - start;
    printResults(clockTicks);
}

void CategoryTester::test(char* positives, char* negatives) {
    const clock_t start = std::clock();
    testClassifier(positives, negatives);
    const clock_t end = std::clock();
    int clockTicks = end - start;
	printResults(clockTicks);
}

void CategoryTester::testClassifier(char* positives, char* negatives) {
    
    Classifier classy;
    const clock_t start = std::clock();
   
    vector<Mat> positiveTrainVector, negativeTrainVector, trainGradientList;
    vector<int> trainLabels;
    vector<Mat> positiveTestVector, negativeTestVector, testGradientList;
    vector<int> testLabels;

    // Gather the images in the given directories into vectors of Mat objects
    Util::debugPrint("CategoryTester::testClassifier", "Populating Positive vectors");
    populateVectors(positives, positiveTrainVector, positiveTestVector);
    Util::debugPrint("CategoryTester::testClassifier", "Populating Negative vectors");
    populateVectors(negatives, negativeTrainVector, negativeTestVector);

    _posTestSamples = (int)positiveTestVector.size();
    _negTestSamples = (int)negativeTestVector.size();
    _posTrainSamples = (int)positiveTrainVector.size();
    _negTrainSamples = (int)negativeTrainVector.size();
    printf("Total training samples: %d\n", (_posTrainSamples + _negTrainSamples));
    printf("    Positive examples: %d\n", _posTrainSamples);
    printf("    Negative examples: %d\n", _negTrainSamples);
    printf("Total test examples: %d\n", (_posTestSamples + _negTestSamples) );
    printf("    Positive examples: %d\n", _posTestSamples);
    printf("    Negative examples: %d\n", _negTestSamples);

    // make sure negative images have the same dimensions as positives:
    Util::debugPrint("CategoryTester::testClassifier", "Sanitizing negative training inputs . . .");
    sanitizeNegatives(negativeTrainVector, positiveTrainVector[0]);
    Util::debugPrint("CategoryTester::testClassifier", "Sanitizing negative testing inputs . . .");
    sanitizeNegatives(negativeTestVector, positiveTestVector[0]);

    // Distill the image collections into feature sets:
    trainGradientList.clear(); // make sure output list is empty first
    Util::debugPrint("CategoryTester::testClassifier", "Extracting features from positive training images . . .");
    classy.generateHOGs(positiveTrainVector, trainGradientList);
    Util::debugPrint("CategoryTester::testClassifier", "Extracting features from negative training images . . .");
    classy.generateHOGs(negativeTrainVector, trainGradientList);
    Util::debugPrint("CategoryTester::testClassifier", "Total number of training feature examples:");
    printf("    %zd\n", trainGradientList.size());
    
    printf("Creating training labels . . . \n");
    trainLabels.clear();
    trainLabels.assign( positiveTrainVector.size(), 1);
    trainLabels.insert(trainLabels.end(), negativeTrainVector.size(), -1);
    printf("Total number of training labels: %zd\n", trainLabels.size());

    printf("\nCreating training Data . . . \n");
    Mat trainingData;
    convertToML(trainGradientList, trainingData);

    // Train the classifier
    Util::debugPrint("CategoryTester::testClassifier", "training classifier");
    classy.train(trainingData, trainLabels);

    const clock_t end = std::clock();
    int clockTicks = end - start;
    float seconds = float(clockTicks)/CLOCKS_PER_SEC;
    printf("Training Complete. Time Elapsed: %f Seconds\n", seconds);

    printf("\nCreating test data . . .\n");
    testLabels.clear();
    testLabels.assign(positiveTestVector.size(), 1);
    testLabels.insert(testLabels.end(), negativeTestVector.size(), -1);
    // concat negative images and positivie images
    positiveTestVector.insert(positiveTestVector.end(), negativeTestVector.begin(), negativeTestVector.end());
    // printf("Total number of test labels: %zd\n", testLabels.size());

    Util::debugPrint("CategoryTester::testClassifier","Start of Tests:");
    if (!classy.isTrained()) {
        Util::errorPrint("CategoryTester::testClassifier", "Categorizer not trained. Returning.");
        return;
    }

    for (size_t i = 0; i < positiveTestVector.size(); i++) {
        // vector<cv::Point> foundPoints = classy.detect(positiveTestVector[i]);
        // if ( ( (foundPoints.size() > 0) && (testLabels[i] == 1) ) || 
        //      ( (foundPoints.size() == 0) && (testLabels[i] == 0) ) )
        // {
        //     _correct++;
        // } else {
        //     _incorrect++;
        //     printf("INCORRECT\n");
        //     printf("  EXPECTED CLASSIFICATION: %d\n",testLabels[i]);
        //     printf("  FOUND POINTS: %zd\n", foundPoints.size());
        //     if (testLabels[i] == 1) {
        //         _falseNeg++;
        //     }
        //     else {
        //         _falsePos++;
        //     }
        // }
        
        // Util::debugPrint("CategoryTester::testClassifier", "getting svm guess");
        float response = classy.classify(positiveTestVector[i]);
        if (response == testLabels[i]) {
            _correct++;
        } else {
            _incorrect++;
            printf("INCORRECT\n");
            printf("  EXPECTED CLASSIFICATION: %d\n", testLabels[i]);
            printf("  PREDICTION: %d\n", response);
            if (testLabels[i] == 1) {
                _falseNeg++;
            } else {
                _falsePos++;
            }
        }
    }
}

void CategoryTester::populateVectors(char* folderPath, 
                             vector<Mat> &trainOutput,
                             vector<Mat> &testOutput) {
    
    std::vector<std::string> fileList = Util::getFileNames(folderPath);
    Util::debugPrint("CategoryTester::populateVectors", "Number of files in folder path:");
    int size = (int)fileList.size();
    printf("%d\n", size);

    for (size_t i = 0; i < size; i++) {
        Mat mat = imread(fileList[i], IMREAD_COLOR);
        if (i % 3 == 0) {
            testOutput.push_back(mat);
        } else {
            trainOutput.push_back(mat);
        }
    }
}

void CategoryTester::sanitizeNegatives(vector<Mat> & matVector, Mat& reference) {
    int x = reference.cols;
    int y = reference.rows;
    Util::debugPrint("CategoryTester::sanitizeNegatives", "Trimming dimensions according to reference image:");
    printf("  reference image cols: %d, rows: %d\n", x, y);
    for (int i = 0; i < matVector.size(); i++) {
        Rect rect = Rect(0, 0, x, y);
        Mat newMat = matVector[i](rect);
        matVector[i] = newMat;
    }
}

void CategoryTester::convertToML(const vector<Mat> & imageVector, Mat& trainData) {
    const int rows = (int)imageVector.size();
    const int cols = (int)std::max( imageVector[0].cols, imageVector[0].rows );
    Mat tmp( 1, cols, CV_32FC1 ); //< used for transposition if needed
    trainData = Mat( rows, cols, CV_32FC1 );

    for( size_t i = 0 ; i < imageVector.size(); ++i )
    {
        CV_Assert( imageVector[i].cols == 1 || imageVector[i].rows == 1 );

        if( imageVector[i].cols == 1 ) {
            transpose( imageVector[i], tmp );
            tmp.copyTo( trainData.row( (int)i ) );
        }
        else if( imageVector[i].rows == 1 ) {
            imageVector[i].copyTo( trainData.row( (int)i ) );
        }
    }
}

// boolean 'match' tells the method if it's positive samples or 
// negatvie samples
void CategoryTester::testPath(char* path, bool match) {
	int step = Util::isFile(path);
    switch (step) {
        case  1: {
            printf("Argument is a file. Testing . . . \n");
            if (match) {
            	_posTestSamples++;
            } else {
            	_negTestSamples++;
            };
            bool matched = testImageFile(path);
            if ((matched & match) | (!matched & !match)) {
            	_correct++;
            } else {
            	_incorrect++;
            }
        }
        break; 
        case 0: {
            printf("Argument is a directory. Iterating . . .\n");
            vector<string> fileList = Util::getFileNames(path);
            if (match) {
                _posTestSamples = fileList.size();
            } else {
                _negTestSamples = fileList.size();
            }
            for(unsigned int i = 0; i < fileList.size(); i++) {
                string fileStr = fileList[i];
                bool matched = testImageFile(fileStr);
                if ((matched & match) || (!matched & !match)) {
	            	_correct++;
	            } else {
	            	_incorrect++;
	            }
            }
        }
        break;
        default: {
            printf("invalid filepath\n");
        }
        break;
    }
}

// NOTE: uses HandFeatureExtractor instead of HOG and SVM
bool CategoryTester::testImageFile(string file) {
	Mat image = imread(file, IMREAD_COLOR);
    HandFeatureExtractor handFeatExt = HandFeatureExtractor();
    bool matched = false;
    printf("Testing file: %s\n", file.c_str());
    if (handFeatExt.detect(image)) {
    	printf("POSITIVE\n");
    	matched = true;
    } else {
    	printf("NEGATIVE\n");
    }
    return matched;
}

void CategoryTester::printResults(int ticks) {
    printf("############################\n");
    printf("#      TEST RESULTS        #\n");
    printf("############################\n");
    printf("Number of Processor clock ticks: %d\n", ticks);
    float seconds = float(ticks)/CLOCKS_PER_SEC;
    printf("Run Time: %f Seconds\n", seconds);
    printf("---INPUTS---\n");
    printf("Total training inputs: %d\n",(_posTrainSamples + _negTrainSamples) );
    printf("Number of positive training samples: %d\n", _posTrainSamples);
    printf("NUmber of negative trainging samples: %d\n", _negTrainSamples);
    printf("Total test inputs: %d\n", (_posTestSamples + _negTestSamples));
	printf("number of positive test samples: %d\n", _posTestSamples);
	printf("number of negative test samples: %d\n", _negTestSamples);
    printf("---OUPUT---\n");
    printf("number of correct guesses: %d\n", _correct);
    printf("number of incorrect guesses: %d\n", _incorrect);
    printf("  False positives: %d\n", _falsePos);
    printf("  False negatives: %d\n", _falseNeg); 
    float percentage = getPercentage(_correct, (_posTestSamples + _negTestSamples) );
    printf("Probability of correct guess: %f\n", percentage);
}

float CategoryTester::getPercentage(int operand, int dividend) {
    float ans = (float)operand / (float)dividend;
    return ans;
}
