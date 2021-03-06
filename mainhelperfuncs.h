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
#ifndef MAINHELPERFUNCS
#define MAINHELPERFUNCS

#include "src/globalCVHeader.h"
#include "src/ggtracker.h"
#include "src/region.h"
#include "src/ggframe.h"
#include "src/ggregiontracker.h"

namespace mainhelpers {
  void FGOutlines(IplImage *fgmap, IplImage *colorop, const CvScalar &coloredge, bool thick=true);
  void LabelRegions(IplImage *img, const gg::frame &framedata, cv::Scalar color=cv::Scalar::all(255));
  void LabelRegion(cv::Mat &img, const gg::frameregion &freg, cv::Scalar color=cv::Scalar::all(255));
  void DrawBoxes(IplImage *img, const gg::frame &framedata, cv::Scalar color=cv::Scalar(255,0,255));
  void DrawBoxes(IplImage *img, const gg::regiontracker::OBJTRACKS_T &tracks, cv::Scalar color=cv::Scalar(0,255,100));

  void DrawBox(const cv::RotatedRect &R, const cv::Scalar color, IplImage *colorop);
  void TrimRegions(const region::REGIONS &regs, const size_t nregs, region::REGIONS &newregs, size_t &new_nregs);
  CvPoint R2Ipnt(CvPoint2D32f &r);
  //  void DrawOutlines(IplImage *img, const gg::frame &framedata)
  void Draw_bgheads(IplImage *img, const gg::frame &framedata);
  template <class T> void TrimRegions(const vector<T> &regs, const size_t nregs, vector<T> &newregs, size_t &new_nregs) {
    new_nregs=0;
    if (new_nregs>=region::MAXNOREGIONS) {
      std::vector<bool> sampled(new_nregs);
      vector<T> newnewregs(region::MAXNOREGIONSB4TRIM);
      for (size_t i=0;i<new_nregs;i++)
	sampled[i]=false;
      size_t newnew_nregs=0;
      //      std::srand(time(NULL));
      std::srand(__rdtsc());
      size_t nloops=0,maxnloops=region::MAXNOREGIONSB4TRIM;
      do {
	size_t sidx=std::rand()%new_nregs;
	if (!sampled[sidx]) {
	  sampled[sidx]=true;
	  newnewregs[newnew_nregs]=newregs[sidx];
	  newnew_nregs++;
	}
	nloops++;
      }while (newnew_nregs<(region::MAXNOREGIONS-1) && nloops<maxnloops);
      newregs=newnewregs;
      new_nregs=newnew_nregs;
    }    
  }
}

#endif //MAINHELPERFUNCS
