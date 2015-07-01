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
#ifndef GGUTILS
#define GGUTILS

#include "globalCVHeader.h"
#include <iostream>

namespace gg {
  typedef float T_PDF;
  typedef uchar T_BINT; //binary template
  typedef float T_IMG;
  typedef float T_FEAT;
  typedef int T_CORRVIDX;
  const int CVT_PDF=cv::DataType<T_PDF>::type;
  const int CVT_BINT=cv::DataType<T_BINT>::type;
  const int CVT_IMG=cv::DataType<T_IMG>::type;
  const int CVT_FEAT=cv::DataType<T_FEAT>::type;
  typedef std::vector<T_FEAT> FEATVEC;
  typedef std::vector<T_CORRVIDX> CORRS;
  size_t inc(const size_t &limit, const size_t &idx);
  size_t dec(const size_t &limit, const size_t &idx);

  //#######################################
  template <class T, size_t N> 
  cv::Vec<T,N> rawtocvvec(T *data) {
    cv::Vec<T,N> d;
    memcpy(d.val,data,sizeof(cv::Vec<T,N>));
    return d;
  }
  //#######################################
  template <class T, size_t N>
  void rawtocvvec(T *data, cv::Vec<T,N> &d) {
    memcpy(d.val,data,sizeof(cv::Vec<T,N>));
  }
  //#######################################
  template <class S, class T, size_t N>
  //  cv::Vec<T,N> structtocvvec(S &s, cv::Vec<T,N> &v) {
  cv::Vec<T,N> structtocvvec(S &s) {
    return rawtocvvec<T,N>((T*)(&s));
  }
  //#######################################
  template <class S, class T, size_t N>
  void structtocvvec(S &s, cv::Vec<T,N> &v) {
    rawtocvvec<T,N>((T*)(&s),v);
  }
  //############################################
  //############################################
  template <class T> size_t maxrowidx(const cv::Mat &row) { 
    assert(row.rows==1); 
    size_t maxidx=0; 
    const T *r=row.ptr<T>(0);
    T  maxval=row.ptr<T>(0)[0];
    for (size_t i=0;i<row.cols;i++) {
      if (r[i]>maxval) {
  maxval=r[i];
  maxidx=i;
      }
    }
    return maxidx;
  }
  //############################################
  template <class T> size_t maxcolidx(const cv::Mat &col) { 
    assert(col.cols==1); 
    size_t maxidx=0; 
    T maxval=col.ptr<T>(0)[0];
    for (size_t i=0;i<col.rows;i++) {
      if (col.ptr<T>(i)[0]>maxval) {
  maxval=col.ptr<T>(i)[0];
  maxidx=i;
      }
    }
    return maxidx;
  }
  //############################################
  template <class T> void maxrowmaxcol(const cv::Mat &data, cv::Mat &max) {
    assert(data.rows==max.rows && data.cols==max.cols);
    assert(data.type()==cv::DataType<T>::type && max.type()==cv::DataType<T>::type);
    cv::Mat_<T> max_rows(data.rows,data.cols);
    max=cv::Scalar(0.);

    for (size_t j=0;j<data.rows;j++) {
      size_t maxidx=gg::maxrowidx<T>(data.row(j));
      max_rows.template at<T>(j,maxidx)=1.;
    }
    for (size_t i=0;i<data.cols;i++) {
      size_t maxidx=gg::maxcolidx<T>(data.col(i));
      if (max_rows.template at<T>(maxidx,i)>0.)
  max.at<T>(maxidx,i)=1.;
    }
  }
  //############################################
  //############################################
  std::ostream& printmatdetails(std::ostream &os, const cv::Mat &m);
  //############################################
  //############################################
  int refcount(const cv::Mat &m);
  //############################################
  /// e.g. BinaryPtrFuncIO currentfunc=cv::bitwise_or;
  typedef void (*BinaryPtrFuncIO)(const cv::_InputArray&, const cv::_InputArray&, const cv::_OutputArray&, const cv::_InputArray&);
  //############################################
  class cvtColorFunctor {
  public:
    cvtColorFunctor(int _code) : code(_code),dstCn(0) {}
    cv::Mat operator() (const cv::Mat &src) {
      if (src.empty())
        return cv::Mat();
      cv::cvtColor(src,res,code,dstCn);
      return res.clone();
    }
  protected:
    cvtColorFunctor(void);
    int code,dstCn;
    cv::Mat res;
  };
  //############################################
  template <class T>
  class changeColorFunctor {
  public:
    changeColorFunctor(const T &_newcolor, const T &_nullcolor) : newcolor(_newcolor),nullcolor(_nullcolor) {}
    cv::Mat operator() (const cv::Mat &src) {
      if (src.empty())
        return cv::Mat();
      res=src.clone();
      for (int y=0;y<res.rows;y++) {
        T *row=(T*)(res.ptr(y));
        for (int x=0;x<res.cols;x++) {
          if (row[x]!=nullcolor)
            row[x]=newcolor;
        }
      }
      return res.clone();
    }
  protected:
    changeColorFunctor(void);
    T newcolor,nullcolor;
    cv::Mat res;
  };
  //############################################
  class BinaryPtrFunctorIO {
  public:
    BinaryPtrFunctorIO(BinaryPtrFuncIO _f) : f(_f) {}
    cv::Mat operator() (const cv::Mat &src1, const cv::Mat &src2, const cv::Mat &mask=cv::Mat()) {
      if (src1.empty() && src2.empty())
        return cv::Mat();
      else if (src1.empty())
        return src2.clone();
      else if (src2.empty())
        return src1.clone();
      f(src1,src2,res,mask);
      return res.clone();
    }
  protected:
    BinaryPtrFunctorIO(void);
    cv::Mat res;
    BinaryPtrFuncIO f;
  };
  //############################################
  template <typename T> cv::Mat bitwise(T func, const cv::Mat &src1, const cv::Mat &src2, const cv::Mat &mask=cv::Mat()) {
    cv::Mat res;
    func(src1,src2,res,mask);
    return res;
  }
  //############################################
}

#endif //GGUTILS

