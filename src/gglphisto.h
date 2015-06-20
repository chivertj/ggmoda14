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
#ifndef GGLPHISTO_NS
#define GGLPHISTO_NS

#include "globalCVHeader.h"
#include "gglowpass.h"

namespace gg {
  class LPHisto {
  public:
    LPHisto(void) {}
    virtual ~LPHisto(void) {}
    virtual void operator() (const LowPass &lowpass, const cv::Mat &mask, int _nbins=30);
    virtual void CalcDefault1D(void);
    const cv::Mat& Default1D(void) const {return default1d;}
    const cv::MatND& PDF(void) const {return pdf;}
  protected:
    cv::Mat default1d;
    cv::MatND pdf;
    cv::MatND hist;
  };
}

#endif //GGLPHISTO_NS

