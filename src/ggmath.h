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
#ifndef GGMATH
#define GGMATH

#include "globalCVHeader.h"
#include "ggutils.h"

namespace gg {
  //####################################
  //####################################
  class spectralcorr {
  public:
    spectralcorr(void) {
    }
    spectralcorr(const cv::Mat_<T_FEAT> &data) { 
      this->operator()(data);
    }
    void operator() (const cv::Mat_<T_FEAT> &data) { 
      if (data.cols==0 || data.rows==0) return;
      if (data.cols>data.rows)
  spect=data.t();
      else
  data.copyTo(spect);
      svd(spect,cv::SVD::FULL_UV);
      cv::Mat_<T_FEAT> wI(spect.rows,spect.cols);
      wI=0.;
      for (size_t i=0;i<spect.cols;i++) {
  if (svd.w.at<T_FEAT>(i,0)>1e-10)
    wI.at<T_FEAT>(i,i)=1.;
      }
      spect=svd.u*wI*svd.vt;
      if (data.cols>data.rows)
  spect=spect.t();
      calccorr();
    }
    const cv::Mat_<T_FEAT> getspect(void) const { return spect; }
    const cv::Mat_<T_FEAT> getcorr(void) const { return corr; }
  protected:
    void calccorr(void) {
      corr=cv::Mat_<T_FEAT>(spect.rows,spect.cols);
      corr_rows=cv::Mat_<T_FEAT>(spect.rows,spect.cols);
      corr=0.;
      corr_rows=0.;
      
      for (size_t j=0;j<spect.rows;j++) {
  size_t maxidx=gg::maxrowidx<T_FEAT>(spect.row(j));
  corr_rows.at<T_FEAT>(j,maxidx)=1.;
      }
      for (size_t i=0;i<spect.cols;i++) {
  size_t maxidx=gg::maxcolidx<T_FEAT>(spect.col(i));
  if (corr_rows.at<T_FEAT>(maxidx,i)>0.)
    corr.at<T_FEAT>(maxidx,i)=1.;
      }

    }
    cv::SVD svd;
    cv::Mat_<T_FEAT> spect;
    cv::Mat_<T_FEAT> corr;
    cv::Mat_<T_FEAT> corr_rows;
  };
  //####################################
  //####################################
  template <class T> void rownormalise(cv::Mat_<T> &m) {
    for (size_t j=0;j<m.rows;j++) {
      cv::Mat r=m.row(j);
      cv::normalize(r,r,1.,0.,cv::NORM_L1);
    }
  }
  //############################################
  //############################################
  template <class T, size_t N> void marginalise(cv::Mat_<cv::Vec<T,N> > &ip, cv::Mat_<T> &op) {
    assert(ip.rows==op.rows && ip.cols==op.cols);
    for (size_t j=0;j<ip.rows;j++) {
      for (size_t i=0;i<ip.cols;i++) {
  op.template at<T>(j,i)=0.;
  for (size_t n=0;n<N;n++)
    op.template at<T>(j,i)+=ip.template at<cv::Vec<T,N> >(j,i)[n];
      }
    }
  }
  //##############################################################################################
  //##############################################################################################
  template <typename _Tp, int cn> cv::Vec<_Tp,cn> centraldiff(const cv::Vec<_Tp,cn> &previous, const cv::Vec<_Tp,cn> &next) {
    return (next-previous)*.5f;
  }
  //##############################################################################################
  template <typename _Tp, int cn> cv::Vec<_Tp,cn> interpolate(const cv::Vec<_Tp,cn> &previous, const cv::Vec<_Tp,cn> &diff) {
    return previous+diff;
  }
  //##############################################################################################
  template <typename _Tp, int cn> cv::Vec<_Tp,cn> diff(const cv::Vec<_Tp,cn> &preprevious, const cv::Vec<_Tp,cn> &previous) {
    return previous-preprevious;
  }
  //##############################################################################################
  //##############################################################################################
}


#endif //GGMATH
