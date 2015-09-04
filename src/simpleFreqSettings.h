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
#ifndef SIMPLEFREQSETTINGS
#define SIMPLEFREQSETTINGS

#include "globalCVHeader.h"
#include <vector>

#define LUV_BACKPROJECTING
//#define LOGBGR_BACKPROJECTING
//#define BGR_BACKPROJECTING
//#define HSV_BACKPROJECTING

namespace simplefreqsettings {
class luv_settings {
public:
  int ubins,vbins; //=30,sbins=30;
  int histSize[2]; //={hbins,sbins};
  float uranges[2]; //={0,180};
  float vranges[2]; //={0,256};
  const float *ranges[2]; //={hranges,sranges};
  int channels[2]; //={0,1};
  int ndims;
  int nimages; //={2};
  bool isuniform;
  int cvtype;
  luv_settings(void) :
    ubins(30),vbins(30),
    histSize{ubins,vbins},
    uranges{-30,70},vranges{-30,70},
    ranges{uranges,vranges},
      nimages(2),
	channels{0,1},
    ndims(2),
      isuniform(true),
      cvtype(CV_BGR2Luv)
    {
    }
    std::vector<cv::Mat> convert(const cv::Mat &img_bgr_f) {
      cv::Mat imgf;
      imgf=img_bgr_f/255.;
      cv::cvtColor(imgf,imgf,cvtype);
      std::vector<cv::Mat> imgchannels(3);
      split(imgf,imgchannels);
      std::vector<cv::Mat> prunedchannels(2);
      prunedchannels[0]=imgchannels[1];
      prunedchannels[1]=imgchannels[2];
      return prunedchannels;
    }
};

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
  int nimages;
  log_bgr_settings(void) :
    bbins(NBINS),gbins(NBINS),rbins(NBINS),
    histSize{bbins,gbins,rbins},
    //      branges({0,256}),granges({0,256}),rranges({0,256}),
    ranges{branges,granges,rranges},
    channels{0,1,2},
    ndims(3),
      isuniform(false),
      nimages(3)
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
    }
    std::vector<cv::Mat> convert(const cv::Mat &img_bgr_f) {
      cv::Mat imgf;
      imgf=img_bgr_f.clone(); //255.;
      std::vector<cv::Mat> imgchannels(3);
      split(imgf,imgchannels);
      return imgchannels;
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
  int nimages;
  bgr_settings(void) :
    bbins(10),gbins(10),rbins(10),
    histSize{bbins,gbins,rbins},
    branges{0,256},granges{0,256},rranges{0,256},
    ranges{branges,granges,rranges},
    channels{0,1,2},
    ndims(3),
      isuniform(true),
      nimages(3)
    {
    }
    std::vector<cv::Mat> convert(const cv::Mat &img_bgr_f) {
      cv::Mat imgf;
      imgf=img_bgr_f.clone(); //255.;
      std::vector<cv::Mat> imgchannels(3);
      split(imgf,imgchannels);
      return imgchannels;
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
  int cvtype;
  int nimages;
  hsv_settings(void) :
    hbins(30),sbins(30),
    histSize{hbins,sbins},
      //    hranges{0,180},sranges{0,256},
    hranges{0,360},sranges{0,1.},
    ranges{hranges,sranges},
    channels{0,1},
    ndims(2),
      isuniform(true),
      cvtype(CV_BGR2HSV),
      nimages(2)
    {
    }
    void dump(std::ostream &os) {
      os <<hbins<<","<<sbins<<","<<histSize[0]<<","<<histSize[1]<<","<<hranges[0]<<","<<hranges[1]<<","<<sranges[0]<<","<<sranges[1]<<",";
      os <<ranges[0][0]<<","<<ranges[0][1]<<","<<ranges[1][0]<<","<<ranges[1][1]<<",";
      os <<channels[0]<<","<<channels[1]<<std::endl;
    }
    std::vector<cv::Mat> convert(const cv::Mat &img_bgr_f) {
      cv::Mat imgf;
      imgf=img_bgr_f.clone(); //255.;
      cv::cvtColor(imgf,imgf,cvtype);
      std::vector<cv::Mat> imgchannels(3);
      split(imgf,imgchannels);
      std::vector<cv::Mat> prunedchannels(2);
      prunedchannels[0]=imgchannels[0];
      prunedchannels[1]=imgchannels[1];
      return prunedchannels;
    }
};
#ifdef LUV_BACKPROJECTING
 typedef luv_settings CS_SETTINGS;
#endif
#ifdef BGR_BACKPROJECTING
 typedef bgr_settings CS_SETTINGS;
#endif
#ifdef HSV_BACKPROJECTING
 typedef hsv_settings CS_SETTINGS;
#endif
#ifdef LOGBGR_BACKPROJECTING
 typedef log_bgr_settings CS_SETTINGS;
#endif
}
#endif //SIMPLEFREQSETTINGS

