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
#include "SignedDistT.h"
#include <assert.h>
#include "../ACDefinitions.h"
using namespace ACDefinitions;
#include <iostream>
using namespace std;

//++++++++++++++++++++++++++++++++++
CSignedDistT::CSignedDistT(void) 
    : m_img(0), m_sgndDistT(0), m_sgnMap(0), m_initSgndDistT(0), m_initCurve(0), m_X(-1), m_Y(-1), m_contourVal(255), m_rawInitLevelSet(0)
{
}
//++++++++++++++++++++++++++++++++++
CSignedDistT::~CSignedDistT(void) {
    if (m_sgndDistT) cvReleaseImage(&m_sgndDistT);
    if (m_sgnMap) cvReleaseImage(&m_sgnMap);
    if (m_initSgndDistT) cvReleaseImage(&m_initSgndDistT);
    if (m_initCurve) cvReleaseImage(&m_initCurve);
}
//++++++++++++++++++++++++++++++++++
void CSignedDistT::Init(IplImage *img, IMAGEBYTE_T contourVal) {
    assert(img->nChannels==1);
    assert(img->depth==IPL_DEPTH_8U);
    m_img=img;
    if (m_sgndDistT) cvReleaseImage(&m_sgndDistT);
    m_sgndDistT=cvCreateImage(cvGetSize(m_img),ACTIVE_IPLDEPTH,1);
    if (m_sgnMap) cvReleaseImage(&m_sgnMap);
    m_sgnMap=cvCreateImage(cvGetSize(m_img),ACTIVE_IPLDEPTH,1);
    if (m_initSgndDistT) cvReleaseImage(&m_initSgndDistT);
    m_initSgndDistT=cvCreateImage(cvGetSize(m_img),ACTIVE_IPLDEPTH,1);
    if (m_initCurve) cvReleaseImage(&m_initCurve);
    m_initCurve=cvCreateImage(cvGetSize(m_img),IPL_DEPTH_32S,1);
    m_X=m_img->width;
    m_Y=m_img->height;
    m_contourVal=contourVal;
    m_mask=std::vector<IMAGEDATA_T>(5);
    m_rawInitLevelSet = (IMAGEDATA_T*)(m_initSgndDistT->imageData);
}
//++++++++++++++++++++++++++++++++++
void CSignedDistT::PerformOp(void) {
    IMAGEINT32S_T *rawInitCurve = (IMAGEINT32S_T*)(m_initCurve->imageData);
    IMAGEBYTE_T *rawInitSnake=(IMAGEBYTE_T*)(m_img->imageData);
    int pos;
    for(int j=0;j<m_Y;j++) {
  for(int i=0;i<m_X;i++){
      pos = j*m_X+i;
      rawInitCurve[pos] = rawInitSnake[pos];
      if(rawInitSnake[pos]!=m_contourVal)
    m_rawInitLevelSet[pos] = SIGNDIST_INFINITY;
      else
    m_rawInitLevelSet[pos] = 0;
  }
    }

    CalcSignMap();

    for(int j=0;j<m_Y;j++) {
  for(int i=0;i<m_X;i++) {
      pos=j*m_X+i;
      m_rawInitLevelSet[pos] = minForward(pos, i, j);
  }
    }
    for(int j=m_Y-1;j>=0;j--) {
  for(int i=m_X-1;i>=0;i--) {
      pos=j*m_X+i;
      m_rawInitLevelSet[pos] = minBackward(pos, i, j);
  }
    }
#if(0)
    IplImage *tmpImg=cvCloneImage(m_sgndDistT);
    cvMul(m_initSgndDistT, m_sgnMap, tmpImg);
    cvSmooth(tmpImg,m_sgndDistT,CV_GAUSSIAN,7);
    cvReleaseImage(&tmpImg);
#else
    cvMul(m_initSgndDistT, m_sgnMap, m_sgndDistT);
#endif
}
//++++++++++++++++++++++++++++++++++
IplImage* CSignedDistT::BorrowSignedDistT(void) {
    return m_sgndDistT;
}
//++++++++++++++++++++++++++++++++++
void CSignedDistT::CalcSignMap(void) {
// buildSignMap(int[][] initCurv){

    IMAGEDATA_T *rawSignMap = (IMAGEDATA_T*)(m_sgnMap->imageData);
    IMAGEINT32S_T *rawInitCurve = (IMAGEINT32S_T*)(m_initCurve->imageData);

    if(rawInitCurve[0]==0) rawInitCurve[0] = SIGNDIST_POSITIVE;
    else if(rawInitCurve[m_X*(m_Y-1)]==0) rawInitCurve[m_X*(m_Y-1)] = SIGNDIST_POSITIVE;
    else if(rawInitCurve[m_X-1]==0) rawInitCurve[m_X-1] = SIGNDIST_POSITIVE;
    else if(rawInitCurve[m_X*(m_Y-1)+m_X-1]==0) rawInitCurve[m_X*(m_Y-1)+m_X-1] = SIGNDIST_POSITIVE;

    int cnt, pos;

    do{
  cnt = 0;
  for(int j=0;j<m_Y;j++) {
      for(int i=0;i<m_X;i++){
    pos=j*m_X+i;
    if(rawInitCurve[pos]==SIGNDIST_POSITIVE){
        if((i+1<m_X) && (rawInitCurve[pos+1]==0)){
      rawInitCurve[pos+1] = SIGNDIST_POSITIVE;
      cnt ++;
        }
        if((i-1>=0) && (rawInitCurve[pos-1]==0)){
      rawInitCurve[pos-1] = SIGNDIST_POSITIVE;
      cnt ++;
        }
        if((j+1<m_Y) && (rawInitCurve[pos+m_X]==0)){
      rawInitCurve[pos+m_X] = SIGNDIST_POSITIVE;
      cnt ++;
        }
        if((j-1>=0) && (rawInitCurve[pos-m_X]==0)){
      rawInitCurve[pos-m_X] = SIGNDIST_POSITIVE;
      cnt ++;
        }
    }
      }
  }

    }while(cnt>0);

    for(int j=0;j<m_Y;j++) {
  for(int i=0;i<m_X;i++){
      pos=j*m_X+i;
      if(rawInitCurve[pos]==SIGNDIST_POSITIVE)
    rawSignMap[pos] = 1.0f;
      else rawSignMap[pos] = -1.0f;
  }
    }
}
//++++++++++++++++++++++++++++++++++
