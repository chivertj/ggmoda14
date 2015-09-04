#include "lbphelpers.hpp"

namespace lbphelpers {
////////////////////////////////////////////////////////////////////////
  vector<Rect> detectrects(CascadeClassifier &cascade, double scale, const Mat &img) {
    Mat gray,smallImg(cvRound(img.rows/scale),cvRound(img.cols/scale),CV_8UC1);
    cvtColor(img,gray,CV_BGR2GRAY);
    resize(gray,smallImg,smallImg.size(),0,0,INTER_LINEAR);
    equalizeHist(smallImg,smallImg);
    vector<Rect> detections;
    cascade.detectMultiScale(smallImg,detections,1.1,2,0|CV_HAAR_SCALE_IMAGE,Size(70,60),Size(250,250));
    int rsize=detections.size();
    for (int i=0;i<rsize;i++)
      detections.push_back(Rect(detections[i]));
    groupRectangles(detections,1,0.5);
    return detections;
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
}
