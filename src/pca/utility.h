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
#ifndef CVUTILS
#define CVUTILS

#include "../globalCVHeader.h"
#include "../region.h"

#if(0)
namespace region {
  typedef std::vector<IplImage*> IREGv;
  typedef std::vector<cv::Mat> MREGv;
  typedef MREGv::iterator MREGit;
}
#endif

namespace cvutils {
  //convert to column format
  //convert to cv::mat
  //combine into one matrix
  template <class T> cv::Mat_<T> imgs2columndata(const region::IREGv &imgvect) {
    assert(!imgvect.empty());
    int X=imgvect[0]->width, Y=imgvect[0]->height, N=imgvect.size();
    cv::Mat dataout(X*Y,imgvect.size(),cv::DataType<T>::type);
    cv::Mat mTimg(Y,X,cv::DataType<T>::type);
    cv::Mat mcol(X*Y,1,cv::DataType<T>::type);
    for (int idx=0;idx<N;idx++) {
      cv::Mat img(imgvect[idx]);
      img.convertTo(mTimg,mTimg.type());
      mcol=mTimg.reshape(1,X*Y);
      cv::Mat columndataout=dataout.colRange(cv::Range(idx,idx+1));
      mcol.copyTo(columndataout);
    }
    return dataout;
  }
  //#########################
  int getdims(const region::MREGv &imgvect, size_t &X, size_t &Y, size_t &N);
  int getdims(const region::MREGv &imgvect, size_t &X, size_t &Y, size_t &N, int &nchannels);
  //#########################
  template<class T> void convertvectT(const region::MREGv &imgvectin, region::MREGv &imgvectout) {
    imgvectout.resize(imgvectin.size());
    for (size_t i=0;i<imgvectout.size();i++)
      imgvectin[i].convertTo(imgvectout[i],cv::DataType<T>::type);
  }
  //#########################
  template<class T> void convertvectT(const region::MREGv &imgvectin, region::MREGv &imgvectout, int nchannels) {
    imgvectout.resize(imgvectin.size());
    for (size_t i=0;i<imgvectout.size();i++)
      imgvectin[i].convertTo(imgvectout[i],CV_MAKETYPE(cv::DataType<T>::depth,nchannels));
  }
  //#########################
  /// Converts a vector of images (cv::Mat) to a column matrix.
  /// If any images are empty, then not copied
  /// Type of cv::Mat in imgvect assumed to be same as T
  template <class T> cv::Mat_<T> imgs2columndata(const region::MREGv &imgvect) {
    assert(!imgvect.empty());
    size_t X=0,Y=0,N=0;
    getdims(imgvect,X,Y,N);
    cv::Mat_<T> dataout(X*Y,N);
    for (size_t i=0,n=0;i<imgvect.size();i++) {
      if (!imgvect[i].empty()) {
        assert(cv::DataType<T>::type==imgvect[i].type());
        cv::Mat_<T> columndataout=dataout.colRange(cv::Range(n,n+1));
        imgvect[i].reshape(1,X*Y).copyTo(columndataout);
        n++;
      }
    }
    return dataout;
  }
  //#######################
  /// Converts a vector of masked colour images (cv::Mat) to a column matrix.
  /// If any images are empty, then not copied
  /// Type of cv::Mat in imgvect assumed to be same as T
#if(1)
  template <class T> cv::Mat_<T> imgs2columndata(const region::MREGv &binaryimgvect, const region::MREGv &pixelimgvect) {
    assert(!binaryimgvect.empty() && !pixelimgvect.empty());
    size_t X=0,Y=0,N=0;
    int nchannels;
    getdims(pixelimgvect,X,Y,N,nchannels);
    cv::Mat_<T> dataout(X*Y*nchannels,N);
    for (size_t i=0,n=0;i<pixelimgvect.size();i++) {
      if (!binaryimgvect[i].empty()) {
        assert(cv::DataType<T>::depth==pixelimgvect[i].depth());
        cv::Mat_<T> columndataout=dataout.colRange(cv::Range(n,n+1));
        pixelimgvect[i].reshape(1,X*Y*nchannels).copyTo(columndataout);
        n++;
      }
    }
    return dataout;
  }
#endif
  //#######################
  typedef std::vector<bool> EMPTYIMGS;
  EMPTYIMGS getemptyimgs(const region::MREGv &imgvect);
  void insertemptyimgs(const EMPTYIMGS &emptyimgs, region::MREGv &imgvect);
  void removeemptyimgs(const region::MREGv &imgvect, region::MREGv &nonemptyimgvect, EMPTYIMGS &emptyimgs, bool copy);
  //#######################
  template <class T> void columndata2imgs(const cv::Mat_<T> &coldata, region::MREGv &imgvect, size_t Y) {
    if (coldata.cols==0) return;
    imgvect.resize(coldata.cols);
    for (size_t i=0;i<coldata.cols;i++) {
      cv::Mat column=coldata.colRange(cv::Range(i,i+1)).clone();
      imgvect[i]=column.reshape(1,Y);
      //      std::cout <<"**************"<<std::endl;
      //      std::cout <<cv::Mat_<T>(imgvect[i])<<std::endl;
      //      std::cout <<"**************"<<std::endl;
    }
  }
  //#######################
  template <class T, int nchannels> void columndata2imgs(const cv::Mat_<T> &coldata, region::MREGv &imgvect, size_t Y) {
    if (coldata.cols==0) return;
    imgvect.resize(coldata.cols);
    for (size_t i=0;i<coldata.cols;i++) {
      cv::Mat column=coldata.colRange(cv::Range(i,i+1)).clone();
      imgvect[i]=column.reshape(nchannels,Y);
      //      std::cout <<"**************"<<std::endl;
      //      std::cout <<cv::Mat_<T>(imgvect[i])<<std::endl;
      //      std::cout <<"**************"<<std::endl;
    }
  }
  //#######################
  template <class T> void columndata2imgs(const cv::Mat_<T> &coldata, region::MREGv &imgvect, size_t Y, int nchannels) {
    if (coldata.cols==0) return;
    imgvect.resize(coldata.cols);
    for (size_t i=0;i<coldata.cols;i++) {
      cv::Mat column=coldata.colRange(cv::Range(i,i+1)).clone();
      std::cout <<column.rows<<"->"<<nchannels<<"x"<<Y<<"x?"<<std::endl;

      imgvect[i]=column.reshape(nchannels,Y);
      //      std::cout <<"**************"<<std::endl;
      //      std::cout <<cv::Mat_<T>(imgvect[i])<<std::endl;
      //      std::cout <<"**************"<<std::endl;
    }
  }
  //#######################
}

const std::string SEP(",");
#include <iostream>
std::ostream& operator<<(std::ostream& os, const cv::Mat &m);
std::ostream& operator<<(std::ostream& os, const cv::Size &s);

template <class T> std::ostream& operator<<(std::ostream& os, const cv::Mat_<T> &m) {
  for (size_t j=0;j<m.rows;j++) {
    for (size_t i=0;i<m.cols;i++) {
      os <<m(j,i)<<SEP;
    }
    os <<std::endl;
  }
  return os;
}

std::ostream& operator<<(std::ostream& os, const cv::Size &s);


#include <vector>
template <class T> std::ostream& operator<<(std::ostream& os, const std::vector<T> &v) {
  for (size_t i=0;i<v.size();i++)
    std::cout <<v[i]<<SEP;
  std::cout <<std::endl;
  return os;
}

#endif
