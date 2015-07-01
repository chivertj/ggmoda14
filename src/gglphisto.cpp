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
#include "gglphisto.h"
#include "gghelpers.h"

namespace gg {
  void LPHisto::operator() (const LowPass &lowpass, const cv::Mat &mask, int _nbins) {
    assert(lowpass.GetLowPassScaled().size()==mask.size());
    assert(mask.type()==cv::DataType<uchar>::type);
    assert(lowpass.GetLowPassScaled().type()==cv::DataType<uchar>::type);
    int nbins=_nbins;
    if (hist.dims==0) {
      const int sizes[]={nbins};
      hist=cv::MatND(1,sizes,cv::DataType<float>::type,cv::Scalar(0.));
    }
    else hist=cv::Scalar(0.);
    float ranges[]={0.,255.};
    const cv::Mat img=lowpass.GetLowPassScaled();
    int imgdims[]={img.rows,img.cols};
    size_t v;
    for (size_t y=0;y<imgdims[0];y++) {
      const uchar *rawdata=img.ptr<uchar>(y);
      const uchar *rawmask=mask.ptr<uchar>(y);
      for (size_t x=0;x<imgdims[1];x++) {
  if (rawmask[x]>0) {
    v=bincalc_nobins(float(rawdata[x]),ranges[0],ranges[1],nbins);
    hist.at<float>(v)++;
  }
      }
    }
    if (pdf.dims==0)
      pdf=cv::MatND(hist.dims,hist.size,hist.type(),cv::Scalar(0.));
    cv::Mat mhist(hist);
    cv::Mat mpdf(pdf);
    gg::normalise(mhist,mpdf);
  }
  void LPHisto::CalcDefault1D(void) {
    default1d=cv::Mat(pdf).t();
  }
}
