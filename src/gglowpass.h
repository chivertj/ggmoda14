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
#ifndef GGLOWPASS_NS
#define GGLOWPASS_NS

#include "globalCVHeader.h"


namespace gg {

class LowPass {
public:
  LowPass(void) {}
  virtual ~LowPass(void) {}
  void operator() (const cv::Mat &_img);
  const cv::Mat& GetLowPass(void) const { return lowpassf; }
  const cv::Mat& GetLowPassScaled(void) const { return lowpass; }
protected:
  cv::Mat img;
  cv::Mat imgf;
  cv::Mat lowpass;
  cv::Mat lowpassf;
};
}

#endif //GGLOWPASS_NS
