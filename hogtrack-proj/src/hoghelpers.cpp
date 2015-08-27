#include<vector>
#include<string>
#include<fstream>
#include<iostream>

#include <stdio.h>
#include <dirent.h>
#include <ios>
#include <stdexcept>
#include <opencv2/opencv.hpp>
#include <opencv2/highgui/highgui.hpp>
#include <opencv2/ml/ml.hpp>
#include <opencv2/objdetect/objdetect.hpp>

using namespace std;
using namespace cv;

namespace hoghelpers {
////////////////////////////////////////////////////////////////////////
vector<float> readDescriptorVectorFromFile(string fileName) {
  cout<<"Reading descriptor vector from file"<<fileName.c_str()<<endl;
  string separator = " "; // Use blank as default separator between single features
  fstream File;
  float percent;
  File.open(fileName.c_str(), ios::in);
  float val;
  string strval;
  vector<float> descriptorVector;
  if (File.good() && File.is_open()) {
    while(File) {
      getline(File,strval,' ');
      if (strval!="" && strval!=" " && strval!="\n") 
  descriptorVector.push_back(atof(strval.c_str()));
    }
    File.close();
  }
#if(1)
  float detector[descriptorVector.size()];
  for (int i=0;i<descriptorVector.size();i++)
    detector[i]=descriptorVector[i];
#endif
  return vector<float>(detector, detector + sizeof(detector)/sizeof(detector[0]));
}
////////////////////////////////////////////////////////////////////////
void showDetections(const vector<Point>& found, Mat& imageData) {
    size_t i, j;
    for (i = 0; i < found.size(); ++i) {
        Point r = found[i];
        // Rect_(_Tp _x, _Tp _y, _Tp _width, _Tp _height);
        rectangle(imageData, Rect(r.x-16, r.y-32, 32, 64), Scalar(64, 255, 64), 3);
    }
}
////////////////////////////////////////////////////////////////////////
void showDetections(const vector<Rect>& found, Mat& imageData) {
    vector<Rect> found_filtered;
    size_t i, j;
    for (i = 0; i < found.size(); ++i) {
        Rect r = found[i];
        for (j = 0; j < found.size(); ++j)
            if (j != i && (r & found[j]) == r)
                break;
        if (j == found.size())
            found_filtered.push_back(r);
    }
    for (i = 0; i < found_filtered.size(); i++) {
        Rect r = found_filtered[i];
        rectangle(imageData, r.tl(), r.br(), Scalar(64, 255, 64), 3);
    }
}
////////////////////////////////////////////////////////////////////////
void detectTest(const HOGDescriptor& hog, const double hitThreshold, Mat& imageData) {
    vector<Rect> found;
    Size padding(Size(32, 32));
    Size winStride(Size(8, 8));
    hog.detectMultiScale(imageData, found, hitThreshold, winStride, padding);
    showDetections(found, imageData);
}
////////////////////////////////////////////////////////////////////////
vector<Rect> detectrects(const HOGDescriptor& hog, const double hitThreshold, Mat& imageData) {
    vector<Rect> found;
    Size padding(Size(32, 32));
    Size winStride(Size(8, 8));
    hog.detectMultiScale(imageData, found, hitThreshold, winStride, padding);
    return found;
}
////////////////////////////////////////////////////////////////////////
void calculateFeaturesFromInput(const string& imageFilename, vector<float>& featureVector, HOGDescriptor& hog) {
    Mat imageData = imread(imageFilename, 0);
    hog.winSize=Size(imageData.cols,imageData.rows);
    if (imageData.empty()) {
        featureVector.clear();
        printf("Error: HOG image '%s' is empty, features calculation skipped!\n", imageFilename.c_str());
        return;
    }
    if (imageData.cols<hog.winSize.width || imageData.rows<hog.winSize.height) {
        featureVector.clear();
        printf("Error: Image '%s' dimensions (%u x %u) do not match HOG window size (%u x %u)! (Too Small)\n", imageFilename.c_str(), imageData.cols, imageData.rows, hog.winSize.width, hog.winSize.height);
        return;      
    }
    else if (imageData.cols>hog.winSize.width || imageData.rows>hog.winSize.height) { //then center and take ROI
      int l=(imageData.cols-hog.winSize.width)/2.,t=(imageData.rows-hog.winSize.height)/2.;
      imageData=imageData(Rect(l,t,hog.winSize.width,hog.winSize.height));
    }
    vector<Point> locations;
    const Size trainingPadding = Size(0, 0);
    const Size winStride = Size(8, 8);
    hog.compute(imageData, featureVector, winStride, trainingPadding, locations);
    imageData.release(); // Release the image again after features are extracted
}
////////////////////////////////////////////////////////////////////////
void inithog(const string &initimgfile, HOGDescriptor &hog) {
  //initialise hog
  vector<float> featureVector;
  calculateFeaturesFromInput(initimgfile, featureVector, hog);
  //end initialise hog  
}

}
