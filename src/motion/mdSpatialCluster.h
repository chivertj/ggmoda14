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
#ifndef MDSPATIALCLUSTER_CLASS
#define MDSPATIALCLUSTER_CLASS

#include "../globalCVHeader.h"

#include "../region.h"

#include "../bwlabel.h"
#include "mdRasterVoronoi.h"

namespace md {

class mdSpatialCluster {
public:
  mdSpatialCluster(const cv::Rect &_rect) :
    m_X(_rect.width),m_Y(_rect.height),
    m_pnts(0),
    m_regions(region::MAXNOREGIONSB4TRIM),
    m_iregions(region::MAXNOREGIONSB4TRIM),
    rastervoronoi(_rect),
    m_noregs(0),
    tmpcntr(0)
  {
    mask=cv::Mat::zeros(_rect.size(),cv::DataType<uchar>::type);
    cmask=IplImage(mask);
    for (size_t i=0;i<region::MAXNOREGIONSB4TRIM;i++)
      m_iregions[i]=cvCreateImage(cvSize(m_X,m_Y),IPL_DEPTH_8U,1);
    rastervoronoicimg=IplImage(rastervoronoi.Get());
    bwlabel.Init(&rastervoronoicimg);
  }
  virtual ~mdSpatialCluster(void) {
    for (size_t i=0;i<region::MAXNOREGIONSB4TRIM;i++)
      cvReleaseImage(&m_iregions[i]);
  }
  void Calc(const CvPoint2D32f *pnts, int nopnts, const int *pntlabels);
  const IplImage* BorrowMask(void) const { 
    return &cmask; 
  }
  const region::IREG BorrowIRegion(int idx) const { assert(idx>=0 && idx<region::MAXNOREGIONSB4TRIM); return m_iregions[idx]; }
  const region::REGIONS& BorrowRegions(void) const { return m_regions; }
  const region::ComponentIndexs& BorrowComponentIndexs(void) const { return componentidxs; }
  const region::REGIONS& BorrowRegionFeatures(void) const { return m_regionfeatures; }
  void UpdateLabels(const cv::Mat &shapesimage);
  const int NoRegions(void) const { return m_noregs; }
protected:
  int m_X,m_Y;
  std::vector<PNT_T> m_pnts; 
  cv::Mat mask;
  IplImage cmask;

  region::REGIONS m_regions;
  region::IREGv m_iregions;

  CBWLabel bwlabel;
  mdRasterVoronoi rastervoronoi;
  IplImage rastervoronoicimg;

  int m_noregs;
  region::ComponentIndexs componentidxs;
  region::REGIONS m_regionfeatures;

  int tmpcntr;

  void CalcComponentIndexs(void);
  void TrimSingleFeatureRegions(void);

  //mdTrimmer regiontrimmer;
};

}
#endif //CSPATIALCLUSTER_CLASS

