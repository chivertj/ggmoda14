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
#include "Thinning.h"

#include <iostream>
using namespace std;
#include "../ACDefinitions.h"
using namespace ACDefinitions;

CThinning::CThinning(void) : m_image(0), m_result(0),
			     BYTEMATRIX(1),BM_SET(255),BM_NOTSET(0)
{
}

CThinning::~CThinning(void) {
    if (m_result) cvReleaseImage(&m_result);
}

void CThinning::Init(IplImage *image) {
    m_image=image;
    if (m_result) cvReleaseImage(&m_result);
    m_result=cvCloneImage(m_image);
    cvZero(m_result);
}

void CThinning::PerformOp(void) {
  IplImage *imageCpy=cvCreateImage(cvGetSize(m_image),IPL_DEPTH_8U,1);
  cvConvertScale(m_image,imageCpy);
  BinaryThin(imageCpy);
  //  BinaryThin(m_result);
  MakeOnePixel(imageCpy);
  cvConvertScale(imageCpy,m_result);
  cvReleaseImage(&imageCpy);
}

IplImage* CThinning::BorrowResultImage(void) {
    return m_result;
}

void CThinning::BinaryThin(IplImage *image)
{
  assert(image);
  bool KeepOn=true;
  int pixCnt, trans, turn=0;
  IMAGEBYTE_T nghbrs[9]; 
  IplImage *workImage=cvCloneImage(image);
  cvZero(workImage);
  ZeroBorder(image);
  int minI=1, maxI=image->width - 1, I=image->width;
  int minJ=1, maxJ=image->height - 1;
  int pos;
  IMAGEBYTE_T *rawImg=(IMAGEBYTE_T*)(image->imageData);
  IMAGEBYTE_T *rawWrkImg=(IMAGEBYTE_T*)(workImage->imageData);
  while (KeepOn) {
    turn=(turn+1)%2;
    KeepOn = false;
    for(int j=minJ; j<maxJ; j++) {
      for (int i=minI; i<maxI; i++) {
	pos=j*I+i;
	if (rawImg[pos] == BM_SET) {
	  GetNineNeighbours(nghbrs,(IMAGEBYTE_T*)(rawImg),i,j,I);
	  pixCnt = CountNeighbours(nghbrs);
	  if (pixCnt>=2 && pixCnt<=6) {
	    trans=CountNeighbourTransitions(nghbrs);
	    if (trans==1) {
	      if (!turn && (nghbrs[3]==BM_NOTSET || nghbrs[5]==BM_NOTSET || (nghbrs[1]==BM_NOTSET && nghbrs[7]==BM_NOTSET))) {
		rawWrkImg[pos]=BM_SET;
		KeepOn = true; 
	      }
	      if (turn && (nghbrs[1]==BM_NOTSET || nghbrs[7]==BM_NOTSET || (nghbrs[3]==BM_NOTSET && nghbrs[5]==BM_NOTSET))) {    
		rawWrkImg[pos]=BM_SET;
		KeepOn = true; 
	      }	 
	    }
	  }
	}
      }
    }

    for(int j=minJ; j<maxJ; j++) {
      for (int i=minI; i<maxI; i++) {
	pos=j*I+i;
	if (rawWrkImg[pos]==BM_SET) {
	  rawWrkImg[pos]=BM_NOTSET;
	  rawImg[pos]=BM_NOTSET;
	}
      }
    }

  }

  ZeroBorder(image);
  cvReleaseImage(&workImage);    
}

inline
void CThinning::GetNineNeighbours(IMAGEBYTE_T *neighbours, IMAGEBYTE_T *data, int x, int y, int Xstep) {
    neighbours[0]=data[(y-1)*Xstep+x-1];
    neighbours[1]=data[(y)  *Xstep+x-1];
    neighbours[2]=data[(y+1)*Xstep+x-1];
    neighbours[3]=data[(y+1)*Xstep+x];
    neighbours[4]=data[(y+1)*Xstep+x+1];
    neighbours[5]=data[(y)  *Xstep+x+1];
    neighbours[6]=data[(y-1)*Xstep+x+1];
    neighbours[7]=data[(y-1)*Xstep+x];
    neighbours[8]=data[(y-1)*Xstep+x-1];
}

inline
int CThinning::CountNeighbourTransitions(IMAGEBYTE_T *neighbours) {
    int trans = 0;
    for (int p=0;p<=7;p++)
	if (neighbours[p]==BM_NOTSET && neighbours[p+1]==BM_SET) trans++;
    return trans;
}

inline
int CThinning::CountNeighbours(IMAGEBYTE_T *neighbours) {
    int neighboursCnt=0;
    for (int p=0;p<7;p++)
	if (neighbours[p]==BM_SET) neighboursCnt++;
    return neighboursCnt;
}

void CThinning::ZeroBorder(IplImage *image) {
  assert(image);
  CvScalar borderVal=cvScalarAll(0);
  int width=image->width,height=image->height;
  int x,y;
  for (y=0;y<height;y++) {
    x=0;
    cvSet2D(image,y,x,borderVal);
    x=width-1;
    cvSet2D(image,y,x,borderVal);
  }
  for (x=0;x<width;x++) {
    y=0;
    cvSet2D(image,y,x,borderVal);
    y=height-1;
    cvSet2D(image,y,x,borderVal);
  }
}

//--------------------------------------------------------------------
// 0 0 1       1 0 0
// 0 1 1       1 1 0
// 1 1 0       0 1 1
//But prob just same as these:
//        0 0 0     1 1 0     0 0 1     0 1 0      
//        0 1 1     0 1 1     0 1 1     0 1 1
//        1 1 0     0 0 0     0 1 0     0 0 1
// should check if part of junction e.g.
// 0 1 0 1
// 0 1 1 0
// 0 0 1 0
// ok if
// 0 1 0 0
// 0 1 1 1
// 0 0 1 0

// 0 0 0    0 0 0    0 1 0   0 1 0  
// 1 1 0    0 1 1    1 1 0   0 1 1
// 0 1 0    0 1 0    0 0 0   0 0 0

// 0 1 0    0 0 0    0 1 0   0 1 0
// 1 1 1    1 1 1    0 1 1   1 1 0
// 0 0 0    0 1 0    0 1 0   0 1 0

void CThinning::MakeOnePixel(IplImage *image) {
  assert(image);

  IplImage *tmpResult=cvCloneImage(image);
  int X=tmpResult->width;
  int Y=tmpResult->height;

  IMAGEBYTE_T *rawRes=(IMAGEBYTE_T*)(image->imageData);
  IMAGEBYTE_T *rawTmpRes=(IMAGEBYTE_T*)(tmpResult->imageData);
  int pos;
  for (int y=2;y<(Y-2);y++) {
    for (int x=2;x<(X-2);x++) {
      pos=y*X+x;
      if (rawTmpRes[pos]==BM_SET) {
	if (rawTmpRes[pos+1]==BM_SET && rawTmpRes[pos+X]==BM_SET && rawTmpRes[pos+X-1]==BM_SET) {
	  if (rawTmpRes[pos+X*2+1]==BM_NOTSET)
	    rawRes[pos+X]=BM_NOTSET;
	}
	if (rawTmpRes[pos-X]==BM_SET && rawTmpRes[pos-X-1]==BM_SET && rawTmpRes[pos+1]==BM_SET) {
	  if (rawTmpRes[pos-X*2+1]==BM_NOTSET)
	    rawRes[pos-X]=BM_NOTSET;
	}
	if (rawTmpRes[pos+1]==BM_SET && rawTmpRes[pos-X+1]==BM_SET && rawTmpRes[pos+X]==BM_SET) {
	  if (rawTmpRes[pos+X+2]==BM_NOTSET)
	    rawRes[pos+1]=BM_NOTSET;
	}
	if (rawTmpRes[pos+1]==BM_SET && rawTmpRes[pos+X+1]==BM_SET && rawTmpRes[pos-X]==BM_SET) {
	  if (rawTmpRes[pos-X+2]==BM_NOTSET)
	    rawRes[pos+1]=BM_NOTSET;
	}
	if (rawTmpRes[pos+X+1]==BM_NOTSET && rawTmpRes[pos+X-1]==BM_NOTSET && rawTmpRes[pos-X-1]==BM_NOTSET && rawTmpRes[pos-X+1]==BM_NOTSET) {
	  if (((rawTmpRes[pos-X]==BM_NOTSET && rawTmpRes[pos+X]==BM_SET) ||
	       (rawTmpRes[pos+X]==BM_NOTSET && rawTmpRes[pos-X]==BM_SET)) &&
	      ((rawTmpRes[pos+1]==BM_NOTSET && rawTmpRes[pos-1]==BM_SET) || 
	       (rawTmpRes[pos+1]==BM_SET && rawTmpRes[pos-1]==BM_NOTSET)))
	    rawRes[pos]=BM_NOTSET;
	  if ((rawTmpRes[pos+1]==BM_SET && rawTmpRes[pos-1]==BM_SET) && ((rawTmpRes[pos+X]==BM_SET && rawTmpRes[pos-X]==BM_NOTSET) || (rawTmpRes[pos-X]==BM_SET && rawTmpRes[pos+X]==BM_NOTSET)))
	    rawRes[pos]=BM_NOTSET;
	  else if ((rawTmpRes[pos+X]==BM_SET && rawTmpRes[pos-X]==BM_SET) && ((rawTmpRes[pos+1]==BM_SET && rawTmpRes[pos-1]==BM_NOTSET) || (rawTmpRes[pos-1]==BM_SET && rawTmpRes[pos+1]==BM_NOTSET)))
	    rawRes[pos]=BM_NOTSET;
	}

      }
    }
  }
  for (int y=2;y<(Y-2);y++) {
    for (int x=2;x<(X-2);x++) {
      pos=y*X+x;
      if (rawTmpRes[pos]==BM_SET) {
	if (rawTmpRes[pos+X+1]==BM_NOTSET && rawTmpRes[pos+X-1]==BM_NOTSET && rawTmpRes[pos-X-1]==BM_NOTSET && rawTmpRes[pos-X+1]==BM_NOTSET) {
	  if ((rawTmpRes[pos+1]==BM_SET && rawTmpRes[pos-1]==BM_SET) && ((rawTmpRes[pos+X]==BM_SET && rawTmpRes[pos-X]==BM_NOTSET) || (rawTmpRes[pos-X]==BM_SET && rawTmpRes[pos+X]==BM_NOTSET)))
	    rawRes[pos]=BM_NOTSET;
	  else if ((rawTmpRes[pos+X]==BM_SET && rawTmpRes[pos-X]==BM_SET) && ((rawTmpRes[pos+1]==BM_SET && rawTmpRes[pos-1]==BM_NOTSET) || (rawTmpRes[pos-1]==BM_SET && rawTmpRes[pos+1]==BM_NOTSET)))
	    rawRes[pos]=BM_NOTSET;
	}

      }
    }
  }
  cvReleaseImage(&tmpResult);
}

