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
#ifndef MDMOTIONCLUSTER
#define MDMOTIONCLUSTER

#include "../globalCVHeader.h"

//ClusterPnts separates foreground from background points
//FeatPnts tracks interest based features
//SpatialCluster performs connected component analysis

#include "FeatPnts.h"
#include "mdSpatialCluster.h"

namespace md {
  class MotionCluster {
  public:
    MotionCluster(const cv::Rect &_rect) : 
      rect(_rect),
      //      m_noObjPnts(0),
      //      m_objPnts(0),
      m_spatialcluster(_rect)
      
    {
      m_featpnts.Init(rect.width,rect.height);
      //      m_objPnts=m_featpnts.BorrowObjPnts(m_noObjPnts);
    }
    void operator () (const cv::Mat &_nxtframegray) {
      nxtframegray=_nxtframegray;
      IplImage cgray=IplImage(nxtframegray);
      m_featpnts.PerformOp(&cgray);
      int noPnts=0;
      m_featpnts.BorrowAllPnts(noPnts);
      //      int *labels=
      m_spatialcluster.Calc(m_featpnts.BorrowAllPnts(noPnts),noPnts,m_featpnts.BorrowLabels());
    }
    virtual ~MotionCluster(void) {}
    const CFeatPnts& featpnts(void) { return m_featpnts; }
    const int noobjpnts(void) const { return m_featpnts.NoObjPnts(); }
    const CvPoint* objpnts(void) const { int nobjpnts; return m_featpnts.BorrowObjPnts(nobjpnts); }
    const mdSpatialCluster& clusters(void) const { return m_spatialcluster; }
    mdSpatialCluster& clusters(void)  { return m_spatialcluster; }
  protected:
    cv::Rect rect;
    CFeatPnts m_featpnts;
    //    int m_noObjPnts;
    //    CvPoint *m_objPnts;
    mdSpatialCluster m_spatialcluster;
    cv::Mat nxtframegray;
  };
}

#endif //MDMOTIONCLUSTER
