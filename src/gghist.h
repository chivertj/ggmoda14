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
 * gghist.h
 *
 *  Created on: Jan 15, 2012
 *      Author: jpc
 */

#ifndef GGHIST_H_
#define GGHIST_H_

namespace gg {
  template <typename T>
  class gghist {
  public:
    gghist(void) {}
     virtual ~gghist(void) {}
    void operator() (const cv::Mat_<T> &img, const cv::Mat_<uchar> &mask, const T &nobins, uchar maskval) {
      if (mask.empty()) {
        size_t X=img.cols,Y=img.rows;
        for (size_t y=0;y<Y;y++) {

        }
      }
    }
  protected:
  };
  class CvMDLMixtModel {
  public:
    CvMDLMixtModel(void);
    virtual ~CvMDLMixtModel(void);
    void Init(IplImage **imgs, int noImgs, IplImage *mask, bool grtr, MDLMM_T maskVal=0, double minCov=1.);
    void OptNumMixes(void);
    void Learn(void);

    double GetProb(MDLMM_T *imgData);
    int GetNoMaskPnts(void);
    int GetNoMixes(void);
    void GetProb(MDLMM_T *imgData, double *probs);
protected:
};
}

#endif /* GGHIST_H_ */
