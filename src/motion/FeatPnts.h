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
#ifndef CFEATPNTS_CLASS
#define CFEATPNTS_CLASS

#include "../globalCVHeader.h"
#include "MotClustNS.h"

#include "mkutilsopencv.hpp"
#include "ClusterPnts.h"

#include "../region.h"

class CFeatPnts {
public:
  CFeatPnts(void);
  ~CFeatPnts(void);

  void Init(int width, int height);

  void PerformOp(IplImage *greyimg);
  const CvPoint* BorrowObjPnts(int &noobjpnts) const;
  const int NoObjPnts(void) const;
  const CvPoint2D32f* BorrowAllPnts(int &nopnts) const;
  const int* BorrowLabels(void) const;
  const CvPoint2D32f* BorrowPrevPnts(int &nopnts) const;
  enum IDXMAPT {FORWARD,BACKWARD};
  const region::IDXT& IdxMap(IDXMAPT mapt) const ;
protected:
  void MakeObjPnts(int pntidx=1);

  void InitPnts(void);
  void Measure(void);
  void Predict(void);
  void Correct(void);
  void ClusterPnts(void);
  void ReCountPnts(void);

  void CheckBounds(void);
  void CheckProximity(void);
  void ResetStatus(char *status, const int maxpnts, const int nopnts);

  int *m_pntLabels;
  const float PNTDISTTHRESH;

  int m_X,m_Y;
  bool m_needToInitPnts;

  static const int MAX_PNTCOUNT=MotClustNS::MAXLKPNTCNT;
  int m_noPnts;
  CvPoint2D32f *m_pnts[2];
  char *m_pntStatus;
  float *m_featerrs;
  int m_lkPyrFlags;
  CvSize m_trackWinSize;
  CvPoint *m_objPnts;
  int m_noObjPnts;
  IplImage *m_grey,*m_prev_grey,*m_pyramid,*m_prev_pyramid;
  IplImage *m_tmpSwapImg, *m_tmppyramid;
  CvPoint2D32f *m_tmpSwapPnts;

  void Clear(void);

  IplImage *m_gftteig;
  IplImage *m_gftttemp;
  const double m_gfttquality;// = 0.01;
  const double m_gfttmindistance;// = 10;

  void GetNewFeatures(CvPoint2D32f *pnts, int &nopnts, int useharris);
  IplImage *m_featmask;

  float *m_pntdists;

  CvKalman *m_kalman[MAX_PNTCOUNT];
  const float m_variance;
  void InitKalman(void);
  void InitKalman(int idx, CvMat *initpnt);

  CClusterPnts m_clusterer;

  region::IDXT m_forwardidxs;
  region::IDXT m_backwardidxs;
};

//---------------------------------------
inline const int CFeatPnts::NoObjPnts(void) const {
  return m_noObjPnts;
}
//---------------------------------------
inline void CFeatPnts::InitKalman(int idx, CvMat *initpnt) {
  assert(idx>=0);
  assert(idx<MAX_PNTCOUNT);
  //transition matrix
  // 1 0 d 0 0 0  x+dx/dt
  // 0 1 0 d 0 0  y+dy/dt
  // 0 0 1 0 d 0  dx/dt+d2x/d2y
  // 0 0 0 1 0 d 
  // 0 0 0 0 1 0
  // 0 0 0 0 0 1
  cvSetIdentity( m_kalman[idx]->transition_matrix, cvRealScalar(1.) );
  //velocity
  cvmSet(m_kalman[idx]->transition_matrix,0,2,0.01);
  cvmSet(m_kalman[idx]->transition_matrix,1,3,0.01);
  //acceleration
  cvmSet(m_kalman[idx]->transition_matrix,2,4,0.01);
  cvmSet(m_kalman[idx]->transition_matrix,3,5,0.01);
  cvSetIdentity( m_kalman[idx]->measurement_matrix, cvRealScalar(1.) );
  //  mk::utils::opencv::PrintMat<float>(m_kalman[idx]->measurement_matrix);
  //cvSetIdentity( m_kalman[idx]->process_noise_cov, cvRealScalar(m_variance*2.) ); //how believable is the model?
  cvSetIdentity( m_kalman[idx]->process_noise_cov, cvRealScalar(m_variance) ); //how believable is the model?
  //cvSetIdentity( m_kalman[idx]->measurement_noise_cov, cvRealScalar(m_variance) ); //how believable are the measurements? 5.
  cvSetIdentity( m_kalman[idx]->measurement_noise_cov, cvRealScalar(m_variance*5.) ); //how believable are the measurements? 5.
  cvSetIdentity( m_kalman[idx]->error_cov_post, cvRealScalar(m_variance) );
  cvSetIdentity( m_kalman[idx]->error_cov_pre, cvRealScalar(m_variance) );
  cvZero(m_kalman[idx]->state_pre);
  cvZero(m_kalman[idx]->state_post);
  m_kalman[idx]->state_pre->data.fl[0]=m_kalman[idx]->state_post->data.fl[0]=initpnt->data.fl[0];
  m_kalman[idx]->state_pre->data.fl[1]=m_kalman[idx]->state_post->data.fl[1]=initpnt->data.fl[1];
}
//oooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooo
#endif //CFEATPNTS_CLASS
