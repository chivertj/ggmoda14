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
#ifndef MDBACKPROJECTOR
#define MDBACKPROJECTOR

#include "../globalCVHeader.h"
#include "../ggsettings.h"
#include "BootStrapProjector.h"

#define LOGBGR_BOOSTING
//#define BGR_BOOSTING
//#define HSV_BOOSTING

namespace md {
class log_bgr_settings {
public:
  static const int NBINS=20;
  int bbins,gbins,rbins; //=30,sbins=30;
  int histSize[3]; //={bbins,gbins,rbins};
  float branges[NBINS+1]; //={0,256};
  float granges[NBINS+1]; //={0,256};
  float rranges[NBINS+1]; //={0,256};
  const float *ranges[3]; //={branges,granges,rranges};
  int channels[3]; //={0,1,2};
  int ndims;
  bool isuniform;
  log_bgr_settings(void) :
    bbins(NBINS),gbins(NBINS),rbins(NBINS),
    histSize{bbins,gbins,rbins},
    //      branges({0,256}),granges({0,256}),rranges({0,256}),
    ranges{branges,granges,rranges},
    channels{0,1,2},
    ndims(3),
    isuniform(false)
    {
      int hfNBINS=cvRound(NBINS/2.);
      float maxval=255.f;
      float hfmax=cvFloor(maxval/2.);
      branges[0]=0;
      granges[0]=0;
      rranges[0]=0;
      branges[NBINS]=maxval+1;
      granges[NBINS]=maxval+1;
      rranges[NBINS]=maxval+1;
      for (size_t i=1;i<hfNBINS+1;i++) {
        branges[i]=cvRound(pow(10.,i/float(hfNBINS)*log(hfmax)/log(10.)));
        granges[i]=cvRound(pow(10.,i/float(hfNBINS)*log(hfmax)/log(10.)));
        rranges[i]=cvRound(pow(10.,i/float(hfNBINS)*log(hfmax)/log(10.)));
        branges[NBINS-i]=maxval-branges[i];
        granges[NBINS-i]=maxval-granges[i];
        rranges[NBINS-i]=maxval-rranges[i];
      }
      for (size_t i=0;i<NBINS+1;i++)
        std::cout <<branges[i]<<std::endl;
    }
};

class bgr_settings {
public:
  int bbins,gbins,rbins; //=30,sbins=30;
  int histSize[3]; //={bbins,gbins,rbins};
  float branges[2]; //={0,256};
  float granges[2]; //={0,256};
  float rranges[2]; //={0,256};
  const float *ranges[3]; //={branges,granges,rranges};
  int channels[3]; //={0,1,2};
  int ndims;
  bool isuniform;
  bgr_settings(void) :
    bbins(10),gbins(10),rbins(10),
    histSize{bbins,gbins,rbins},
    branges{0,256},granges{0,256},rranges{0,256},
    ranges{branges,granges,rranges},
    channels{0,1,2},
    ndims(3),
    isuniform(true)
    {
    }
};
class hsv_settings {
public:
  int hbins,sbins; //=30,sbins=30;
  int histSize[2]; //={hbins,sbins};
  float hranges[2]; //={0,180};
  float sranges[2]; //={0,256};
  const float *ranges[2]; //={hranges,sranges};
  int channels[2]; //={0,1};
  int ndims;
  bool isuniform;
  hsv_settings(void) :
    hbins(30),sbins(30),
    histSize{hbins,sbins},
    hranges{0,180},sranges{0,256},
    ranges{hranges,sranges},
    channels{0,1},
    ndims(2),
    isuniform(true)
    {
    }
    void dump(std::ostream &os) {
      os <<hbins<<","<<sbins<<","<<histSize[0]<<","<<histSize[1]<<","<<hranges[0]<<","<<hranges[1]<<","<<sranges[0]<<","<<sranges[1]<<",";
      os <<ranges[0][0]<<","<<ranges[0][1]<<","<<ranges[1][0]<<","<<ranges[1][1]<<",";
      os <<channels[0]<<","<<channels[1]<<std::endl;
    }
};
#ifdef BGR_BOOSTING
typedef bgr_settings CS_SETTINGS;
#endif
#ifdef HSV_BOOSTING
typedef hsv_settings CS_SETTINGS;
#endif
#ifdef LOGBGR_BOOSTING
typedef log_bgr_settings CS_SETTINGS;
#endif
class freqdata {
public:
  cv::Mat hist;
  cv::Mat pdf;
  double normval;
  freqdata(void) :normval(-1.) {}
  void calc(const cv::Mat &img, const cv::Mat &mask, CS_SETTINGS &settings) {
    assert(cv::countNonZero(mask)>0);
    cv::calcHist(&img,1,settings.channels,mask,
        hist,settings.ndims,settings.histSize,settings.ranges,
        settings.isuniform,
        false);
    normval=cv::norm(hist,cv::NORM_L1);
    hist.convertTo(pdf,cv::DataType<float>::type,1./normval);
  }
  //img can be different
  //mask can also be different (but not actually used!)
  cv::Mat likelihoodimg(const cv::Mat &img, const cv::Mat &mask, CS_SETTINGS &settings) {
    assert(normval>0.);
    assert(cv::countNonZero(mask)>0);
    cv::Mat backProject;
    cv::calcBackProject(&img,1,settings.channels,hist,backProject,settings.ranges,1.,settings.isuniform);
    cv::Mat likelihood;
    backProject.convertTo(likelihood,cv::DataType<float>::type,1./normval);
    return likelihood;
  }
};
cv::Mat fgboosting(const cv::Mat &initfg, const cv::Mat &img, CS_SETTINGS &settings);

#ifdef QUICKHISTO_BACKPROJECTOR
class BackProjector {
public:
  BackProjector(const cv::Rect &_rect) {}
  virtual ~BackProjector(void) {}
  void setimg(const cv::Mat &_img) {
    bgrdataimg=_img;
    cv::cvtColor(bgrdataimg,hsvdataimg,CV_BGR2HSV);
  }
  void operator() (const cv::Mat &_initfg) {
    initfg=_initfg;
#ifdef BGR_BOOSTING
    result=fgboosting(initfg, bgrdataimg, settings);
#endif
#ifdef HSV_BOOSTING
    result=fgboosting(initfg, hsvdataimg, settings);
#endif
#ifdef LOGBGR_BOOSTING
    result=fgboosting(initfg, bgrdataimg, settings);
#endif
#ifndef BGR_BOOSTING
#ifndef HSV_BOOSTING
#ifndef LOGBGR_BOOSTING
    assert(0);
#endif
#endif
#endif
  }
  const cv::Mat& operator() (void) {return result;}
protected:
  cv::Mat bgrdataimg,initfg,result,hsvdataimg;
  CS_SETTINGS settings;
};

#endif


#ifdef EMHISTO_BACKPROJECTOR
class BackProjector {
public:
  BackProjector(const cv::Rect &_rect) : rect(_rect),halo(_rect.size(),cv::DataType<uchar>::type),comb(_rect.size(),cv::DataType<uchar>::type) {chalo=IplImage(halo); ccomb=IplImage(comb);}
  virtual ~BackProjector(void) {}
  void setimg(const cv::Mat &_img) { assert(_img.size()==halo.size()); img=_img;cimg=IplImage(img); bsp.Init(&cimg); }
  void operator() (const cv::Mat &_initfg) {
    assert(_initfg.size()==halo.size());
    assert(_initfg.size()==comb.size());
    initfg=_initfg;
    //make the halo
    cv::dilate(initfg,comb,cv::Mat(),cv::Point(-1,-1),10);
    cv::subtract(comb,initfg,halo,comb);
    //backproject...
    cinitfg=IplImage(initfg);
    std::cout <<"PerformOp BackProjector:"<<cv::countNonZero(halo)<<","<<cv::countNonZero(initfg)<<","<<cv::countNonZero(comb)<<std::endl;
    bsp.PerformOp(&chalo,&cinitfg,&ccomb);
    std::cout <<"Finished PerformOp BackProjector"<<std::endl;
    result=cv::Mat(bsp.BorrowResult()); //,true); //copy data not needed
  }
  const cv::Mat& operator() (void) {return result;}
protected:
  BackProjector(void);
  BackProjector(const BackProjector &);
  const cv::Rect rect;
  cv::Mat initfg;
  cv::Mat halo;
  cv::Mat comb;
  cv::Mat img;
  IplImage cimg;
  IplImage chalo,cinitfg,ccomb;
  CBootStrapProjector bsp;
  cv::Mat result;
};
#endif //EMHISTO_BACKPROJECTOR
}
#endif //MDBACKPROJECTOR

