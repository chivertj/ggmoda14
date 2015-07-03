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
#include "mdSpatialCluster.h"
#include "../region.h"
#include "mdRasterVoronoi.h"
//#############################################
//#############################################
void md::mdSpatialCluster::Calc(const CvPoint2D32f *pnts, int nopnts, const int *pntlabels) {
  assert(pnts);
  assert(nopnts>0);
  assert(pntlabels);

  //  m_pnts=pnts;
  if (nopnts!=m_pnts.size())
    m_pnts.resize(nopnts);

  rastervoronoi.ClearReinit();

  for (size_t i=0;i<nopnts;i++) {
    m_pnts[i]=PNT_T(cvRound(pnts[i].x),cvRound(pnts[i].y),pntlabels[i]);
    rastervoronoi.AddPnt(m_pnts[i]);
  }
  rastervoronoi.Tessellate();
  m_noregs=bwlabel.CalcRegions();

  bwlabel.MakeIRegs();
  for (size_t i=0;i<m_noregs;i++) {
    cvConvert(bwlabel.BorrowIRegion(i),m_iregions[i]);
    m_regions[i].resize(bwlabel.BorrowRegions()[i].size());
    std::copy(bwlabel.BorrowRegions()[i].begin(),bwlabel.BorrowRegions()[i].end(),m_regions[i].begin());
  }
  mask=rastervoronoi.Get();
  cmask=IplImage(mask);
  CalcComponentIndexs();
  TrimSingleFeatureRegions();
}
//#############################################
void md::mdSpatialCluster::CalcComponentIndexs(void) {
  //now to calculate componentindexs -> tells us the list of feature indexs for each region (component)
  componentidxs.resize(m_noregs);
  m_regionfeatures.resize(m_noregs);
  for (size_t i=0;i<m_noregs;i++) {
    componentidxs[i].clear();
    m_regionfeatures[i].clear();
  }
  PNTS intstpnts(rastervoronoi.GetPnts().begin(),rastervoronoi.GetPnts().end());
  pPNTS pIt;
  size_t pIdx;
  for (pIt=intstpnts.begin(),pIdx=0;pIt!=intstpnts.end();++pIt,++pIdx) {
    POS_T x=(*pIt).x,y=(*pIt).y;
    for (size_t i=0;i<m_noregs;i++) {
      uchar *data=(uchar*)(m_iregions[i]->imageData);
      if (data[y*m_iregions[i]->widthStep+x]>0) {
        componentidxs[i].push_back(pIdx);
        m_regionfeatures[i].push_back(*pIt);
        break;
      }
    }
  }
}
//#############################################
void md::mdSpatialCluster::TrimSingleFeatureRegions(void) {
  bool trimmed=false;
  int notrimmed=0;
  do {
    trimmed=false;
    size_t idx;
    region::REGIT regit;
    region::IREGit iregit;
    region::REGIT featit;
    region::ComponentIndexs::iterator compit;
    for ( idx=0,regit=m_regions.begin(),iregit=m_iregions.begin(),featit=m_regionfeatures.begin(),compit=componentidxs.begin();
        idx<m_regionfeatures.size();
        ++idx,++regit,++iregit,++featit,++compit) {
      if (m_regionfeatures[idx].size()<=1) {
        m_regions.erase(regit);
        m_iregions.erase(iregit);
        m_regionfeatures.erase(featit);
        componentidxs.erase(compit);
        notrimmed++;
        trimmed=true;
        break;
      }
    }
  } while(trimmed && m_noregs>=0);
  if (notrimmed>0) {
    m_noregs-=notrimmed;
    m_regions.resize(region::MAXNOREGIONSB4TRIM);
    m_iregions.resize(region::MAXNOREGIONSB4TRIM);
    for (size_t i=region::MAXNOREGIONSB4TRIM-notrimmed;i<region::MAXNOREGIONSB4TRIM;i++)
      m_iregions[i]=cvCreateImage(cvSize(m_X,m_Y),IPL_DEPTH_8U,1);
  }

  mask=cv::Scalar_<LAB_T>::all(0);
  for (size_t i=0;i<m_noregs;i++) {
    for (size_t j=0;j<m_regions[i].size();j++)
      mask.at<LAB_T>(m_regions[i][j].y,m_regions[i][j].x)=255;
  }
}
//#############################################
void md::mdSpatialCluster::UpdateLabels(const cv::Mat &shapesimage) {
  int update=0;
  for (int i=0;i<m_pnts.size();i++) {
    if (shapesimage.at<uchar>(m_pnts[i])>0 && m_pnts[i].label==0) {
      m_pnts[i].label=1;
      update++;
    }
  }
}
//#############################################
