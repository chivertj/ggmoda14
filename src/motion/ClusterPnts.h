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
#ifndef CCLUSTERPNTS_CLASS
#define CCLUSTERPNTS_CLASS

#include "../globalCVHeader.h"
#include "../region.h"
//#include "FeatPnts.h"
#include "ClusterPnts.h"

#include <vector>
typedef std::vector<int> LABT;

typedef std::vector<cv::Point2f> PNTS;

template <class T> T Sign(T v) {
  return v>0?1:(v<0?-1:0);
}


class CClusterPnts {
public:
  CClusterPnts(void);
  ~CClusterPnts(void);
  void PerformOp(CvPoint2D32f *currentpnts, CvPoint2D32f *previouspnts, int *labels, int nopnts);
protected:
  CvPoint2D32f *m_pnts[2];
  int *m_labels;
  int m_nopnts;

  LABT m_intlabels;

  void Select(region::IDXT &indx, int maxidx, LABT &labeled, int subset);
  cv::Point2f CalcMean(region::IDXT &indx);
  int CalcModelMean(PNTS &means, int &cnt, region::IDXT &corr);

  time_t m_timer;

  const int m_noinsubset;
  const int m_nosubsets;

  bool Equal(cv::Point2f &a, cv::Point2f &b);
  float m_xtolerance,m_ytolerance;

  PNTS m_dispnts;
  void CalcDispPnts(void);

  enum CLUSTERNAMES {NOTHING, BACKGROUND, FOREGROUND};

  cv::Point2f m_meanalldispnts;
  void CalcMeanAll(void);

  std::vector<region::IDXT> m_indxs; //(m_nosubsets);
  PNTS m_submeans; //(m_nosubsets);
  region::IDXT m_corr; //(m_nosubsets);



};

#endif //CCLUSTERPNTS_CLASS
