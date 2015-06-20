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
#ifndef WJIMGNS_NAMESPACE
#define WJIMGNS_NAMESPACE

#include "../globalCVHeader.h"
//#include "FiniteDiffExNS.h"
#include "../region.h"

namespace WJImgNS {
  void SetBorder(IplImage *img, CvScalar borderVal, int borderSize);
  void MasksToSDs(region::IREGv &masks, region::IREGv &masksds);
  bool CheckImages(const region::IREGv &imgs);
  void MaskToSD(IplImage *mask, IplImage *sd);
  void Intersect(IplImage *in_sd1, IplImage *in_sd2, IplImage *out_sd);
  bool CheckDims(IplImage *im1, IplImage *im2);
}
//~#~#~#~#~#~#~#~#~#~#~#~#~#~#~#~#~#~#~#~#~#~#~#~#~#~#~#~#~#~#~#~#~


#endif //WJIMGNS_NAMESPACE

