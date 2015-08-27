#ifndef HOGHELPERS_CLASS
#define HOGHELPERS_CLASS

#include<vector>
#include<string>
#include <opencv2/opencv.hpp>
#include <opencv2/highgui/highgui.hpp>
#include <opencv2/ml/ml.hpp>
#include <opencv2/objdetect/objdetect.hpp>

#include "../svmlight/svmlight.h"
#define TRAINHOG_SVM_TO_TRAIN SVMlight

using namespace std;
using namespace cv;

namespace hoghelpers {
////////////////////////////////////////////////////////////////////////
vector<float> readDescriptorVectorFromFile(string fileName);
////////////////////////////////////////////////////////////////////////
void showDetections(const vector<Point>& found, Mat& imageData);
////////////////////////////////////////////////////////////////////////
void showDetections(const vector<Rect>& found, Mat& imageData);
////////////////////////////////////////////////////////////////////////
void detectTest(const HOGDescriptor& hog, const double hitThreshold, Mat& imageData);
////////////////////////////////////////////////////////////////////////
vector<Rect> detectrects(const HOGDescriptor& hog, const double hitThreshold, Mat& imageData);
////////////////////////////////////////////////////////////////////////
void calculateFeaturesFromInput(const string& imageFilename, vector<float>& featureVector, HOGDescriptor& hog);
////////////////////////////////////////////////////////////////////////
void inithog(const string &initimgfile, HOGDescriptor &hog);
////////////////////////////////////////////////////////////////////////
}

#endif //HOGHELPERS_CLASS

