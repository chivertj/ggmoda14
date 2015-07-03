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
#ifndef MDSHAPECLUSTER
#define MDSHAPECLUSTER

#include "mdMotionCluster.h"
#include "mdBackProjector.h"

namespace md {
class ShapeCluster : public MotionCluster {
public:
  ShapeCluster(const cv::Rect &_rect) : MotionCluster(_rect),bproj(_rect),greyimg(_rect.size(),cv::DataType<uchar>::type) {}
  void operator()(const cv::Mat &colorimg) {
    cv::cvtColor(colorimg,greyimg,CV_BGR2GRAY);
    MotionCluster::operator()(greyimg);
    bproj.setimg(colorimg);
  }
  void backproject(int idx) {
    bproj(clusters().BorrowIRegion(idx));
  }
  const cv::Mat& getbp(void) { return bproj(); }
protected:
  BackProjector bproj;
  cv::Mat greyimg;
};
}

#endif //MDSHAPECLUSTER

