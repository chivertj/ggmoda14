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
#ifndef CSHAPECONTOUR_CLASS
#define CSHAPECONTOUR_CLASS

#include "../globalCVHeader.h"
#include "../ACDefinitions.h"
using namespace ACDefinitions;
#include "WJImgNS.h"

template <class PNT_T> class CShapeContour {
public:
  CShapeContour(void);
  virtual ~CShapeContour(void);
  void Init(IplImage *levSet);
  void InitWithMask(IplImage *mask);
  void PerformOp(float zerothreshold=0.);
  IplImage* BorrowContourImg(void);
  ACPnt<PNT_T>* BorrowContourPnts(int &noPnts);
protected:
  IplImage *m_levSet;
  IplImage *m_maskfl;
  ACPnt<PNT_T> *m_contourPnts;
  int m_noContourPnts;
  IplImage *m_contourImg;
  void CalcContour(void);
  void ExtractContourPnts(void);
  int m_X,m_Y;
//  void ZeroBorder(IplImage *image);
  float m_zerothreshold;
  bool m_usemask;
};
//:::::::::::::::::::::::::::::::::::::::::::::::::
template <class PNT_T> CShapeContour<PNT_T>::CShapeContour(void) :m_levSet(0),m_maskfl(0),m_contourPnts(0),m_noContourPnts(0),m_contourImg(0),m_X(0),m_Y(0),m_zerothreshold(0.),m_usemask(false){
}
//:::::::::::::::::::::::::::::::::::::::::::::::::
template <class PNT_T> CShapeContour<PNT_T>::~CShapeContour(void) {
  if (m_contourImg) cvReleaseImage(&m_contourImg);
  if (m_contourPnts) delete [] m_contourPnts;
  cvReleaseImage(&m_maskfl);
}
//:::::::::::::::::::::::::::::::::::::::::::::::::
template <class PNT_T> void CShapeContour<PNT_T>::Init(IplImage *levSet) {
  m_levSet=levSet;
  if (m_X!=m_levSet->width || m_Y!=m_levSet->height) {
    m_X=m_levSet->width;
    m_Y=m_levSet->height;
    if (m_contourImg) { cvReleaseImage(&m_contourImg); m_contourImg=0; }
    m_contourImg=cvCreateImage(cvGetSize(m_levSet),ACTIVE_IPLDEPTH,1); 
    cvZero(m_contourImg);
  }
  if (m_contourPnts) { delete [] m_contourPnts; m_contourPnts=0; m_noContourPnts=0; }
  m_usemask=false;
}
//:::::::::::::::::::::::::::::::::::::::::::::::::
template <class PNT_T> void CShapeContour<PNT_T>::InitWithMask(IplImage *mask) {
  if (!m_maskfl || mask->width!=m_X || mask->height!=m_Y) {
    if (m_maskfl) cvReleaseImage(&m_maskfl);
    m_maskfl=cvCreateImage(cvGetSize(mask),ACTIVE_IPLDEPTH,1);
    m_X=m_maskfl->width;
    m_Y=m_maskfl->height;
    if (m_contourImg) cvReleaseImage(&m_contourImg);
    m_contourImg=cvCreateImage(cvGetSize(mask),ACTIVE_IPLDEPTH,1); 
    cvZero(m_contourImg);
  }
  cvConvertScale(mask,m_maskfl);
  if (m_contourPnts) { delete [] m_contourPnts; m_contourPnts=0; m_noContourPnts=0; }
  m_usemask=true;
}
//:::::::::::::::::::::::::::::::::::::::::::::::::
template <class PNT_T> void CShapeContour<PNT_T>::PerformOp(float zerothreshold) {
  m_zerothreshold=zerothreshold;
  CalcContour();
  ExtractContourPnts();
}
//:::::::::::::::::::::::::::::::::::::::::::::::::
template <class PNT_T> IplImage* CShapeContour<PNT_T>::BorrowContourImg(void) {
  return m_contourImg;
}
//:::::::::::::::::::::::::::::::::::::::::::::::::
template <class PNT_T> ACPnt<PNT_T>* CShapeContour<PNT_T>::BorrowContourPnts(int &noPnts) {
  noPnts=m_noContourPnts;
  return m_contourPnts;
}
//:::::::::::::::::::::::::::::::::::::::::::::::::
template <class PNT_T> void CShapeContour<PNT_T>::CalcContour(void) {
  assert(m_contourImg);
  IMAGEDATA_T *rawContour=(IMAGEDATA_T*)(m_contourImg->imageData);
  cvZero(m_contourImg);
  IplImage *procImg=0;
  if (!m_usemask) {
    procImg=cvCloneImage(m_levSet);
    cvThreshold(m_levSet,procImg,m_zerothreshold,255.,CV_THRESH_BINARY_INV); //quite good with -.5 -> 255 and no smoothing
  } else { //mask set
    procImg=cvCloneImage(m_maskfl);
    cvThreshold(m_maskfl,procImg,128,255,CV_THRESH_BINARY);
  }
  WJImgNS::SetBorder(procImg,cvScalarAll(0.),10);

  IMAGEDATA_T *rawProcImg=(IMAGEDATA_T*)(procImg->imageData);
  int pos;
  IMAGEDATA_T nbourCnt;
  for (int y=1;y<m_Y-1;y++) {
    for (int x=1;x<m_X-1;x++) {
      pos=y*m_X+x;
      if (rawProcImg[pos]==255.f) {
        nbourCnt=0.;
        for (int in_y=-1;in_y<=1;in_y++) {
    for (int in_x=-1;in_x<=1;in_x++) {
      if ((in_y!=0 || in_x!=0) && rawProcImg[(y+in_y)*m_X+(x+in_x)]==0.f) //  && rawProcImg[(y+in_y)*m_X+(x+in_x)]>=0.f) 
        nbourCnt++;
    }
        }
  if (nbourCnt>0.f) 
    rawProcImg[pos]=(-1)*nbourCnt;
      }
    }
  }

  for (int y=1;y<m_Y-1;y++) {
    for (int x=1;x<m_X-1;x++) {
      pos=y*m_X+x;
      if (rawProcImg[pos]<-1.f) {
        rawContour[pos]=CONTOURVAL;
      }
      else if (rawProcImg[pos]<0.f) {
//(y-1,x-1),(y-1,x+1)->(y-1,x)
//(y-1,x+1),(y+1,x+1)->(y,x+1)
//(y+1,x+1),(y+1,x-1)->(y+1,x)
//(y+1,x-1),(y-1,x-1)->(y,x-1)
      if (rawProcImg[(y-1)*m_X+x-1]<0.f && rawProcImg[(y-1)*m_X+x+1]<0.f) // && rawProcImg[(y-1)*m_X+x]==255.f)
    rawContour[(y-1)*m_X+x]=CONTOURVAL;
      else if (rawProcImg[(y-1)*m_X+x+1]<0.f && rawProcImg[(y+1)*m_X+x+1]<0.f) // && rawProcImg[y*m_X+x+1]==255.f)
    rawContour[y*m_X+x+1]=CONTOURVAL;
      else if (rawProcImg[(y+1)*m_X+x+1]<0.f && rawProcImg[(y+1)*m_X+x-1]<0.f) // && rawProcImg[(y+1)*m_X+x]==255.f)
    rawContour[(y+1)*m_X+x]=CONTOURVAL;
      else if (rawProcImg[(y+1)*m_X+x-1]<0.f && rawProcImg[(y-1)*m_X+x-1]<0.f) // && rawProcImg[y*m_X+x-1]==255.f)
    rawContour[y*m_X+x-1]=CONTOURVAL;
      }
    }
  }
//  cvCopy(procImg,m_contourImg);
#if(0)

  int in_pos;
  for (int y=1;y<m_Y-1;y++) {
    for (int x=1;x<m_X-1;x++) {
      pos=y*m_X+x;
      if (rawProcImg[pos]<-1.f)
  rawContour[pos]=CONTOURVAL;
      else if (rawProcImg[pos]<0.f) {
  nbourCnt=0.f;
  for (int in_y=-1;in_y<=1;in_y++) {
    for (int in_x=-1;in_x<=1;in_x++) {
      in_pos=(y+in_y)*m_X+(x+in_x);
      if ((in_y!=0 || in_x!=0) && rawProcImg[in_pos]<0.f) {
        nbourCnt+=rawProcImg[in_pos];
      }
    }
  }
  if (nbourCnt<-6.f)
    rawContour[pos]=CONTOURVAL;
      }
    }
  }
#endif
  cvReleaseImage(&procImg);
}
//:::::::::::::::::::::::::::::::::::::::::::::::::
template <class PNT_T> void CShapeContour<PNT_T>::ExtractContourPnts(void) {
  assert(m_contourImg);
  IMAGEDATA_T *rawContour=(IMAGEDATA_T*)(m_contourImg->imageData);
  int pos,pntCntr=0;
  for (pos=0;pos<m_X*m_Y;pos++) {
    if (rawContour[pos]==CONTOURVAL)
      pntCntr++;
  }
  if (m_contourPnts) { delete [] m_contourPnts; m_contourPnts=0; m_noContourPnts=0; }
  m_noContourPnts=pntCntr;
  pntCntr=0;
  m_contourPnts=new ACPnt<PNT_T>[m_noContourPnts];
  for (int y=0;y<m_Y;y++) {
    for (int x=0;x<m_X;x++) {
      pos=y*m_X+x;
      if (rawContour[pos]==CONTOURVAL) {
  m_contourPnts[pntCntr].x=(PNT_T)(x);
  m_contourPnts[pntCntr].y=(PNT_T)(y);
  pntCntr++;
      }
    }
  }
}
//:::::::::::::::::::::::::::::::::::::::::::::::::
#endif //CSHAPECONTOUR_CLASS
