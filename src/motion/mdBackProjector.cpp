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
#include "mdBackProjector.h"

namespace md {
cv::Mat fgboosting(const cv::Mat &initfg, const cv::Mat &img, CS_SETTINGS &settings) {
  cv::Mat comb,halo;
  cv::dilate(initfg,comb,cv::Mat(),cv::Point(-1,-1),10);
  cv::subtract(comb,initfg,halo,comb);

  freqdata fd_comb,fd_halo;
  fd_halo.calc(img, halo, settings);
  fd_comb.calc(img, comb, settings);

  cv::Mat halo_likelihood=fd_halo.likelihoodimg(img,comb,settings);
  cv::Mat comb_likelihood=fd_comb.likelihoodimg(img,comb,settings);

  cv::Mat result(initfg.rows,initfg.cols,cv::DataType<float>::type);
  result=cv::Scalar::all(1.);
  result=result-halo_likelihood*0.7/comb_likelihood;

  result.convertTo(result,cv::DataType<uchar>::type,255.);

  cv::bitwise_and(result,initfg,result);

  cv::threshold(result,result,185,255,cv::THRESH_BINARY);

  return result;
}
}

