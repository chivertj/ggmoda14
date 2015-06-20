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
#ifndef GGLPANALYSIS_NS
#define GGLPANALYSIS_NS

#include "globalCVHeader.h"
#include "region.h"
#include "gghelpers.h"
#include "gglowpass.h"
#include "gglphisto.h"

namespace gg {
  //#######################################
  //#######################################
  class LPAnalysis {
  public:
    LPAnalysis(void) {}
    ~LPAnalysis(void) {}
    void operator() (region::IREGv &grndtrths, region::IREGv &colorimgs, gg::LowPass &lowpass, gg::LPHisto &histo);
    void operator() (const cv::Mat &fg, const cv::Mat &img, gg::LowPass &lowpass, gg::LPHisto &histo);
    void operator() (const cv::Mat &fg, const cv::Mat &img);
    void Label(std::vector<std::string> &labels);
    const cv::Mat& AllPDFs(void) const {return allpdfs;}
    const cv::Mat& Labelled(void) const {return labelled;}
  protected:
    cv::Mat allpdfs;
    cv::Mat labelled;
  };

  void AnalyseLP(const cv::Mat &fgmap, gg::LowPass &lowpass, gg::LPHisto &histo, cv::Mat &pdf);

}

#endif //GGLPANALYSIS_NS
