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
	_posSamples = 0;
	_negSamples = 0;
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
	// testPath(positives, true);
	// testPath(negatives, false);
    testClassifier(positives, negatives);
    const clock_t end = std::clock();
    int clockTicks = end - start;
	printResults(clockTicks);
}

void CategoryTester::testClassifier(char* positives, char* negatives) {
    Classifier classy;
    
    printf("Gathering images . . .\n");
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
    // make sure negative images have the same dimensions as positives:
    Util::debugPrint("CategoryTester::testClassifier", "Sanitizing negative training inputs . . .\n");
    sanitizeNegatives(negativeTrainVector, positiveTrainVector[0]);
    printf("Sanitizing negative testing inputs . . .\n");
    sanitizeNegatives(negativeTestVector, positiveTestVector[0]);
    
    _posSamples = (int)positiveTestVector.size();
    _negSamples = (int)negativeTestVector.size();
    printf("Number of positive training examples: %zd\n", positiveTrainVector.size());
    printf("Number of negative training examples: %zd\n", negativeTrainVector.size());
    int numTrainings = ((int)positiveTrainVector.size() + (int)negativeTrainVector.size());
    printf("Total Number of training examples: %d\n", numTrainings );
    printf("Number of test examples: %d\n", (_posSamples + _negSamples) );

    // Distill the image collections into feature sets:
    printf("\nExtracting features from training images . . .\n");
    trainGradientList.clear(); // make sure it's empty
    classy.computeHOGs(positiveTrainVector, trainGradientList);
    classy.computeHOGs(negativeTrainVector, trainGradientList);
    printf("Total number of training feature examples: %zd\n", trainGradientList.size());
    
    printf("Creating training labels . . . \n");
    trainLabels.clear();
    trainLabels.assign( positiveTrainVector.size(), +1);
    trainLabels.insert(trainLabels.end(), negativeTrainVector.size(), -1);
    printf("Total number of training labels: %zd\n", trainLabels.size());

    printf("\nCreating training Data . . . \n");
    Mat trainingData;
    convertToML(trainGradientList, trainingData);

    // Train the classifier
    printf("Training Classifier . . .\n");
    Util::debugPrint("CategoryTester::testClassifier", "training classifier");
    classy.train(trainingData, trainLabels);

    const clock_t end = std::clock();
    int clockTicks = end - start;
    float seconds = float(clockTicks)/CLOCKS_PER_SEC;
    printf("Training Complete. Time Elapsed: %f Seconds\n", seconds);

    printf("\nCreating test data set . . .\n");
    classy.computeHOGs(positiveTestVector, testGradientList);
    classy.computeHOGs(negativeTestVector, testGradientList);
    printf("Number of test examples: %zd\n", testGradientList.size());

    printf("Creating labels for test data . . .\n");
    // create answer key for test data:
    testLabels.clear();
    testLabels.assign(positiveTestVector.size(), +1);
    testLabels.insert(testLabels.end(), negativeTestVector.size(), -1);
    printf("Total number of test labels: %zd\n", testLabels.size());
    
    printf("\nStart of Tests:\n");
    if (!classy.isTrained()) {
        Util::debugPrint("CategoryTester::testClassifier", "Categorizer not trained. Returning.");
        return;
    }
    HOGDescriptor hog;
    hog.load("HOGDescriptor_Saved");
   
    // gather list of positive and negative Mats
    positiveTestVector.insert(positiveTestVector.end(), negativeTestVector.begin(), negativeTestVector.end());
    for (size_t i = 0; i < positiveTestVector.size(); i++ ){
        vector<cv::Point> foundObjects;
        hog.detect(positiveTestVector[i], foundObjects);
        if ( ( (foundObjects.size() > 0) && (testLabels[i] == 1) ) || 
             ( (foundObjects.size() == 0) && (testLabels[i] == -1) ) )
        {
            _correct++;
            printf("CORRECT\n");
        } else {
            _incorrect++;
            printf("INCORRECT\n");
            printf("  EXPECTED CLASSIFICATION: %d\n", testLabels[i]);
            printf("  FOUND OBJECTS: %zd\n", foundObjects.size());
        }
    }
}

void CategoryTester::populateVectors(char* folderPath, 
                             vector<Mat> &trainOutput,
                             vector<Mat> &testOutput) {
    
    std::vector<std::string> fileList = Util::getFileNames(folderPath);
    Util::debugPrint("CategoryTester::populateVectors", "Number of files in folder path:");
    printf("%zd\n", fileList.size());

    for (int i = 0; i < fileList.size(); i++) {
        Mat mat = imread(fileList[i], CV_LOAD_IMAGE_COLOR);
        printf("Image columns: %d. rows: %d\n", mat.cols, mat.rows);
        if (i % 3 == 0) {
            testOutput.push_back(mat);
        } else {
            trainOutput.push_back(mat);
        }
    }
}

void CategoryTester::sanitizeNegatives(vector<Mat> matVector, Mat reference) {
    int x = reference.cols;
    int y = reference.rows;
    printf("reference image cols: %d, rows: %d\n", x, y);
    for (int i = 0; i < matVector.size(); i++) {
        //Mat oldMat = ;
        printf("negative image cols: %d, rows: %d\n", matVector[i].cols, matVector[i].rows);
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

void CategoryTester::convertToML(const Mat hogFeature, Mat& svmInput) {
    const int cols = (int)std::max( hogFeature.cols, hogFeature.rows );
    Mat tmp( 1, cols, CV_32FC1 ); //< used for transposition if needed
    svmInput = Mat( 1, cols, CV_32FC1 );

    CV_Assert( hogFeature.cols == 1 || hogFeature.rows == 1 );

    if( hogFeature.cols == 1 ) {
        transpose( hogFeature, tmp );
        tmp.copyTo( svmInput.row(0) );
    }
    else if(hogFeature.rows == 1 ) {
        hogFeature.copyTo( svmInput.row(0) );
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
            	_posSamples++;
            } else {
            	_negSamples++;
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
                _posSamples = fileList.size();
            } else {
                _negSamples = fileList.size();
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
	Mat image = imread(file, CV_LOAD_IMAGE_COLOR);
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
    printf("Total inputs: %d\n", (_posSamples + _negSamples));
	printf("number of positive test samples: %d\n", _posSamples);
	printf("number of negative test samples: %d\n", _negSamples);
    printf("---OUPUT---\n");
    printf("number of correct guesses: %d\n", _correct);
    printf("number of incorrect guesses: %d\n", _incorrect);
    float percentage = getPercentage(_correct, _incorrect);
    printf("Probability of correct guess: %f\n", percentage);
}

float CategoryTester::getPercentage(int operand, int dividend) {
    float ans = (float)operand / (float)dividend;
    return ans;
}
