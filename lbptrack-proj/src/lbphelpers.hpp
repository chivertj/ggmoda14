#ifndef LBPHELPERS_CLASS
#define LBPHELPERS_CLASS

#include<vector>
#include<string>
#include<opencv2/opencv.hpp>
#include<opencv2/highgui/highgui.hpp>
#include<opencv2/ml/ml.hpp>
#include<opencv2/objdetect/objdetect.hpp>

using namespace std;
using namespace cv;

namespace lbphelpers {
vector<Rect> detectrects(CascadeClassifier &cascade, double scale, const Mat &img);
void showDetections(const vector<Rect>& found, Mat& imageData);
}

#endif //LBPHELPERS_CLASS

