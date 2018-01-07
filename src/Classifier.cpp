#include <opencv2/opencv.hpp>

#include "RoboVision/Classifier.h"
#include "RoboVision/Util.h"

using namespace cv;
using namespace std;

Classifier::Classifier() {
	Util::debugPrint("classifier", "initializing CLassifier object");
	_svm = ml::SVM::create();
	
    // TODO: figure out what all this means.
	_svm->setCoef0( 0.0 );
    _svm->setDegree( 3 );
    _svm->setTermCriteria( TermCriteria( CV_TERMCRIT_ITER + CV_TERMCRIT_EPS, 1000, 1e-3 ) );
    _svm->setGamma( 0 );
    _svm->setKernel( ml::SVM::LINEAR );
    _svm->setNu( 0.5 );
    _svm->setP( 0.1 ); // for EPSILON_SVR, epsilon in loss function?
    _svm->setC( 0.01 ); // From paper, soft classifier
    _svm->setType( ml::SVM::C_SVC ); // EPS_SVR; // EPSILON_SVR; // may be also NU_SVR; // do regression task
}

Classifier::Classifier(char* svmFile) {
	//TODO: check if input is a file location?
	try {
		// Ptr<ml::SVM> _svm = ml::SVM::create();
		Classifier();
		_svm->load(svmFile);
	} catch (std::exception &ex) {
		printf("Error initializing classifier with param: %s\n", svmFile);
		cout << ex.what();
	}
}

void Classifier::train(Mat data, vector<int> labels) {
	if (data.rows == 0) {
		Util::errorPrint("Classifier::train", "number of rows in Mat input cannot be zero.");
		return;
	}
	if ((int)labels.size() != data.rows) {
		Util::errorPrint("Classifier::train", "number of Mat rows and number of labels do not match.");
		return;
	}
	bool success = _svm->train(data, cv::ml::ROW_SAMPLE, labels);
	if (success) {
		Util::debugPrint("Classifier::train", "successfully trained SVM");

		Util::debugPrint("Classifier::train", "Saving SVM . . .");
		_hog.setSVMDetector( get_svm_detector(_svm) );
    	_hog.save("HOGDescriptor_Saved");
	} else {
		Util::errorPrint("Classifier::train", "Error training SVM");
	}

}

void Classifier::computeHOGs(const vector< Mat > & imageList, vector< Mat > & gradient_lst) {
    // copied from http://www.learnopencv.com/handwritten-digits-classification-an-opencv-c-python-tutorial/
    Mat gray;
    vector< float > descriptors;
    _hog = HOGDescriptor(
        Size(20,20), //winSize
        Size(10,10), //blocksize
        Size(5,5), //blockStride,
        Size(10,10), //cellSize,
                 9, //nbins,
                  1, //derivAper,
                 -1, //winSigma,
                  0, //histogramNormType,
                0.2, //L2HysThresh,
                  1,//gammal correction,
                  64,//nlevels=64
                  1);//Use signed gradients 

    for (size_t i = 0; i < imageList.size(); i++) {
        if ( imageList[i].cols >= _hog.winSize.width && imageList[i].rows >= _hog.winSize.height ) {
            Rect r = Rect(( imageList[i].cols - _hog.winSize.width ) / 2,
                          ( imageList[i].rows - _hog.winSize.height ) / 2,
                          _hog.winSize.width,
                          _hog.winSize.height);
            cvtColor( imageList[i](r), gray, COLOR_BGR2GRAY );
            _hog.compute( gray, descriptors, Size( 8, 8 ), Size( 0, 0 ) );
            gradient_lst.push_back( Mat( descriptors ).clone() );
            // if ( use_flip ) {
            //     flip( gray, gray, 1 );
            //     hog.compute( gray, descriptors, Size( 8, 8 ), Size( 0, 0 ) );
            //     gradient_lst.push_back( Mat( descriptors ).clone() );
            // }
        }
    }  
}

Mat Classifier::convertToMat(vector< vector<float> > featureSet) {
	int vect1Size = (int)featureSet.size();
	int vect2Size = (int)featureSet[0].size();
	Util::debugPrint("Classifier::convertToMat", "Inputs:");
	printf("vect1Size: %d, vect2Size: %d\n", vect1Size, vect2Size);
	//printf("sample: %f\n", featureSet[0]);

	float matInput[vect1Size][vect2Size];
	//convert the vector of vectors into a float array
	Util::debugPrint("Classifier::convertToMat", "Creating Float array:");
	for (int i = 0; i < vect1Size; i++) {
		for (int j = 0; j < vect2Size; j++) {
			printf("value %f going into index [%d][%d]\n", featureSet[i][j], i, j);
			matInput[i][j] = featureSet[i][j];
		}
	}
	Util::debugPrint("Classifier::convertToMat", "matInput:");
	printf("%f\n", matInput);

	Mat mat = Mat(vect1Size, vect2Size, CV_32FC1, matInput);

	Util::debugPrint("Classifier::convertToMat", "Created emtpy Matrix?:");
	printf(mat.empty() ? "true\n" : "false\n");
	return mat;
}

vector< float > Classifier::get_svm_detector( const Ptr< ml::SVM >& svm )
{
    // get the support vectors
    Mat sv = svm->getSupportVectors();
    const int sv_total = sv.rows;
    // get the decision function
    Mat alpha, svidx;
    double rho = svm->getDecisionFunction( 0, alpha, svidx );

    CV_Assert( alpha.total() == 1 && svidx.total() == 1 && sv_total == 1 );
    CV_Assert( (alpha.type() == CV_64F && alpha.at<double>(0) == 1.) ||
               (alpha.type() == CV_32F && alpha.at<float>(0) == 1.f) );
    CV_Assert( sv.type() == CV_32F );

    vector< float > hog_detector( sv.cols + 1 );
    memcpy( &hog_detector[0], sv.ptr(), sv.cols*sizeof( hog_detector[0] ) );
    hog_detector[sv.cols] = (float)-rho;
    return hog_detector;
}


float Classifier::classify(vector<float> input) {
	Util::debugPrint("Classifier.classify()", "predicting response");
	float response = _svm->predict(input);
	return response;
}

bool Classifier::isTrained() {
	return _svm->isTrained();
}
