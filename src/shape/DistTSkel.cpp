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
#include "DistTSkel.h"
#include "../ACDefinitions.h"
using namespace ACDefinitions;

#include <iostream>
using namespace std;
#include "Thinning.h"
//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
CDistTSkel::CDistTSkel(void) 
  :
  m_distT(0), m_skel(0), m_distSkel(0), m_inverse(false), BYTEMATRIX(1),BM_SET(255),BM_NOTSET(0),m_minMaxSkelVal(-5.)
{
}
//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
CDistTSkel::~CDistTSkel(void) {
  if (m_skel) cvReleaseImage(&m_skel);
  if (m_distSkel) cvReleaseImage(&m_distSkel);
}
//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
void CDistTSkel::Init(IplImage *distT, bool inverse) {
  assert(distT);
  m_distT=distT;
  if (m_skel && (m_skel->width!=m_distT->width || m_skel->height!=m_distT->height)) {
    cvReleaseImage(&m_skel);
    m_skel=cvCloneImage(m_distT);
    cvReleaseImage(&m_distSkel);
    m_distSkel=cvCloneImage(m_distT);
  }
  else if (!m_skel) {
    m_skel=cvCloneImage(m_distT);
    m_distSkel=cvCloneImage(m_distT);
  }
  cvZero(m_skel);
  cvZero(m_distSkel);
  m_inverse=inverse;
}
//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
void CDistTSkel::PerformOp(void) {
  if (m_inverse) {
    LocalMax(m_distT, .75f, 1.01f, m_skel, true, -2);
    ConnectPnts(m_skel, 1.f, 1.351f, m_distT, true,0);
    Thin();
    RemoveIsolatedPoints();
    CreateDistSkel();
    RemoveBoundaryPnts(m_minMaxSkelVal,false);
  }
  else {
    LocalMax(m_distT, -.75f, -1.01f, m_skel, false, 2);
    ConnectPnts(m_skel, -1.f, -1.351f, m_distT, false,0);
    Thin();
    RemoveIsolatedPoints();
    CreateDistSkel();
  }
}
//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
IplImage* CDistTSkel::BorrowSkel(void) {
  return m_skel;
}
//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
void CDistTSkel::ZeroBorder(IplImage *image) {
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
//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
void CDistTSkel::RemoveIsolatedPoints(void) {
  assert(m_skel);
  int width=m_skel->width,height=m_skel->height;
  IMAGEDATA_T *rawSkel=(IMAGEDATA_T*)(m_skel->imageData);
  int pos,in_pos;
  bool has_neighbour;
  for (int y=1;y<height-1;y++) {
    for (int x=1;x<width-1;x++) {
      pos=y*width+x;
      if (rawSkel[pos]>0.) {
	has_neighbour=false;
	for (int in_y=-1;in_y<=1;in_y++) {
	  for (int in_x=-1;in_x<=1;in_x++) {
	    if (!(in_y==0 && in_x==0)) {
	      in_pos=(y+in_y)*width+x+in_x;
	      if (rawSkel[in_pos]>0.) {
		has_neighbour=true;
	      }
	    }
	  }
	}
	if (!has_neighbour)
	  rawSkel[pos]=0.;
      }
    }
  }
}
//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
void CDistTSkel::LocalMax(IplImage *img, float dist1, float dist2, IplImage *localMax, bool inverse, float threshold) {
  assert(img);
  assert(localMax);
  assert(img->width==localMax->width && img->height==localMax->height);
  IMAGEDATA_T *rawImg=(IMAGEDATA_T*)(img->imageData);
  IMAGEDATA_T *rawLocalMax=(IMAGEDATA_T*)(localMax->imageData);
  cvZero(localMax);
  int X=img->width;
  int Y=img->height;
  int pos,in_pos;
  float activeDist;
  float *nbourVals=new float [8];
  int nbourIdx;
  bool localMaxFlag;
  for (int y=1; y<Y-1;y++) {
    for (int x=1;x<X-1;x++) {
      pos=y*X+x;
      nbourIdx=0;
      localMaxFlag=true;
      for (int in_y=-1;in_y<=1;in_y++) {
	for (int in_x=-1;in_x<=1;in_x++) {
	  if (in_x!=0 || in_y!=0) {
	    in_pos=(y+in_y)*X+x+in_x;
	    if (in_x==0 || in_y==0) 
	      activeDist=dist1;
	    else
	      activeDist=dist2;
	    nbourVals[nbourIdx]=activeDist+rawImg[in_pos];
	    if ((nbourVals[nbourIdx]>rawImg[pos] && !inverse) || (nbourVals[nbourIdx]<rawImg[pos] && inverse)) 
	      localMaxFlag=false;
	    nbourIdx++;
	  }
	  if (!localMaxFlag) break;
	}
	if (!localMaxFlag) break;
      }
      if ((inverse && localMaxFlag && rawImg[pos]<threshold) || (!inverse && localMaxFlag && rawImg[pos]>threshold)) {
	rawLocalMax[pos]=255;
      }
    }
  }
  delete [] nbourVals;
}
//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
bool CDistTSkel::GrowPnts(IplImage *img, float dist1, float dist2, IplImage *distImg, bool inverse, float threshold) {
  assert(img);
  assert(distImg);
  assert(img->width==distImg->width && img->height==distImg->height);
  IMAGEDATA_T *rawImg=(IMAGEDATA_T*)(img->imageData);
  IMAGEDATA_T *rawDist=(IMAGEDATA_T*)(distImg->imageData);
  int X=img->width,Y=img->height;
  int pos,in_pos,maxInPos;
  float *nbourGrads=new float [8];
  float activeDist;
  int nbourIdx;
  bool change=false;
  float maxGrad;
  for (int y=1;y<Y-1;y++) {
    for (int x=1;x<X-1;x++) {
      pos=y*X+x;
      if (rawImg[pos]==255.f) {
	nbourIdx=0;
	maxGrad=0;
	maxInPos=-1;
	for (int in_y=-1;in_y<=1;in_y++) {
	  for (int in_x=-1;in_x<=1;in_x++) {
	    if (in_x!=0 || in_y!=0) {
	      in_pos=(in_y+y)*X+in_x+x;
	      if (in_x==0 || in_y==0) 
		activeDist=dist1;
	      else
		activeDist=dist2;
	      if (!inverse)
		nbourGrads[nbourIdx]=1.f/activeDist*(rawDist[in_pos]-rawDist[pos]);
	      else
		nbourGrads[nbourIdx]=1.f/activeDist*(rawDist[pos]-rawDist[in_pos]);
	      if (((nbourGrads[nbourIdx]>maxGrad && inverse) || (nbourGrads[nbourIdx]<maxGrad && !inverse))) {
		maxGrad=nbourGrads[nbourIdx];
		maxInPos=in_pos;
	      }
	      nbourIdx++;
	    }
	  }
	}
	if (((maxGrad>0 && inverse) || (maxGrad<0 && !inverse))  && maxInPos>=0 && rawImg[maxInPos]<255.f) {
	  rawImg[maxInPos]=255;
	  change=true;
	}
      }
    }
  }
  delete [] nbourGrads;
  return change;
}
//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
void CDistTSkel::ConnectPnts(IplImage *img, float dist1, float dist2, IplImage *distImg, bool inverse, float threshold) {
  while(GrowPnts(img,dist1,dist2,distImg,inverse,threshold)) {}
}
//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
void CDistTSkel::BinaryThin(IplImage *image)
{
  assert(image);
  bool KeepOn=true;
  int pixCnt, trans, turn=0;
  IMAGEBYTE_T nghbrs[9]; 
  IplImage *workImage=cvCloneImage(image);
  cvZero(workImage);
  ZeroBorder(image);
  int minI=1, maxI=image->width - 1, I=image->width;
  int minJ=1, maxJ=image->height - 1;//, J=image->height;
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
//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
void CDistTSkel::CreateDistSkel(void) {
  assert(m_skel);
  assert(m_distSkel);
  assert(m_distT);
  int imgSize=((m_distT->width)*(m_distT->height));
  IMAGEDATA_T *rawSkel=(IMAGEDATA_T*)(m_skel->imageData);
  IMAGEDATA_T *rawDistSkel=(IMAGEDATA_T*)(m_distSkel->imageData);
  IMAGEDATA_T *rawDistT=(IMAGEDATA_T*)(m_distT->imageData);
  for (int i=0;i<imgSize;i++) 
    if (rawSkel[i]>0.) rawDistSkel[i]=rawDistT[i];
}
//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
IplImage* CDistTSkel::BorrowDistSkel(void) {
  return m_distSkel;
}
//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
void CDistTSkel::Thin(void) {
  IplImage *tmpSkel=cvCreateImage(cvGetSize(m_skel),IPL_DEPTH_8U,1);
  cvConvertScale(m_skel,tmpSkel);
  //  BinaryThin(tmpSkel);
  CThinning thinner;
  thinner.Init(tmpSkel);
  thinner.PerformOp();
  cvConvertScale(thinner.BorrowResultImage(),m_skel);
  //  cvConvertScale(tmpSkel,m_skel);
  cvReleaseImage(&tmpSkel);
}
//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
void CDistTSkel::RemoveBoundaryPnts(double skelVal, bool min) {
  assert(m_distSkel);
  int imgSize=((m_distSkel->width)*(m_distSkel->height));
  IMAGEDATA_T *rawDistSkel=(IMAGEDATA_T*)(m_distSkel->imageData);
  IMAGEDATA_T *rawSkel=(IMAGEDATA_T*)(m_skel->imageData);
  if (min) {
    for (int i=0;i<imgSize;i++)
      if (rawDistSkel[i]<skelVal) {
	rawDistSkel[i]=0.;
	rawSkel[i]=0.;
      }
  }
  else {
    for (int i=0;i<imgSize;i++)
      if (rawDistSkel[i]>skelVal) {
	rawDistSkel[i]=0.;
	rawSkel[i]=0.;
      }
  }
  //  cout <<"Removed boundary points"<<endl;
#if(0)
  int imgSize=((m_distT->width)*(m_distT->height));
  IMAGEDATA_T *rawSkel=(IMAGEDATA_T*)(m_skel->imageData);
  IMAGEDATA_T *rawDistSkel=(IMAGEDATA_T*)(m_distSkel->imageData);
  IMAGEDATA_T *rawDistT=(IMAGEDATA_T*)(m_distT->imageData);
  for (int i=0;i<imgSize;i++) 
    if (rawSkel[i]>0.) rawDistSkel[i]=rawDistT[i];
#endif

}
//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
