////////////////////////////////////////////////////////////////////////
//Copyright (c) 2007-2015, John Chiverton 
//
//Permission to use, copy, modify, and/or distribute this software for any
//purpose with or without fee is hereby granted, provided that the above
//copyright notice and this permission notice appear in all copies.
//
//THE SOFTWARE IS PROVIDED "AS IS" AND THE AUTHOR DISCLAIMS ALL WARRANTIES
//WITH REGARD TO THIS SOFTWARE INCLUDING ALL IMPLIED WARRANTIES OF
//MERCHANTABILITY AND FITNESS. IN NO EVENT SHALL THE AUTHOR BE LIABLE FOR
//ANY SPECIAL, DIRECT, INDIRECT, OR CONSEQUENTIAL DAMAGES OR ANY DAMAGES
//WHATSOEVER RESULTING FROM LOSS OF USE, DATA OR PROFITS, WHETHER IN AN
//ACTION OF CONTRACT, NEGLIGENCE OR OTHER TORTIOUS ACTION, ARISING OUT OF
//OR IN CONNECTION WITH THE USE OR PERFORMANCE OF THIS SOFTWARE.
////////////////////////////////////////////////////////////////////////
#include "ggheadtracker.h"
#include <iostream>
namespace gg {
Heads::Heads(void) : minheadsize(30),maxheadsize(10000),xyratio(0.4), minregsize(minheadsize*4.) {} //minheadsize=80 minregsize=
bool Heads::isHead(const region::REGIONPNTS &reg, float &headproportion) const {
  region::Rects::R bounding=rects.makeRotatedRect(reg);
  float heighttowidthratio=bounding.size.height/float(bounding.size.width);
  std::cout <<"h2w:"<<heighttowidthratio<<",";
  bool ishead=false;
  if (heighttowidthratio>0.4 && heighttowidthratio<2.65) {
    headproportion=0.033124*heighttowidthratio+0.0079524;
    std::cout <<"headprop:"<<headproportion<<",";
    headproportion*=0.5;
    float possibleheadsize=cvRound(reg.size()*headproportion);
    std::cout <<"poss headsize:"<<possibleheadsize<<",";
    cv::Point minxminy(region::MinXMinY(reg,possibleheadsize));
    cv::Point maxxmaxy(region::MaxXMaxY(reg,possibleheadsize));
    float xdist=maxxmaxy.x-minxminy.x,ydist=maxxmaxy.y-minxminy.y;
    std::cout <<"xdist:"<<xdist<<",ydist:"<<ydist<<",";
    std::cout <<"xdist*x2y:"<<xdist*xyratio<<",xdist/x2y:"<<xdist/xyratio<<",";
    //XXX insufficient - could just be a small head!
    if (possibleheadsize>minheadsize &&
        possibleheadsize<maxheadsize && ydist>=(xdist*xyratio) && ydist<=(xdist/xyratio)) {
      ishead=true;
    }
  }
  return ishead;
}
void Heads::Find(std::vector<size_t> &heads, std::vector<float> &headproportions, const region::REGIONS &regs, size_t numregions) const {
  heads.clear();
  headproportions.clear();
  float headproportion;
  for (size_t i=0;i<numregions;i++) {
    if (regs[i].size()>minregsize && isHead(regs[i],headproportion)) {
      heads.push_back(i);
      headproportions.push_back(headproportion);
    }
  }
}
void Heads::Make(const region::REGIONPNTS &allpnts, float headproportion, region::REGIONPNTS &headpnts) const {
  assert(allpnts.size()>minregsize);
  //XXX headproportion not calculated here so v. poss. invalid value!
  size_t headsize=cvRound(allpnts.size()*headproportion);
  headpnts=region::REGIONPNTS(headsize);
  for (size_t i=0;i<headsize;i++)
    headpnts[i]=allpnts[i];
}
//#################################
void HeadClassifier::train(const cv::Mat &trainingdata, const cv::Mat &labels) {
  //    svm=CvSVM();
  svm.clear();
  criteria=cvTermCriteria(CV_TERMCRIT_EPS,1000,FLT_EPSILON);
  params=CvSVMParams(CvSVM::C_SVC,CvSVM::RBF,10,8,1,10,0.5,0.1,NULL,criteria);
  CvMat c_training_data(trainingdata),c_training_classifications(labels);
  svm.train_auto(&c_training_data,&c_training_classifications,0,0,params,10);
  params=svm.get_params();
}
float HeadClassifier::classify(const cv::Mat &feature) const {
  return svm.predict(feature);
}
//#################################
MakeHeadClassifier::MakeHeadClassifier(const std::string &trainfilename) {
  cv::Mat_<float> csvtraindata;
  csvfiling::ReadCSVtoMat(trainfilename,",",csvtraindata);
  if (csvtraindata.empty()) throw;
  training_data=cv::Mat(csvtraindata.rows,csvtraindata.cols-1,CV_32FC1,cv::Scalar(0.));
  cv::Mat csvtraindatarange=csvtraindata.colRange(0,csvtraindata.cols-1);
  csvtraindatarange.copyTo(training_data);

  NUMBER_OF_TRAINING_SAMPLES=training_data.rows;
  ATTRIBUTES_PER_SAMPLE=training_data.cols;

  training_classifications=cv::Mat(NUMBER_OF_TRAINING_SAMPLES,1,CV_32FC1);
  csvtraindatarange=csvtraindata.colRange(csvtraindata.cols-1,csvtraindata.cols);
  csvtraindatarange.copyTo(training_classifications);
  headclass.train(training_data,training_classifications);
}
//################
}
