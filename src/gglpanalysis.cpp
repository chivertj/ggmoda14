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
#include "gglpanalysis.h"


//###############################################
void gg::LPAnalysis::operator() (region::IREGv &grndtrths, region::IREGv &colorimgs, gg::LowPass &lowpass, gg::LPHisto &histo) {
  assert(grndtrths.size()==colorimgs.size());

  region::IREGv subregions=gg::PreInitSubRegions(grndtrths[0], gg::_nosubregions);

  cv::Mat pdf;
  for (size_t idx=0;idx<grndtrths.size();idx++) {
    int gtcount=cv::countNonZero(cv::Mat(grndtrths[idx]));
    if (gtcount>0) {
      lowpass(colorimgs[idx]);
      region::ExportImage(lowpass.GetLowPassScaled(),"lpimg",3,idx);
      gg::SubRegions(grndtrths[idx], subregions);
      for (int regidx=0;regidx<subregions.size();regidx++) {
	gg::AnalyseLP(subregions[regidx],lowpass,histo,pdf);
	if (allpdfs.empty()) 
	  allpdfs=cv::Mat::zeros(grndtrths.size(),pdf.cols*gg::_nosubregions,pdf.type());
	cv::Mat allpdfrange=allpdfs(cv::Range(idx,idx+1),cv::Range(regidx*pdf.cols,(regidx+1)*pdf.cols));
	pdf.copyTo(allpdfrange);
      }
    }
  }  
  region::DestroyImages(subregions);
}
//###############################################
void gg::LPAnalysis::operator() (const cv::Mat &fg, const cv::Mat &img, gg::LowPass &lowpass, gg::LPHisto &histo) {
  assert(fg.size()==img.size());
  const IplImage fgc(fg);
  region::IREGv subregions=gg::PreInitSubRegions(&fgc,gg::_nosubregions);
  cv::Mat pdf;
  int gtcount=cv::countNonZero(fg);
  if (gtcount>0) {
    lowpass(img);
    gg::SubRegions(&fgc,subregions);
    for (size_t regidx=0;regidx<subregions.size();regidx++) {
      gg::AnalyseLP(subregions[regidx],lowpass,histo,pdf);
      if (allpdfs.empty())
	allpdfs=cv::Mat::zeros(1,pdf.cols*gg::_nosubregions,pdf.type());
      cv::Mat allpdfrange=allpdfs.colRange(regidx*pdf.cols,(regidx+1)*pdf.cols);
	//allpdfs(1,cv::Range(regidx*pdf.cols,(regidx+1)*pdf.cols));

      pdf.copyTo(allpdfrange);
    }
  }
  region::DestroyImages(subregions);
}
//###############################################
void gg::LPAnalysis::operator() (const cv::Mat &fg, const cv::Mat &img) {
  gg::LowPass lp;
  gg::LPHisto h;
  this->operator()(fg,img,lp,h);
}
//###############################################
void gg::LPAnalysis::Label(std::vector<std::string> &labels) {
  assert(!allpdfs.empty());
  assert(labels.size()==allpdfs.rows);
  if (labelled.rows!=allpdfs.rows || labelled.cols!=allpdfs.cols+1)
    labelled=cv::Mat::zeros(allpdfs.rows,allpdfs.cols+1,allpdfs.type());
  cv::Mat labelrange=labelled.colRange(0,labelled.cols-1);
  allpdfs.copyTo(labelrange);
  for (int idx=0;idx<labels.size();idx++) 
    labelled.at<float>(idx,labelled.cols-1)=gg::GetLabelKey(labels[idx]);
}
//###############################################
void gg::AnalyseLP(const cv::Mat &fgmap, gg::LowPass &lowpass, gg::LPHisto &histo, cv::Mat &pdf) {
  histo(lowpass, fgmap);
  histo.CalcDefault1D();
  histo.Default1D().copyTo(pdf);
}
//###############################################
