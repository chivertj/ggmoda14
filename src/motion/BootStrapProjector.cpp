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
#include "BootStrapProjector.h"
#include <iostream>
using namespace std;
#include "../globalCVHeader.h"
#include "../ACDefinitions.h"
using namespace ACDefinitions;
//#define USEJOINTHISTO
//#define ALTBP
//#define BACKPROJECT_BIGGESTREGION
#include "MotClustNS.h"
using namespace MotClustNS;

//#include "CvMDLMixtModel.h"
//typedef CvMDLMixtModel HISTOMODEL_T;
#include "ggmatlearn.h"
typedef gg::ggmatlearn HISTOMODEL_T;

//==================================
CBootStrapProjector::CBootStrapProjector(void) 
  : m_img(0),
    m_backprojection(0),
    m_colImageFl(0),
    m_mixtModelRes(0),
    m_halomFl(0), m_fgmFl(0), 
    m_halom(0), m_fgm(0), m_combm(0),
    m_tmpbwimg(0)
{
}
//==================================
CBootStrapProjector::~CBootStrapProjector(void) {
  cvReleaseImage(&m_backprojection);
  cvReleaseImage(&m_colImageFl);
  cvReleaseImage(&m_mixtModelRes);
  cvReleaseImage(&m_halomFl);
  cvReleaseImage(&m_fgmFl);
  cvReleaseImage(&m_tmpbwimg);
}
//==================================
void CBootStrapProjector::Init(IplImage *img) {
  assert(img->nChannels==3);
  m_img=img;
  if (!m_colImageFl) {
    m_colImageFl=cvCreateImage(cvGetSize(m_img),IPL_DEPTH_32F,3);
    m_mixtModelRes=cvCreateImage(cvGetSize(m_img),IPL_DEPTH_32F,1);
    m_halomFl=cvCreateImage(cvGetSize(m_img),IPL_DEPTH_32F,1);
    m_fgmFl=cvCreateImage(cvGetSize(m_img),IPL_DEPTH_32F,1);
    m_combm=0;
  }
  cvConvertScale(m_img,m_colImageFl);

  if (!m_backprojection)
    m_backprojection=cvCreateImage(cvGetSize(m_img),IPL_DEPTH_8U,1);
  if (!m_tmpbwimg) {
    m_tmpbwimg=cvCreateImage(cvGetSize(m_img),IPL_DEPTH_8U,1);
    bwlabel.Init(m_tmpbwimg);
  }
}
//==================================
void CBootStrapProjector::PerformOp(IplImage *halom, IplImage *fgm, IplImage *combm) {
  assert(halom);
  assert(fgm);
  assert(combm);
  cvConvertScale(halom,m_halomFl);
  cvConvertScale(fgm,m_fgmFl);
  m_halom=halom;
  m_fgm=fgm;
  m_combm=combm;

  std::cout <<"CBootStrapProjector::BackProject"<<std::endl;
  BackProject();
#ifdef BACKPROJECT_BIGGESTREGION
  std::cout <<"CBootStrapProjector::BiggestRegion"<<std::endl;
  BiggestRegion();
#else
  cvThreshold(m_backprojection,m_backprojection,200,255,CV_THRESH_BINARY);
#endif
  std::cout <<"Finishing CBootStrapProjector::PerformOp"<<std::endl;
}
//==================================
IplImage* CBootStrapProjector::BorrowResult(void) {
  return m_backprojection;
}
//==================================
void CBootStrapProjector::BackProject(void) {
  int X=m_colImageFl->width, Y=m_colImageFl->height;
  int size=X*Y;
  HISTOMODEL_T haloMixtModel;
  std::cout <<"in BackProject..."<<std::endl;

  IplImage **splitImgs=new IplImage *[3];
  for (int i=0;i<3;i++)
    splitImgs[i]=cvCreateImage(cvGetSize(m_colImageFl),IPL_DEPTH_32F,1);
  cvSplit(m_colImageFl,splitImgs[0],splitImgs[1],splitImgs[2],0);
  cvZero(m_backprojection);
  //  haloMixtModel.Init(splitImgs,3,m_halomFl,true);
  std::cout <<"\t haloMixtModel learning"<<std::endl;
  haloMixtModel.learn(splitImgs,3,m_halomFl);
  HISTOMODEL_T combMixtModel;
  //  combMixtModel.Init(splitImgs,3,m_fgmFl,true);
  std::cout <<"\t combMixtModel learning"<<std::endl;
  combMixtModel.learn(splitImgs,3,m_fgmFl);
  std::cout <<"\t inferring"<<std::endl;
  if (combMixtModel.GetNoMaskPnts()>30 && haloMixtModel.GetNoMaskPnts()>30) {
    
    float *rawImg=(float*)(m_colImageFl->imageData);
    IMAGEBYTE_T *rawCombMask=(IMAGEBYTE_T*)(m_combm->imageData);
    float *rawRes=(float*)(m_mixtModelRes->imageData);
    cvZero(m_mixtModelRes);
    
    //initial classify
        float fgPrior=0.5;
    //    float fgPrior=0.05;
    float bgPrior=1.-fgPrior;
    float *rawI_BN=new float [size];
    memset(rawI_BN,0,size*sizeof(float));
    float *rawI=new float [size];
    memset(rawI,0,size*sizeof(float));
    float combMaskCnt=0;
    for (int i=0;i<size;i++) {
      if (rawCombMask[i]>0) {
	combMaskCnt++;
	rawI_BN[i]=exp(haloMixtModel.GetLogProb(&rawImg[i*3]));
	rawI[i]=exp(combMixtModel.GetLogProb(&rawImg[i*3]));
	rawRes[i]=1.-rawI_BN[i]*bgPrior/(rawI[i]+1e-100);
	if (rawRes[i]<0.)
	  rawRes[i]=0.;
	else if (rawRes[i]>1.)
	  rawRes[i]=1.;
      }
    }
    std::cout <<"\t ex inferring..."<<std::endl;
#if(0)
    //optimize for fg prior
    int noIters=50;
    float ttlFG;
    for (int j=0;j<noIters;j++) {
      ttlFG=0.;
      for (int i=0;i<size;i++)
	ttlFG+=rawRes[i];
      fgPrior=ttlFG/combMaskCnt;
      //    cout <<fgPrior<<" "<<flush;
      bgPrior=1.-fgPrior;
      cvZero(m_mixtModelRes);
      for (int i=0;i<size;i++) {
	if (rawCombMask[i]>0) {
	  rawRes[i]=1.-rawI_BN[i]*bgPrior/(rawI[i]+1e-100);
	  if (rawRes[i]<0.)
	    rawRes[i]=0.;
	  else if (rawRes[i]>1.)
	    rawRes[i]=1.;
	}
      }
    }
#endif
    cvConvertScale(m_mixtModelRes,m_backprojection,255);
    delete [] rawI_BN;
    delete [] rawI;
  std::cout <<"ex BackProject."<<std::endl;
  }
  for (int i=0;i<3;i++)
    cvReleaseImage(&splitImgs[i]);
  delete [] splitImgs;
  cvAnd(m_backprojection,m_fgm,m_backprojection);
}
//==================================
void CBootStrapProjector::BiggestRegion(void) {
  cvThreshold(m_backprojection,m_tmpbwimg,200,255,CV_THRESH_BINARY);
  //  cvDilate(m_tmpbwimg,m_tmpbwimg,NULL,2);
  int nregs=bwlabel.CalcRegions();
  bwlabel.MakeIRegs();
  if (nregs>0) {
    int maxnononzero=0,maxidx=-1;
    for (int i=0;i<nregs;i++) {
      int nononzero=bwlabel.BorrowRegions()[i].size();
      //      std::cout <<i<<":"<<nononzero<<" ";
      if (nononzero>maxnononzero) {
	maxnononzero=nononzero;
	maxidx=i;
      }
    }
    //    std::cout <<std::endl<<"MAX:"<<maxidx<<std::endl;
    if (maxidx!=-1)  {
      cvCopy(bwlabel.BorrowIRegion(maxidx),m_tmpbwimg);
      //      cvDilate(m_tmpbwimg,m_tmpbwimg);
      cvAnd(m_tmpbwimg,m_backprojection,m_backprojection);
    }
  }
}
//==================================
