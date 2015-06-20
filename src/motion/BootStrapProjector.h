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
#ifndef CBOOTSTRAPPROJECTOR_CLASS
#define CBOOTSTRAPPROJECTOR_CLASS

#include "../globalCVHeader.h"
#include "../ACDefinitions.h"
using namespace ACDefinitions;

#include "../bwlabel.h"

class CBootStrapProjector {
public:
  CBootStrapProjector(void);
  virtual ~CBootStrapProjector(void);
  void Init(IplImage *img);
  void PerformOp(IplImage *halom, IplImage *fgm, IplImage *combm);
  IplImage* BorrowResult(void);
protected:
  IplImage *m_img;
  IplImage *m_backprojection;

  void BackProject(void);

  IplImage *m_colImageFl;
  IplImage *m_mixtModelRes;
  IplImage *m_halomFl, *m_fgmFl;
  IplImage *m_halom,*m_fgm,*m_combm;

  CBWLabel bwlabel;
  IplImage *m_tmpbwimg;
  void BiggestRegion(void);
};

#endif //CBOOTSTRAPPROJECTOR_CLASS
