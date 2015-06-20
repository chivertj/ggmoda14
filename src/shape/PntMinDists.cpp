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
#include "PntMinDists.h"

CPntMinDists::CPntMinDists(void) {
}

CPntMinDists::~CPntMinDists(void) {
}

void CPntMinDists::Calc(ACDefinitions::ACPnt<int> *boundary, int nboundary, const std::vector<cv::Point> &skeleton) {
  assert(boundary && nboundary>0 && skeleton.size()>0);
  Convert(boundary,nboundary);
  Calc(m_boundary,skeleton);
}


void CPntMinDists::Calc(const std::vector<cv::Point> &boundary, const std::vector<cv::Point> &skeleton) {
  assert(boundary.size()>0 && skeleton.size()>0);
  if (m_dists.size()!=boundary.size())
    m_dists.resize(boundary.size());
  for (int b=0;b<boundary.size();b++) {
    double mindist=1e10,dist;
    int minidx=-1;
    for (int s=0;s<skeleton.size();s++) {
      dist=sqrt(pow(boundary[b].x-skeleton[s].x,2.)+pow(boundary[b].y-skeleton[s].y,2.));
      if (dist<mindist) {
	mindist=dist;
	minidx=s;
      }
    }
    m_dists[b]=mindist;
  }
}

void CPntMinDists::Convert(ACDefinitions::ACPnt<int> *boundary, int nboundary) {
  assert(boundary && nboundary>0);
  if (m_boundary.size()!=nboundary)
    m_boundary.resize(nboundary);
  for (int i=0;i<nboundary;i++) {
    m_boundary[i].x=boundary[i].x;
    m_boundary[i].y=boundary[i].y;
  }
}

void CPntMinDists::Calc(ACDefinitions::ACPnt<int> *boundary, int nboundary, const cv::Mat &skelimg) {
  Convert(skelimg);
  Calc(boundary,nboundary,m_skeleton);
}


void CPntMinDists::Convert(const cv::Mat &skelimg) {
  int nskeleton=cv::countNonZero(skelimg);
  if (m_skeleton.size()!=nskeleton)
    m_skeleton.resize(nskeleton);
  cv::Size s=skelimg.size();
  int sidx=0;
  for (int y=0;y<s.height;y++) {
    const ACDefinitions::IMAGEDATA_T *sdata=skelimg.ptr<const ACDefinitions::IMAGEDATA_T>(y);
    for (int x=0;x<s.width;x++) {
      if (sdata[x]!=0) {
	m_skeleton[sidx].x=x;
	m_skeleton[sidx++].y=y;
	if (sidx==nskeleton)
	  break;
      }
    }
    if (sidx==nskeleton)
      break;
  }
}
