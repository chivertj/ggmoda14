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
//modified 2010/07/28
//# added spatial standard deviation calculations

#ifndef REGION_HEADER
#define REGION_HEADER
#include "globalCVHeader.h"

//#define REGIONEROSTREAM

#include <iostream>
#include <list>
#include <vector>
using std::vector;

#include <boost/numeric/ublas/matrix.hpp>
#include <boost/numeric/ublas/io.hpp>
#include <boost/numeric/ublas/matrix_proxy.hpp>
#include <boost/numeric/ublas/vector.hpp>
using boost::numeric::ublas::matrix;
using boost::numeric::ublas::zero_matrix;
using boost::numeric::ublas::matrix_row;
using boost::numeric::ublas::matrix_column;

namespace region {
typedef vector<int> IDXT;
typedef vector<IDXT> ComponentIndexs;
typedef cv::Point RegionPnt;
typedef vector<RegionPnt> REGIONPNTS;
typedef vector<REGIONPNTS> REGIONS;
typedef REGIONPNTS::iterator PNTIT;
typedef REGIONS::iterator REGIT;
typedef IplImage* IREG;
typedef vector<IREG> IREGv;
typedef IREGv::iterator IREGit;

typedef std::list<IREG> IREGl;
typedef IREGl::iterator IREGlit;

typedef cv::Mat MREG;
typedef vector<MREG> MREGv;
typedef MREGv::iterator MREGit;

typedef matrix<IREG> IREGm;
typedef matrix<int> IDXm;

const int MAXNOREGIONS=50;
const int MAXNOREGIONSB4TRIM=100;
const int MINREGIONSIZE=50;

struct regionscontainer {
  REGIONS regs;
  size_t nregs;
  regionscontainer(void) : regs(MAXNOREGIONS),nregs(0) {}
};

enum REGMATCHT { DIST, SHAPE, FEATURESET };

typedef vector<float> vPR;
typedef matrix<float> mPR;

template <class T>
void SetZero(T* data, const int N) {
  memset((void*)data,0,N*sizeof(T));
}

template <class T>
void cvPntToMat(T *pnt, CvMat *mat) {}
template <> inline void cvPntToMat<CvPoint2D32f>(CvPoint2D32f *pnt, CvMat *mat) {
  memcpy(mat->data.fl,pnt,2*sizeof(float));
}


template <class T, class A>
void cvPntToMat(T *pnt, CvMat *mat) {}
template <> inline
void cvPntToMat<CvPoint2D32f,float>(CvPoint2D32f *pnt, CvMat *mat) {
  memcpy(mat->data.fl,pnt,2*sizeof(float));
}
template <> inline
void cvPntToMat<CvPoint,float>(CvPoint *pnt, CvMat *mat) {
  mat->data.fl[0]=pnt->x;
  mat->data.fl[1]=pnt->y;
}

template <class T>
void cvMatToPnt(CvMat *mat, T *pnt) {}
template <> inline void cvMatToPnt<CvPoint2D32f>(CvMat *mat, CvPoint2D32f *pnt) {
  memcpy(pnt,mat->data.fl,2*sizeof(float));
}

template <class T, class A>
void cvMatToPnt(CvMat *mat, T *pnt) {}
template <> inline
void cvMatToPnt<CvPoint2D32f,float>(CvMat *mat, CvPoint2D32f *pnt) {
  memcpy(pnt,mat->data.fl,2*sizeof(float));
}
template <> inline
void cvMatToPnt<CvPoint,float>(CvMat *mat, CvPoint *pnt) {
  pnt->x=mat->data.fl[0];
  pnt->y=mat->data.fl[1];
}

void ReadImages(IREGv &imgvect, std::string imglistfile, bool iscolor=false);
void ReadImages(MREGv &imgvect, std::string imglistfile, bool iscolor=false);
void DestroyImages(IREGv &imgvect);
template <class T> void DestroyImages(T &imgs) {
  for (typename T::iterator it=imgs.begin();it!=imgs.end();++it)
    cvReleaseImage(&(*it));
}
void ExportImages(const IREGv &imgvect, std::string basefilename, int pad=3);
void ExportImages(std::list<IplImage*> &imglist, std::string basefilename, int pad=3);
void ExportImage(const IplImage *img, const std::string &basefilename, int pad, int no);
void ExportImage(const cv::Mat &img, const std::string &basefilename, int pad, int no);

void Align(const REGIONPNTS &d, REGIONS &m, int nd);
CvPoint CalcCenter(const REGIONPNTS &r);
void Translate(REGIONPNTS &r, int h, int v);

void SetBinImg(const REGIONPNTS &a, IplImage *img, bool clear=true);
void ExtractPnts(const cv::Mat_<uchar> &img, REGIONPNTS &r, bool clear);

std::ostream& operator<<(std::ostream &os, const IDXT &data);

CvPoint2D32f CalcSD(const REGIONPNTS &r, int nopntstoinclude=-1);
CvPoint MinXMinY(const REGIONPNTS &r, int nopntstoinclude=-1);
CvPoint MaxXMaxY(const REGIONPNTS &r, int nopntstoinclude=-1);

template <class T> cv::Point MinXMinY(const cv::Mat &img);
template <class T> cv::Point MaxXMaxY(const cv::Mat &img);
template <class T> cv::Point COG2D(const cv::Mat &img);
template <class T> cv::Point_<T> COG2D(const std::vector<cv::Point_<T> > &pnts);

///Point type conversion, from float to non float.
template <size_t N, class A, class B> void pntsR2I(const A &a, B &b) {
  for (size_t i=0;i<N;i++) {
    b[i].x=cvRound(a[i].x);
    b[i].y=cvRound(a[i].y);
  }
}


class Rects {
public:
  typedef cv::RotatedRect R;
  typedef std::vector<R> RECTS;
  void addrect(const R &r) {
    rects.push_back(r);
  }
  inline static R makeRotatedRect(const REGIONPNTS &r) {
    cv::Mat_<RegionPnt> regpnts(r);
    return Rects::makeRotatedRect<RegionPnt>(regpnts);
  }
  template <class T> inline static R makeRotatedRect(const cv::Mat_<T> &r) {
    assert(cv::DataType<T>::type==CV_32SC2||cv::DataType<T>::type==CV_32FC2);
    R tmpR=cv::minAreaRect(r);
    Rects::angleCorrect(tmpR);
    return tmpR;
  }
  const RECTS& makeRects(const REGIONS &regs) {
    rects.resize(regs.size());
    for (size_t i=0;i<regs.size();i++) {
      if (regs[i].size()>0)
        rects[i]=Rects::makeRotatedRect(regs[i]);
      else
        rects[i]=R(cv::Point2f(0.f,0.f),cv::Size2f(0.f,0.f),0);
    }
    return rects;
  }
  static bool isempty(const R &r) {
    if (r.size.area()<0.0001)
      return true;
    return false;
  }
  inline static R measurementToRotatedRect(const cv::Mat_<float> &m) {
    return cv::RotatedRect(cv::Point2f(m(0,0),m(1,0)), cv::Size2f(m(2,0),m(3,0)), m(4,0));
  }
  static cv::Mat_<float> getMatrixVals(const R &r) {
    cv::Mat_<float> m(5,1);
    m(0,0)=r.center.x;
    m(1,0)=r.center.y;
    m(2,0)=r.size.width;
    m(3,0)=r.size.height;
    m(4,0)=r.angle;
    return m;
  }
  static R& angleCorrect(R &r) {
    if (r.angle<-45) {
      double tmpheight=r.size.height;
      r.size.height=r.size.width;
      r.size.width=tmpheight;
      r.angle+=90;
    }
    return r;
  }
  static void recttocorners(const cv::RotatedRect &r, cv::Point corners[5]) {
    CvPoint2D32f ptsf[4];
    cvBoxPoints(r,ptsf);
    pntsR2I<4>(ptsf,corners);
    corners[4]=corners[0];
  }
  static void drawrect(const cv::RotatedRect &r, cv::Mat &img, cv::Scalar color=cv::Scalar::all(255)) {
    cv::Point ptsi[5];
    Rects::recttocorners(r,ptsi);
    cv::fillConvexPoly(img,ptsi,5,color);
  }
  RECTS& getrects(void) { return rects; }
  const RECTS& getrects(void) const { return rects; }
  cv::Size getuprightmaxdims(void) {
    cv::Size maxdims(0,0);
    for (size_t i=0;i<rects.size();i++) {
      cv::Size size=rects[i].boundingRect().size();
      if (size.width>maxdims.width)
        maxdims.width=size.width;
      if (size.height>maxdims.height)
        maxdims.height=size.height;
    }
    return maxdims;
  }
  static void constrainrectvals(cv::Rect &rect, int imgwidth, int imgheight) {
    if (rect.x<0) rect.x=0;
    if (rect.y<0) rect.y=0;
    if (rect.x+rect.width>=imgwidth)
      rect.x-=rect.x+rect.width-imgwidth+1;
    if (rect.y+rect.height>=imgheight)
      rect.y-=rect.y+rect.height-imgheight+1;
  }
protected:
  RECTS rects;
};

///combines two rotated rectangles together
cv::RotatedRect mergeRects(const cv::RotatedRect R1, const cv::RotatedRect R2);

//specialize to cv::Point in the cv::Mat...
template <class T> void cvMat2REGIONPNTS(const cv::Mat_<T> &ip, REGIONPNTS &op) {
  op=REGIONPNTS(ip.rows);
  for (size_t i=0;i<ip.rows;i++) {
    op[i].x=ip(i,0);
    op[i].y=ip(i,1);
  }
}

template <> inline void cvMat2REGIONPNTS<cv::Point>(const cv::Mat_<cv::Point> &ip, REGIONPNTS &op) {
  op=REGIONPNTS(ip.rows);
  for (size_t i=0;i<ip.rows;i++) {
    op[i].x=ip(i,0).x;
    op[i].y=ip(i,0).y;
  }
}
}
//
namespace region {
//################################################
template <class T> cv::Point MinXMinY(const cv::Mat &img) {
  cv::Point maxs(0,0);
  for (int j=0;j<img.rows;j++) {
    const T *rawdata=img.ptr<T>(j);
    for (int i=0;i<img.cols;i++) {
      if (rawdata[i]) {
        if (i>maxs.x) maxs.x=i;
        if (j>maxs.y) maxs.y=j;
      }
    }
  }
  return maxs;
}
//################################################
template <class T> cv::Point MaxXMaxY(const cv::Mat &img) {
  cv::Point mins(img.cols,img.rows);
  for (int j=img.rows-1;j>=0;j--) {
    const T *rawdata=img.ptr<T>(j);
    for (int i=img.cols-1;i>=0;i--) {
      if (rawdata[i]) {
        if (i<mins.x) mins.x=i;
        if (j<mins.y) mins.y=j;
      }
    }
  }
  return mins;
}
//################################################
template <class T> cv::Point COG2D(const cv::Mat &img) {
  cv::Point cog(0,0);
  int cogcnt=0;
  for (int j=0;j<img.rows;j++) {
    const T *rawdata=img.ptr<T>(j);
    for (int i=0;i<img.cols;i++) {
      if (rawdata[i]) {
        cog.x+=i;
        cog.y+=j;
        cogcnt++;
      }
    }
  }
  cog.x/=float(cogcnt);
  cog.y/=float(cogcnt);
  return cog;
}
//################################################
template <class T> cv::Point_<T> COG2D(const std::vector<cv::Point_<T> > &pnts) {
  cv::Point_<T> cog(0,0);
  for (size_t i=0;i<pnts.size();i++)
    cog+=pnts[i];
  cog*=1./float(pnts.size());
  return cog;
}
//################################################
template <class T> void MeanWeightedPoint(const cv::Point_<T> &a, float prop, cv::Point_<T> b) {
  b.x=b.x*prop+a.x*(1.-prop);
  b.y=b.y*prop+a.y*(1.-prop);
}
//################################################
template <class T> void AppendMatRowDir(const cv::Mat_<T> &a, cv::Mat_<T> &b) {
  assert(a.cols==b.cols);
  if (a.rows>0) {
    cv::Mat newmat(a.rows+b.rows,a.cols,cv::DataType<T>::type);
    cv::Mat newmatrange=newmat.rowRange(0,a.rows);
    a.copyTo(newmatrange);
    newmatrange=newmat.rowRange(a.rows,a.rows+b.rows);
    b.copyTo(newmatrange);
    newmatrange.copyTo(b);
  }
}
//################################################
void MergePointSets(const cv::Mat &a, cv::Mat &b);
//################################################
}
#endif
