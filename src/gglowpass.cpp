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
#include "gglowpass.h"
#include "gghelpers.h"

namespace gg {
  void LowPass::operator() (const cv::Mat &_img) {
    assert(_img.type()==cv::DataType<cv::Vec3b>::type);
    if (img.empty()) {
      img=cv::Mat::zeros(_img.rows,_img.cols,cv::DataType<uchar>::type);
      imgf=cv::Mat::zeros(_img.rows,_img.cols,cv::DataType<float>::type);
      lowpass=cv::Mat::zeros(_img.rows,_img.cols,cv::DataType<uchar>::type);
      lowpassf=cv::Mat::zeros(_img.rows,_img.cols,cv::DataType<float>::type);
    }
    cv::cvtColor(_img,img,CV_RGB2GRAY);
    img.convertTo(imgf,imgf.type());
    //    cv::GaussianBlur(imgf,lowpassf,cv::Size(7,7),3); //5->2, 3->0.8
    cv::GaussianBlur(imgf,lowpassf,cv::Size(11,11),3); //5->2, 3->0.8
    //    imgf.copyTo(lowpassf);
    //  MinMaxScale(lowpassf,lowpass);
    lowpassf.convertTo(lowpass,lowpass.type());
  }
}
