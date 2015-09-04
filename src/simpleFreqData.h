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
#ifndef SIMPLEFREQDATA
#define SIMPLEFREQDATA

#include "globalCVHeader.h"
#include "simpleFreqSettings.h"
#include <vector>

using namespace simplefreqsettings;

namespace simplefreqdata {
class freqdata {
public:
  cv::Mat hist;
  cv::Mat pdf;
  double normval;
  freqdata(void) :normval(-1.) {}
  void calc(const cv::Mat &img_bgr_f, const cv::Mat &mask, CS_SETTINGS &settings) {
    std::vector<cv::Mat> imgchannels=settings.convert(img_bgr_f);
    double min,max;
    for (int i=0;i<imgchannels.size();i++) {
      cv::minMaxLoc(imgchannels[i],&min,&max);
      //      std::cout <<"Min:"<<min<<" Max:"<<max<<std::endl;
    }
    cv::calcHist(&imgchannels[0],settings.nimages,settings.channels,mask,
        hist,settings.ndims,settings.histSize,settings.ranges,
        settings.isuniform,
        false);
    normval=cv::norm(hist,cv::NORM_L1);
    hist.convertTo(pdf,cv::DataType<float>::type,1./normval);
    std::cout <<hist<<std::endl;
  }
  void set(const cv::Mat &_pdf, double _normval) {
    normval=_normval;
    pdf=_pdf;
    hist=pdf*normval;
  }
  cv::Mat likelihoodimg(const cv::Mat &img_bgr_f, const cv::Mat &mask, CS_SETTINGS &settings) {
    assert(normval>0.);
    std::vector<cv::Mat> imgchannels=settings.convert(img_bgr_f);
    cv::Mat backProject;
    //   cv::calcBackProject(&imgchannels[0],settings.nimages,settings.channels,(1-pdf)*normval,backProject,settings.ranges,1.,settings.isuniform);
   cv::calcBackProject(&imgchannels[0],settings.nimages,settings.channels,hist,backProject,settings.ranges,1.,settings.isuniform);
    cv::Mat likelihood;
    backProject.convertTo(likelihood,cv::DataType<float>::type,1./normval);
    return likelihood;
  }
};


}
#endif //SIMPLEFREQDATA

