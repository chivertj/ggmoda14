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
#include "FeatPnts.h"
#include <iostream>

const int PNTRADIUS=MotClustNS::LKPNTDENSITY;
const uchar FEATCLR=255;
const CvScalar CVFEATCLR=cvScalarAll(FEATCLR);

template <class T> void SetZero(T* data, const int N) {
  memset((void*)data,0,N*sizeof(T));
}

void cvPntToMat(CvPoint2D32f *pnt, CvMat *mat) {
  memcpy(mat->data.fl,pnt,2*sizeof(float));
}

void cvMatToPnt(CvMat *mat, CvPoint2D32f *pnt) {
  memcpy(pnt,mat->data.fl,2*sizeof(float));
}

//oooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooo
CFeatPnts::CFeatPnts(void) 
  :        m_pntLabels(0),
     PNTDISTTHRESH(0.3),  //1.
     m_X(0),m_Y(0),  
     m_needToInitPnts(true),
     m_noPnts(0),
     m_pntStatus(0),
     m_featerrs(0),
     m_objPnts(0),
     m_noObjPnts(0),
     m_grey(0),m_prev_grey(0),m_pyramid(0),m_prev_pyramid(0),
     m_tmpSwapImg(0),
     m_tmppyramid(0),
     m_tmpSwapPnts(0),
     m_gftteig(0),
     m_gftttemp(0),
     m_gfttquality(0.01),
     m_gfttmindistance(MotClustNS::HFLKPNTDENSITY),
     m_featmask(0),
     m_pntdists(0),
     m_variance(30.),
     m_forwardidxs(MAX_PNTCOUNT),
     m_backwardidxs(MAX_PNTCOUNT)
{
  m_pnts[0]=0;
  m_pnts[1]=0;
  m_trackWinSize=cvSize(13,13);

  for (int i=0;i<MAX_PNTCOUNT;i++)
    m_kalman[i]=cvCreateKalman(6,2); //statedims, measdims

  m_pnts[0]=new CvPoint2D32f[MAX_PNTCOUNT];
  m_pnts[1]=new CvPoint2D32f[MAX_PNTCOUNT];
  m_objPnts=new CvPoint[MAX_PNTCOUNT];
  m_pntStatus=new char[MAX_PNTCOUNT];
  m_featerrs=new float[MAX_PNTCOUNT];
  m_pntLabels=new int[MAX_PNTCOUNT];
  m_pntdists=new float [MAX_PNTCOUNT];
}
//oooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooo
CFeatPnts::~CFeatPnts(void) {
  Clear();

  delete [] m_pnts[0];
  delete [] m_pnts[1];
  delete [] m_pntStatus;
  delete [] m_featerrs;
  delete [] m_pntLabels;
  delete [] m_objPnts;
  delete [] m_pntdists;

  for (int i=0;i<MAX_PNTCOUNT;i++)
    cvReleaseKalman(&m_kalman[i]);
}
//oooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooo
void CFeatPnts::Init(int width, int height) {
  assert(width>0);
  assert(height>0);
  Clear();
  m_X=width;  m_Y=height;
  
  m_grey=cvCreateImage(cvSize(width,height),IPL_DEPTH_8U,1);
  m_prev_grey = cvCreateImage( cvSize(width,height), IPL_DEPTH_8U, 1 );
  m_pyramid = cvCreateImage( cvSize(width+8,height/3.), IPL_DEPTH_32F, 1 );
  m_prev_pyramid = cvCreateImage( cvSize(width+8,height/3.), IPL_DEPTH_32F, 1 );
  m_lkPyrFlags=0;
  m_needToInitPnts=true;

  m_gftteig = cvCreateImage( cvGetSize(m_grey), 32, 1 );
  m_gftttemp = cvCreateImage( cvGetSize(m_grey), 32, 1 );

  m_featmask=cvCreateImage(cvGetSize(m_grey), IPL_DEPTH_8U,1);

  SetZero<>(m_pnts[0],MAX_PNTCOUNT);
  SetZero<>(m_pnts[1],MAX_PNTCOUNT);
  SetZero<>(m_pntStatus,MAX_PNTCOUNT);
  SetZero<>(m_featerrs,MAX_PNTCOUNT);
  SetZero<>(m_pntLabels,MAX_PNTCOUNT);
  SetZero<>(m_objPnts,MAX_PNTCOUNT);
  SetZero<>(m_pntdists,MAX_PNTCOUNT);

  InitKalman();
}
//oooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooo
void CFeatPnts::InitKalman(void) {
  CvMat *zpnt=cvCreateMat(2,1,CV_32FC1);
  cvZero(zpnt);
  for (int i=0;i<MAX_PNTCOUNT;i++) 
    InitKalman(i,zpnt);
  cvReleaseMat(&zpnt);
}
//oooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooo
void CFeatPnts::Clear(void) {
  cvReleaseImage(&m_grey);
  cvReleaseImage(&m_prev_grey);
  cvReleaseImage(&m_pyramid);
  cvReleaseImage(&m_prev_pyramid);
  cvReleaseImage(&m_gftteig);
  cvReleaseImage(&m_gftttemp);
  cvReleaseImage(&m_featmask);
}
//oooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooo
void CFeatPnts::PerformOp(IplImage *greyimg) {
  cvCopy(greyimg,m_grey);

  if (m_needToInitPnts) 
    InitPnts();
  else if (m_noPnts>0) {
    //Predict();
    Measure();
//    Correct();

    CheckBounds();
    CheckProximity();

    ReCountPnts();
    ResetStatus(m_pntStatus,MAX_PNTCOUNT,m_noPnts);

    ClusterPnts();

    if (m_noPnts<MAX_PNTCOUNT-5) 
      GetNewFeatures(m_pnts[1],m_noPnts,0);

    CV_SWAP( m_pnts[0], m_pnts[1], m_tmpSwapPnts );
  }
  CV_SWAP( m_prev_grey, m_grey, m_tmpSwapImg  );
  CV_SWAP( m_pyramid, m_prev_pyramid,m_tmppyramid);
}
//oooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooo
void CFeatPnts::InitPnts(void) {
  cvZero(m_gftteig);
  cvZero(m_gftttemp);
  m_noPnts = MAX_PNTCOUNT;
  cvGoodFeaturesToTrack(m_grey,m_gftteig,m_gftttemp,m_pnts[0],&m_noPnts,
        m_gfttquality,m_gfttmindistance,NULL);
  cvFindCornerSubPix( m_grey, m_pnts[0], m_noPnts,
          m_trackWinSize, cvSize(-1,-1),
          cvTermCriteria(CV_TERMCRIT_ITER|CV_TERMCRIT_EPS,20,0.03));
  CvMat *pnt=cvCreateMat(2,1,CV_32FC1);
  for (int i=0;i<m_noPnts;i++) {
    cvPntToMat(&m_pnts[0][i],pnt);
    InitKalman(i,pnt);
  }
  cvReleaseMat(&pnt);
  SetZero<>(m_pntLabels,MAX_PNTCOUNT);
  m_needToInitPnts=false;
}
//oooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooo
void CFeatPnts::Measure(void) {
  memset((void*)m_pnts[1],0,MAX_PNTCOUNT*sizeof(CvPoint2D32f));
  cvCalcOpticalFlowPyrLK( m_prev_grey, m_grey, m_prev_pyramid, m_pyramid,
        m_pnts[0], m_pnts[1], m_noPnts, m_trackWinSize, 3, 
        m_pntStatus, m_featerrs,
        cvTermCriteria(CV_TERMCRIT_ITER|CV_TERMCRIT_EPS,20,0.03), 
        m_lkPyrFlags );
}
//oooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooo
void CFeatPnts::ReCountPnts(void) {
  int i,k;
  CvPoint2D32f tmppnt;
  float tmperr;
  CvKalman *tmpkalman;
  for(i = 0,k = 0; i < m_noPnts; i++ ) {
    if ( !m_pntStatus[i] ) {
      m_forwardidxs[i]=-1;
      continue;
    }
    if (i!=k) {
      CV_SWAP(m_pnts[0][k],m_pnts[0][i],tmppnt);
      CV_SWAP(m_pnts[1][k],m_pnts[1][i],tmppnt);
      CV_SWAP(m_featerrs[k],m_featerrs[i],tmperr);
      CV_SWAP(m_kalman[k],m_kalman[i],tmpkalman);
      m_backwardidxs[k]=i;
      m_forwardidxs[i]=k;
    }
    k++;
  }
  for (i=m_noPnts;i<MAX_PNTCOUNT;i++)
    m_forwardidxs[i]=-1;
  m_noPnts=k;
  for (i=m_noPnts;i<MAX_PNTCOUNT;i++)
    m_backwardidxs[i]=-1;
}
//oooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooo
void CFeatPnts::ClusterPnts(void) {
  memset((void*)m_pntLabels,0,MAX_PNTCOUNT*sizeof(int));
  memset((void*)m_objPnts,0,MAX_PNTCOUNT*sizeof(CvPoint));
#if(0)
  for(int i = 0; i < m_noPnts; i++ ) {
    m_pntdists[i]=sqrt(pow(m_pnts[1][i].x-m_pnts[0][i].x,2.)+pow(m_pnts[1][i].y-m_pnts[0][i].y,2.));
    if (m_pntdists[i]<PNTDISTTHRESH)
      m_pntLabels[i]=0;
    else if (m_pntdists[i]>=PNTDISTTHRESH) 
      m_pntLabels[i]=1;
  }
#else
  m_clusterer.PerformOp(m_pnts[1],m_pnts[0],m_pntLabels,m_noPnts);
#endif
  MakeObjPnts();
}
//oooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooo
const CvPoint* CFeatPnts::BorrowObjPnts(int &noobjpnts) const {
  noobjpnts=m_noObjPnts;
  return m_objPnts;
}
//oooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooo
const CvPoint2D32f* CFeatPnts::BorrowAllPnts(int &nopnts) const {
  nopnts=m_noPnts;
  return m_pnts[0];
}
//oooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooo
const CvPoint2D32f* CFeatPnts::BorrowPrevPnts(int &nopnts) const {
  nopnts=m_noPnts;
  return m_pnts[1];
}
//oooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooo
const int* CFeatPnts::BorrowLabels(void) const {
  return m_pntLabels;
}
//oooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooo
void CFeatPnts::GetNewFeatures(CvPoint2D32f *pnts, int &nopnts, int useharris) {
  cvZero(m_featmask);
  CvPoint pnt;
  for (int i=0;i<nopnts;i++) {
    pnt.x=cvRound(pnts[i].x);
    pnt.y=cvRound(pnts[i].y);
    cvCircle(m_featmask,pnt,PNTRADIUS,CVFEATCLR,-1);
  }
  cvNot(m_featmask,m_featmask);
  int extra=MAX_PNTCOUNT-nopnts;
  cvGoodFeaturesToTrack( m_grey, m_gftteig, m_gftttemp, pnts+nopnts, &extra,
       m_gfttquality, m_gfttmindistance, m_featmask, 3, useharris, 0.04 );
  CvMat *mpnt=cvCreateMat(2,1,CV_32FC1);
  for (int i=nopnts;i<nopnts+extra;i++) {
    m_pntStatus[i]=1;
    cvPntToMat(&pnts[i],mpnt);
    InitKalman(i,mpnt);  
  }
  cvReleaseMat(&mpnt);

  nopnts+=extra;
}
//oooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooo
void CFeatPnts::Predict(void) {
  for (int i=0;i<m_noPnts;i++)
    cvKalmanPredict(m_kalman[i],0);
}
//oooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooo
void CFeatPnts::Correct(void) {
  CvMat *pnt=cvCreateMat(2,1,CV_32FC1);
  for (int i=0;i<m_noPnts;i++) {
    cvPntToMat(&m_pnts[1][i],pnt);
    cvKalmanCorrect(m_kalman[i],pnt);
    cvMatToPnt(m_kalman[i]->state_post,&m_pnts[1][i]);
  }
  cvReleaseMat(&pnt);
}
//oooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooo
void CFeatPnts::CheckBounds(void) {
  for(int i = 0; i < m_noPnts; i++ ) {
    if ( (m_pnts[1][i].x) > (m_X-1.) || (m_pnts[1][i].x) < 0. || (m_pnts[1][i].y) > (m_Y-1.) || (m_pnts[1][i].y) < 0.)
      m_pntStatus[i]=0;
  }
}
//oooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooo
void CFeatPnts::CheckProximity(void) {
  // identify points too close
  cvZero(m_featmask);
  uchar *maskdata=(uchar*)(m_featmask->imageData);
  int widthStep=m_featmask->widthStep;
  CvPoint pnt;
  for (int i=0;i<m_noPnts;i++) {
    pnt.x=cvRound(m_pnts[1][i].x);
    pnt.y=cvRound(m_pnts[1][i].y);
    if (maskdata[pnt.y*widthStep+pnt.x]==FEATCLR) 
      m_pntStatus[i]=0;
    else
      cvCircle(m_featmask,pnt,MotClustNS::HFLKPNTDENSITY,CVFEATCLR,-1);
  }
}
//oooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooo
void CFeatPnts::ResetStatus(char *status, const int maxpnts, const int nopnts) {
  memset((void*)status,0,maxpnts*sizeof(char));
  for (int i=0;i<nopnts;i++)
    status[i]=1;
}
//oooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooo
void CFeatPnts::MakeObjPnts(int pntidx) {
  assert(pntidx==0 || pntidx==1);
  m_noObjPnts=0;
  for(int i = 0; i < m_noPnts; i++ ) {
    if (m_pntLabels[i]>0) {
      m_objPnts[m_noObjPnts].x=cvRound(m_pnts[pntidx][i].x);
      m_objPnts[m_noObjPnts].y=cvRound(m_pnts[pntidx][i].y);
      m_noObjPnts++;
    }
  }
}
//oooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooo
const region::IDXT& CFeatPnts::IdxMap(IDXMAPT mapt) const {
  if (mapt==FORWARD)
    return m_forwardidxs;
  return m_backwardidxs;
}
//oooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooo

