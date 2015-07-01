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
#ifndef GGHELPERS_NS
#define GGHELPERS_NS

#include "globalCVHeader.h"
#include "region.h"
#include <vector>
#include <string>

namespace gg {
  const int _nosubregions=4;

  void FGOutlines(IplImage *fgmap, IplImage *colorop);
  region::IDXT LabelHeads(IplImage *fgmap, region::REGIONS &regs, int numregions);
  void AddToPDFs(const cv::Mat &pdf, std::string &label, cv::Mat &helmet_pdf, cv::Mat &nohelmet_pdf, int &helmet_cnt, int &nohelmet_cnt);
  void AddToPDFs(const cv::Mat &pdf, std::string &label, cv::Mat helmet_pdfs[], cv::Mat nohelmet_pdfs[], int helmet_cnts[], int nohelmet_cnts[], int pdfidx);
  void Normalise(cv::Mat pdfs[], int cnts[], int nopdfs);
  void Normalise(cv::Mat &pdf, int cnt);

  region::IREGv PreInitSubRegions(const IplImage *region, int nosubregions);
  void SubRegions(const IplImage *region, region::IREGv &subregions);
  inline int GetQuadrant(const cv::Point &cog, int x, int y);

  void PreInitPDFs(cv::Mat pdfs[], int nosubregions, cv::Size size, int type);

  int GetLabelKey(std::string &label);
  //#######################################
  //#######################################
  void MinMaxScale(const cv::Mat &ipdata, cv::Mat &opdata, double newmax=255.);
  //#############################################
  //#############################################
  // min < val <= max
  inline size_t bincalc_binsize(const float &val, const float &min, const float &max, const float &binsize);
  //#############################################
  inline size_t nobins(const float &min, const float &max, const float &binsize);
  //#############################################
  inline size_t bincalc_nobins(const float &val, const float &min, const float &max, const float &nobins);
  //#############################################
  //#############################################
  void normalise(const cv::Mat &_hist, cv::Mat &_pdf);
  //##########################################
  template <class T> std::ostream& op_cvMat(std::ostream &os,const cv::Mat &data);
  template <class T, size_t N> std::ostream& op_cvMat(std::ostream &os,const cv::Mat_<cv::Vec<T,N> > &data);
  template <class T, int cn> std::ostream& op_cvVec(std::ostream &os, const cv::Vec<T,cn> &data);
  //##########################################
  template <class T> std::vector<T> pdfmoments(const cv::MatND &data);
}

namespace gg {
  //###############################################
  /// 2 3
  /// 1 0
  inline int GetQuadrant(const cv::Point &cog, int x, int y) {
    int quadrant=-1;
    if (x>cog.x && y>cog.y)
      quadrant=0;
    else if (x<=cog.x && y>cog.y)
      quadrant=1;
    else if (x<=cog.x && y<=cog.y)
      quadrant=2;
    else // if (x>cog.x && y<=cog.y)
      quadrant=3;
    return quadrant;
  }
  //###############################################
  
  //#############################################
  //#############################################
  // min < val <= max
  inline size_t bincalc_binsize(const float &val, const float &min, const float &max, const float &binsize) {
    return floor((val-min)/binsize);
  }
  //#############################################
  inline size_t nobins(const float &min, const float &max, const float &binsize) {
    return ceil((max-min)/binsize);
  }
  //#############################################
  inline size_t bincalc_nobins(const float &val, const float &min, const float &max, const float &nobins) {
    float binsize=ceil((max-min)/nobins);
    return floor((val-min)/binsize);
  }
  //#############################################
}

namespace gg {
  //##########################################
  //##########################################
  template <class T> std::ostream& op_cvMat(std::ostream &os,const cv::Mat &data) {
    for (int j=0;j<data.rows;j++) {
      const T *rawdata=data.ptr<T>(j);
      for (int i=0;i<data.cols;i++) {
  os <<rawdata[i]<<",";
      }
      os <<std::endl;
    }
    return os;
  }
  //##########################################
  template <class T, size_t N> std::ostream& op_cvMat(std::ostream &os,const cv::Mat_<cv::Vec<T,N> > &data) {
    for (size_t j=0;j<data.rows;j++) {
      for (size_t i=0;i<data.cols;i++) {
  for (size_t k=0;k<N;k++) 
    os <<data.template at<cv::Vec<T,N> >(j,i)[k]<<",";
  os <<" ";
      }
      os <<std::endl;
    }
  }
  //##########################################
  template <class T, int cn> std::ostream& op_cvVec(std::ostream &os, const cv::Vec<T,cn> &data) {
    for (int i=0;i<cn;i++)
      os <<data.val[i]<<",";
    os <<std::endl;
    return os;
  }
  //##########################################
  //##########################################
  //moments = mean_rows,mean_cols,stddev_rows,stddev_cols
  //or 
  //moments = mean_cols,stddev_cols
  template <class T> std::vector<float> pdfmoments(const cv::Mat &data) {
    assert(!data.empty());
    std::vector<float> moments;
    if (data.rows==1) {
      moments=std::vector<float>(2);
      moments[0]=0.;
      moments[1]=0.;
      const T *rawdata=data.ptr<T>(0);
      for (int i=0;i<data.cols;i++) {
  moments[0]+=rawdata[i]*float(i); //mu=sum x p(x)
  moments[1]+=rawdata[i]*pow(float(i),2.);
  //sum (x-mu)^2p(x)
  //=sum (x^2-2.x.mu+mu^2)p(x)
  //=sum x^2.p(x)-2.mu.sum x.p(x)+mu^2
  //=sum x^2.p(x)-mu^2
      }
      moments[1]-=pow(moments[0],2.);
    }
    else {
      moments=std::vector<float>(4);
      for (int i=0;i<4;i++)
  moments[i]=0.;
      for (int j=0;j<data.rows;j++) {
  const T* rawdata=data.ptr<T>(j);
  for (int i=0;i<data.cols;i++) {
    moments[0]+=rawdata[i]*float(i); //mu_x=sum x p(x,y)
    moments[1]+=rawdata[i]*float(j); //mu_y=sum y p(x,y)
    moments[2]+=rawdata[i]*pow(float(i),2.);
    moments[3]+=rawdata[i]*pow(float(j),2.);
  }
      }
      moments[2]-=pow(moments[0],2.);
      moments[3]-=pow(moments[1],2.);
    }
    return moments;
  }
//##########################################
//##########################################
}

#endif //GGHELPERS_NS

