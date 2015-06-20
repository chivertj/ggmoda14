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
#ifndef BWLABEL_CLASS
#define BWLABEL_CLASS

#include "globalCVHeader.h"
#include <vector>

#include "region.h"
using namespace region;

class CBWLabel {
public:
  CBWLabel(void);
  ~CBWLabel(void);
  void Init(IplImage *ipimg);
  int CalcRegions(void);
  const REGIONS& BorrowRegions(void) const {return regions;}
  IplImage* BorrowLabelImg(void);
  IplImage* BorrowCLabelImg(void);
  void MakeLabelImg(void);
  void MakeCLabelImg(void);
  IREGv BorrowIRegions(void);
  IREG BorrowIRegion(int idx);
  void MakeIRegs(void);
protected:
  CBWLabel(CBWLabel &bw); //: L(0),I(0),width(-1),height(-1) {}
  IplImage *I;
  IplImage *IUS;
  int width;
  int height;
  static const int maxnoregions=MAXNOREGIONSB4TRIM;
  //vector<CvPoint> regions[maxnoregions];
  REGIONS regions;
  IplImage *labelimg;
  IplImage *labelimgUS;
  int label_count;
  int noregions;
  IplImage *labelimgC;

  IREGv iregs;
};


#endif //BWLABEL_CLASS

