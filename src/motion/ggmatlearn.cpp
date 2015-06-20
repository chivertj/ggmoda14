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
/*
 * ggmatlearn.cpp
 *
 *  Created on: July 28, 2014
 *      Author: jpc
 */

#include "ggmatlearn.h"
#include <iostream>

namespace gg {
  void ggmatlearn::learn(const std::vector<FEATIMG> &learnimgfeats, const MASKIMG &mask) {

    std::cout <<"in learn..."<<std::endl;

    assert(learnimgfeats.size()>0);
    for (int j=0;j<learnimgfeats.size();j++) {
      assert(learnimgfeats[j].channels()==1);
    }

    m_nmaskpnts=cv::countNonZero(mask);
    if (m_nmaskpnts<MINLEARNSIZE) {
      m_em_model.clear();
      return;
    }
    m_nfeatures=learnimgfeats.size();
    cv::Mat trainingsamples(m_nmaskpnts,m_nfeatures,CV_32FC1);
    std::cout <<"\t extracting samples:"<<m_nmaskpnts<<","<<m_nfeatures<<std::endl;
    int trainingidx=0;
    int rows=mask.rows,cols=mask.cols;
    //copy data over
    for (int y=0;y<rows;y++) {
      for (int x=0;x<cols;x++) {
	if (mask.at<MASKT>(y,x)>0) {
	  for (int j=0;j<m_nfeatures;j++)
	    trainingsamples.at<featT>(trainingidx,j)=learnimgfeats[j](y,x);
	  trainingidx++;
	}
      }
    }
    std::cout <<"\t training."<<std::endl;
    m_em_model.train(trainingsamples);
  }
  void ggmatlearn::learn(IplImage **c_imgs, int noImgs, IplImage *c_mask) {
    std::vector<FEATIMG> learnimgfeats(noImgs);
    for (int i=0;i<noImgs;i++)
      learnimgfeats[i]=cv::cvarrToMat(c_imgs[i]);
    MASKIMG mask=cv::cvarrToMat(c_mask);
    learn(learnimgfeats,mask);
  }
  logprobT ggmatlearn::GetLogProb(const cv::Mat_<featT> &samplevalue) const {
    return m_em_model.predict(samplevalue)[0];
  }
  logprobT ggmatlearn::GetLogProb(featT *rawdata) const {
    return GetLogProb(cv::Mat(1,m_nfeatures,cv::DataType<featT>::type,rawdata));
  }
}
