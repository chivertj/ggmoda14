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
#include "../globalCVHeader.h"
#include "utility.h"
#include <iostream>
#include <fstream>
#include <vector>

#include "is2fsrob.h"
#include "..//region.h"


namespace gg {
  void exportcolm(const cv::Mat &colm, size_t Y, 
		  const std::string &name, 
		  bool transpose=false);
  void exportrowm(const cv::Mat &rowm, size_t X, const std::string &name);
  cv::PCA compressPCA(const cv::Mat& pcaset, int maxComponents,
		      const cv::Mat& testset, cv::Mat& compressed, 
		      cv::Mat &reconstructset);
  cv::PCA robcompressPCA(const cv::Mat& pcaset, int maxComponents,
			 const cv::Mat& testset, 
			 cv::Mat_<float>& compressed, cv::Mat &reconstructset);
  int getdata(const std::string &filename, uchar **data);
  void converttocolmat(uchar *rawdata,
		       size_t nimgs,
		       size_t X, size_t Y, 
		       cv::Mat &colmat);
}
