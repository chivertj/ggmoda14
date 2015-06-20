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
/*
 * mdTrimmer.h
 *
 *  Created on: Dec 28, 2011
 *      Author: jpc
 */

#ifndef MDTRIMMER_H_
#define MDTRIMMER_H_

#include "../globalCVHeader.h"
#include "../region.h"

namespace md {
  /**
    Given a set of feature points
    and a set (or vector) of regions
    identify how many feature points fall within each region
  */
  class mdTrimmer {
  public:
    ///Given vector of region points and vector of feature points
    ///create new image called regionidximg
    ///loop through region points, setting each pixel
    ///to the index of the region in the vector of region points
    ///create new image called featurecountimg=0
    ///loop through feature points and increment all pixels in featurecountimg
    ///that have the same index in regionidximg
    ///create a new vector of region points who
    ///have values in featurecountimg greater than 1.
    mdTrimmer(const size_t _X, const size_t _Y) : X(_X),Y(_Y),regionidximg(_Y,_X),featurecntimg(Y,X) {}
    const region::regionscontainer& operator () (const CFeatPnts &featpnts, const region::regionscontainer &regions) {
      regionidximg=uchar(regions.nregs);
      featurecntimg=uchar(0);
      for (size_t i=0;i<regions.nregs;i++) {
        for (size_t j=0;j<regions.regs[i].size();j++)
          regionidximg(regions.regs[i][j].y,regions.regs[i][j].x)=i;
      }
      int noobjpnts;
      const CvPoint *features=featpnts.BorrowObjPnts(noobjpnts);
      for (int i=0;i<noobjpnts;i++) {
        size_t regionidx=regionidximg(features[i].y,features[i].x);
        assert(regionidx<regions.nregs);
        for (size_t j=0;j<regions.regs[regionidx].size();j++)
          featurecntimg(regions.regs[regionidx][j].y,regions.regs[regionidx][j].x)++;
      }
      trimmedregs.nregs=0;
      removedidxs.clear();
      for (size_t i=0;i<regions.nregs;i++) {
        if (featurecntimg(regions.regs[i][0].y,regions.regs[i][0].x)>1)
          trimmedregs.regs[trimmedregs.nregs++]=regions.regs[i];
        else
          removedidxs.push_back(i);
      }
      return trimmedregs;
    }
    const region::regionscontainer& get(void) const { return trimmedregs; }
    const region::IDXT& getremovedidxs(void) const { return removedidxs; }
  protected:
    const size_t X, Y;
    cv::Mat_<uchar> regionidximg,featurecntimg;
    region::regionscontainer trimmedregs;
    region::IDXT removedidxs;
    mdTrimmer(void);
    void operator= (const mdTrimmer &);
  };
}

#endif /* MDTRIMMER_H_ */
