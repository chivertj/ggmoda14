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
}

#endif //MAINHELPERFUNCS
