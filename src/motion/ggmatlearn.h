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
 * ggmatlearn.h
 *
 *  Created on: 28 July, 2014
 *      Author: jpc
 */

#ifndef GGMATLEARN_H_
#define GGMATLEARN_H_

#include "opencv2/core/core.hpp"
#include "opencv2/ml/ml.hpp"
#include "opencv2/highgui/highgui.hpp"


namespace gg {
  //need to learn over a set of images and class masks -> number of classes = number of masks
  //need to test over a set of images with single mask
  //is this too fat? something simpler for most cases?
  //also have 1 ggmatlearn per learning class? then another class to calculate posterior probabilities
  typedef float featT;
  typedef double logprobT;
  //  template <typename featT=float, typename logprobT=double>
  class ggmatlearn {
  public:
    typedef cv::Mat_<featT> FEATIMG;
    typedef cv::Mat_<logprobT> PROBIMG;
    typedef float MASKT;
    typedef cv::Mat_<MASKT> MASKIMG;
    ggmatlearn(void) {}
    virtual ~ggmatlearn(void) {}
    void learn(const std::vector<FEATIMG> &learnimgfeats, const MASKIMG &mask);
    void learn(IplImage **imgs, int noImgs, IplImage *mask);
    logprobT GetLogProb(const cv::Mat_<featT> &samplevalue) const;
    logprobT GetLogProb(featT *rawdata) const;
    int GetNoMaskPnts(void) const { return m_nmaskpnts; }
  protected:
    cv::EM m_em_model;
    int m_nfeatures=0;
    const int MINLEARNSIZE=10;
    int m_nmaskpnts;
  };
}

#endif /* GGMATLEARN_H_ */
