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
#include "ClusterPnts.h"
#include "MotClustNS.h"
//#include "FeatPnts.h"

#include <cstdlib>
#include <ctime>


CClusterPnts::CClusterPnts(void) 
  :
  m_labels(0), m_nopnts(0), m_noinsubset(5), m_nosubsets(5), m_xtolerance(0.3), m_ytolerance(0.3)
{
  m_pnts[0]=0; m_pnts[1]=0;
  m_intlabels=LABT(MotClustNS::MAXLKPNTCNT);
  std::fill(m_intlabels.begin(),m_intlabels.end(),NOTHING);
  m_dispnts=PNTS(MotClustNS::MAXLKPNTCNT);
  m_meanalldispnts=cv::Point2f(0.,0.);

  m_indxs=std::vector<region::IDXT>(m_nosubsets);
  for (int i=0;i<m_nosubsets;i++)
    m_indxs[i]=region::IDXT(m_noinsubset);

  m_submeans=PNTS(m_nosubsets);  

  m_corr=region::IDXT(m_nosubsets);

  //  srand(time(&m_timer));
  srand(0);
}

CClusterPnts::~CClusterPnts(void) {
}

void CClusterPnts::PerformOp(CvPoint2D32f *currentpnts, CvPoint2D32f *previouspnts, int *labels, int nopnts) {
  assert(currentpnts);
  assert(previouspnts);
  assert(labels);
  assert(nopnts>0);

  m_pnts[0]=currentpnts;
  m_pnts[1]=previouspnts;
  m_labels=labels;
  m_nopnts=nopnts;

  CalcDispPnts();
  CalcMeanAll();
  m_xtolerance=fabs(m_meanalldispnts.x)*2.;
  m_ytolerance=fabs(m_meanalldispnts.y)*2.;
  //  std::cout <<m_xtolerance<<","<<m_ytolerance<<"\t";
#if(1)
  if (m_xtolerance<0.3) m_xtolerance=0.3;
  if (m_ytolerance<0.3) m_ytolerance=0.3;
#else
  if (m_xtolerance<0.1) m_xtolerance=0.1;
  if (m_ytolerance<0.1) m_ytolerance=0.1;
#endif
  int iter=0,maxiter=10;
  int accept=0,notaccept=0;
  do {
    int cnt=0;
    std::fill(m_intlabels.begin(),m_intlabels.end(),NOTHING);
    for (int i=0;i<m_nosubsets;i++) {
      Select(m_indxs[i],m_nopnts,m_intlabels,i);
      m_submeans[i]=CalcMean(m_indxs[i]);
    }

    int modelmeanidx=CalcModelMean(m_submeans,cnt,m_corr);
    
    cv::Point2f modelmean(0.,0.);
    for (uint i=0;i<m_corr.size();i++) {
      if (m_corr[i]>0)
  modelmean+=m_submeans[i];
    }
    float invcnt=1./float(cnt);
    modelmean*=invcnt;

    //label points that do not match
    for (uint i=0;i<m_corr.size();i++) {
      if (m_corr[i]==0) {
  for (uint j=0;j<m_indxs[i].size();j++) 
    m_intlabels[m_indxs[i][j]]=FOREGROUND;
      }
    }

    //now test every untested point in the data
    for (int i=0;i<m_nopnts;i++) {
      if (Equal(modelmean,m_dispnts[i]))
  m_intlabels[i]=BACKGROUND;
      else
  m_intlabels[i]=FOREGROUND;
    }
  
    accept=0,notaccept=0;
    for (int i=0;i<m_nopnts;i++) {
      if (m_intlabels[i]==BACKGROUND) {
  accept++;
  m_labels[i]=0;
      }
      else { //foreground...
  notaccept++;
  m_labels[i]=1;
      }
    }
  } while(accept<notaccept && iter++<maxiter);
}


void CClusterPnts::Select(region::IDXT &indx, int maxidx, LABT &labeled, int subset) {
  uint i=0,j=0,maxj=1000000;
  do {
    int rpntidx=cvRound(rand()%maxidx);
    if (labeled[rpntidx]==0) {
      indx[i]=rpntidx;
      labeled[rpntidx]=BACKGROUND;
      i++;
    }
    j++;
  } while (i<indx.size() && j<maxj);
  assert(j<maxj);
}

cv::Point2f CClusterPnts::CalcMean(region::IDXT &indx) {
  cv::Point2f mean=cvPoint2D32f(0.,0.);
  for (uint i=0;i<indx.size();i++) {
    assert(indx[i]>=0 && indx[i]<m_nopnts);
    mean.x+=m_dispnts[indx[i]].x;
    mean.y+=m_dispnts[indx[i]].y;
  }
  mean.x/=indx.size();
  mean.y/=indx.size();
  return mean;
}

int CClusterPnts::CalcModelMean(PNTS &submeans, int &cnt, region::IDXT &corr) {
  assert(submeans.size()>0);
  assert(submeans.size()==corr.size());

  region::IDXm idxm=zero_matrix<int>(submeans.size(),submeans.size());
  region::IDXT rowsum(submeans.size());

  int maxrowsum=0,maxidx=-1;
  for (uint j=0;j<idxm.size1();j++) {
    idxm(j,j)=1;
    for (uint i=j+1;i<idxm.size2();i++) {
      if (Equal(submeans[i],submeans[j])) {
  idxm(j,i)=1;
  idxm(i,j)=1;
      }
    }
    for (uint i=0;i<idxm.size2();i++) {
      if (idxm(j,i)==1)
  rowsum[j]++;
    }
    if (rowsum[j]>maxrowsum) {
      maxrowsum=rowsum[j];
      maxidx=j;
    }
  }
  cnt=maxrowsum;
  for (uint i=0;i<idxm.size2();i++)
    corr[i]=idxm(maxidx,i);

  return maxidx;
}


bool CClusterPnts::Equal(cv::Point2f &a, cv::Point2f &b) {
  if (a.x>=b.x-m_xtolerance && a.x<=b.x+m_xtolerance &&
      a.y>=b.y-m_ytolerance && a.y<=b.y+m_ytolerance)
    return true;
  return false;
}

void CClusterPnts::CalcDispPnts(void) {
  assert(m_pnts[0]);
  assert(m_pnts[1]);
  for (int i=0;i<m_nopnts;i++) 
    m_dispnts[i]=cv::Point2f(m_pnts[0][i].x-m_pnts[1][i].x,
           m_pnts[0][i].y-m_pnts[1][i].y);
}

void CClusterPnts::CalcMeanAll(void) {
  m_meanalldispnts=cv::Point2f(0.,0.);
  for (int i=0;i<m_nopnts;i++) 
    m_meanalldispnts+=m_dispnts[i];
  float invnopnts=1./float(m_nopnts);
  m_meanalldispnts*=invnopnts;
}
