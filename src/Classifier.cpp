#include <opencv2/opencv.hpp>

#include "RoboVision/Classifier.h"
#include "RoboVision/Util.h"

using namespace cv;
using namespace std;

Classifier::Classifier() {
    _hogTrained = false;
	Util::debugPrint("classifier", "initializing CLassifier object");
	_svm = ml::SVM::create();
	
    // TODO: figure out what all this means.
	_svm->setCoef0( 0.0 );
    _svm->setDegree( 3 );
    _svm->setTermCriteria( TermCriteria( CV_TERMCRIT_ITER + CV_TERMCRIT_EPS, 1000, 1e-3 ) );
    //_svm->setGamma( 0 );
    _svm->setKernel( ml::SVM::LINEAR );
    _svm->setNu( 0.5 );
    _svm->setP( 0.1 ); // for EPSILON_SVR, epsilon in loss function?
    _svm->setC( 500 ); // From paper, soft classifier
    _svm->setType( ml::SVM::C_SVC ); // Categorical. Can also be: // EPS_SVR; // EPSILON_SVR; // may be also NU_SVR; // do regression task
}

Classifier::Classifier(char* configFile) {
	//TODO: check if input is a file location?
	try {
		Classifier();
		_hog.load(configFile);
        _hogTrained = true;
	} catch (std::exception &ex) {
		printf("Error initializing classifier with param: %s\n", configFile);
		cout << ex.what();
	}
}

void Classifier::train(Mat data, vector<int> labels) {
	if (data.rows == 0) {
		Util::errorPrint("Classifier::train", "number of rows in Mat input cannot be zero.");
		return;
	}
	if ((int)labels.size() != data.rows) {
		Util::errorPrint("Classifier::train", "number of rows in data Mat and number of labels do not match.");
        printf("    Number of rows: %d\n", data.rows);
        printf("    Number of labels: %zd\n", labels.size());
        return;
	}
    // if this works, mayberefactor the method to accept a mat  insted of vector:
    Mat labelMat = Mat(labels, true);
	bool success = _svm->train(data, cv::ml::ROW_SAMPLE, labelMat);
	if (success) {
		Util::debugPrint("Classifier::train", "successfully trained SVM");
		Util::debugPrint("Classifier::train", "Saving SVM . . .");
        vector<float> save = get_svm_detector(_svm);
		printf("attaching svm to HOG\n");
        _hog.setSVMDetector(save);
        _hogTrained = true;
        printf("saving HOG\n");
    	_hog.save("HOGDescriptor_Saved");
	} else {
		Util::errorPrint("Classifier::train", "Error training SVM");
	}

}

void Classifier::generateHOGs(const vector< Mat > & imageList, vector< Mat > & gradient_lst) {
    // copied from http://www.learnopencv.com/handwritten-digits-classification-an-opencv-c-python-tutorial/
    Mat gray;
    vector< float > descriptors;
    initializeHOG(imageList[0]);
    Util::debugPrint("Classifier::generateHOGs", "Number of input objects:");
    printf("    %zd\n", imageList.size());

    for (size_t i = 0; i < imageList.size(); i++) {
        // crop the image if it's bigger than the sample image.
        if ( imageList[i].cols >= _hog.winSize.width && imageList[i].rows >= _hog.winSize.height ) {
            Rect rect = Rect(( imageList[i].cols - _hog.winSize.width ) / 2,
                          ( imageList[i].rows - _hog.winSize.height ) / 2,
                          _hog.winSize.width,
                          _hog.winSize.height);
            cvtColor( imageList[i](rect), gray, COLOR_BGR2GRAY );
            _hog.compute( gray, descriptors, Size( 8, 8 ), Size( 0, 0 ) );
            gradient_lst.push_back( Mat( descriptors ).clone() );
        } else {
            Util::errorPrint("Classifier::generateHOGs", "Mat in input list smaller than HOG dimensions. Excluding . . .");
            printf("    At index: %zd\n", i);
            printf("    image dimensions: %d x %d\n", imageList[i].cols, imageList[i].rows);
        }
    }
    Util::debugPrint("Classifier::generateHOGs", "Number of output objects:");
    printf("    %zd\n", gradient_lst.size()); 
}

void Classifier::initializeHOG(Mat mat) {
    Util::debugPrint("Classifier::initializeHOG", "Gathering HOG dimensions from input sample");
    int matWidth = mat.cols;
    int matHeight = mat.rows;
    if (matWidth == 0 || matHeight == 0) {
        Util::errorPrint("Classifier::InitializeHOG", "Training image dimensions must be greater than zero.");
    }
    // if there's remainders, it'll screw things up.
    if (matWidth % 4 != 0) {
        int remain1 = matWidth % 4;
        matWidth -= remain1;
    }
    if (matHeight % 4 != 0) {
        int remain2 = matHeight % 4;
        matHeight -= remain2;
    }
    Size winSize = Size(matWidth,matHeight);
    Size blocksize = Size( (matWidth/2),(matHeight/2) );
    Size blockStride = Size( (matWidth/4),(matHeight/4) );
    Size cellSize = Size((matWidth/2),(matHeight/2) );
    printf("winsize: %d x %d\n", winSize.width, winSize.height);
    printf("blocksize: %d x %d\n", blocksize.width, blocksize.height);
    printf("blockStride: %d x %d\n", blockStride.width, blockStride.height);
    printf("cellSize: %d x %d\n", cellSize.width, cellSize.height);

    _hog = HOGDescriptor(
        winSize,
        blocksize,
        blockStride,
        cellSize,
        9, //nbins,
        1, //derivAper,
        -1, //winSigma,
        0, //histogramNormType,
        0.2, //L2HysThresh,
        1, //gammal correction,
        64, //nlevels=64
        1 //Use signed gradients
    );
}

vector< float > Classifier::get_svm_detector( const Ptr< ml::SVM >& svm ) {
    char* methodSig = "Classifier::get_svm_detector";
    // get the support vectors
    Util::debugPrint(methodSig, "calling svm.getSupportVectors");
    Mat sv = svm->getSupportVectors();
    const int sv_total = sv.rows;
    // get the decision function
    Mat alpha, svidx;
    Util::debugPrint(methodSig, "calling svm.getDecisionFunction");
    double rho = svm->getDecisionFunction( 0, alpha, svidx );

    Util::debugPrint(methodSig, "doing some CVAsserts");
    CV_Assert( alpha.total() == 1 && svidx.total() == 1 && sv_total == 1 );
    CV_Assert( (alpha.type() == CV_64F && alpha.at<double>(0) == 1.) ||
               (alpha.type() == CV_32F && alpha.at<float>(0) == 1.f) );
    CV_Assert( sv.type() == CV_32F );

    vector< float > hog_detector( sv.cols + 1 );
    Util::debugPrint(methodSig, "doing a memcopy");
    memcpy( &hog_detector[0], sv.ptr(), sv.cols*sizeof( hog_detector[0] ) );
    hog_detector[sv.cols] = (float)-rho;
    Util::debugPrint("Classifier::get_svm_detector", "successfully retrieved svm");
    return hog_detector;
}

// Wrapper for classify(vector<float>) that allows user to pass a Mat
float Classifier::classify(Mat input) {
    // Util::debugPrint("Classifier::classify", "attempting classification on Mat object");
    vector<float> descriptors;
    _hog.compute(input, descriptors);
    return classify(descriptors);
}

float Classifier::classify(vector<float> input) {
	// Util::debugPrint("Classifier::classify", "predicting response");
    float response = 0;
    if (isTrained()) {
        response = _svm->predict(input);
    }
	return response;
}

vector<cv::Point> Classifier::detect(Mat mat) {
    vector<cv::Point> foundPoints;
    _hog.detect(mat, foundPoints);
    return foundPoints;
}

void Classifier::detectMultiScale(
    cv::Mat rawImage, 
    vector< cv::Rect > detections, 
    vector<double> foundWeights
    ) 
    {
      _hog.detectMultiScale(rawImage, detections, foundWeights);  

}

bool Classifier::isTrained() {
	return _hogTrained;
}
