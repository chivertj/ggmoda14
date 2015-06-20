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
#ifndef CTHINNING_CLASS
#define CTHINNING_CLASS

#include "../globalCVHeader.h"

#include "../ACDefinitions.h"
using namespace ACDefinitions;

class CThinning {
public:
  CThinning(void);
  virtual ~CThinning(void);
  void Init(IplImage *image);
  void PerformOp(void);
  IplImage* BorrowResultImage(void);
protected:
  IplImage *m_image;
  IplImage *m_result;
  void BinaryThin(IplImage *image);
  void GetNineNeighbours(IMAGEBYTE_T *neighbours, IMAGEBYTE_T *data, int x, int y, int Xstep);
  int CountNeighbourTransitions(IMAGEBYTE_T *neighbours);
  int CountNeighbours(IMAGEBYTE_T *neighbours);
  const int BYTEMATRIX;//=1;
  const IMAGEBYTE_T BM_SET;//=255;
  const IMAGEBYTE_T BM_NOTSET;//=0;
  void ZeroBorder(IplImage *image);
  void MakeOnePixel(IplImage *image);
};
#endif //CTHINNING_CLASS

