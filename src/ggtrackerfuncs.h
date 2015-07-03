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
#ifndef GGTRACKERFUNCS_NS
#define GGTRACKERFUNCS_NS

#include "region.h"
#include "ggtracker.h"
#include "ggframe.h"
#include "ggregionhierarchy.h"
#include "ggclassify.h"
#include "ggclassification.h"

namespace gg {
//#####################
//#####################
void SubsetRegions(const region::REGIONS &allregs, const std::vector<size_t> idxs, region::REGIONS &subsetregs) {
  subsetregs=region::REGIONS(idxs.size());
  for (size_t i=0;i<idxs.size();i++)
    subsetregs[i]=allregs[idxs[i]];
}
//#####################
void LoadFrame(const region::REGIONS &regs, size_t nregs, const cv::Mat &img, frame &f) {
  f.unset();
  cv::Mat mask(img.size(),cv::DataType<uchar>::type);
  IplImage cmask(mask);
  f.resize(nregs);
  std::cout <<"Constructing regions"<<std::endl;
  for (size_t i=0;i<nregs;i++) {
    if (regs[i].size()>0) {
      region::SetBinImg(regs[i],&cmask,true);
      cv::Mat regpnts=cv::Mat(regs[i]);
      regionhierarchy regprops(img,mask,regpnts);
      std::cout <<"REGION:"<<i<<"\t"
          <<"CENTER:"<<regprops.getboundbox().center.x<<","<<regprops.getboundbox().center.y<<"\t"
          <<"SIZE:"<<regprops.getboundbox().size.width<<","<<regprops.getboundbox().size.height<<"\t";
      f[i].set(mask,regprops);
      std::cout <<"ID:"<<f[i].uid_hash()<<std::endl;
    }
  }
  //    f.recountvalid();
  f.set(img);
}
//#####################
void LoadFrame(const std::vector<cv::Rect> &regs, size_t nregs, const cv::Mat &img, frame &f) {
  f.unset();
  cv::Mat mask(img.size(),cv::DataType<uchar>::type);
  f.resize(nregs);
  std::cout <<"Constructing regions"<<std::endl;
  for (size_t i=0;i<nregs;i++) {
    mask=cv::Scalar(0);
    cv::rectangle(mask,regs[i],cv::Scalar(255));
    REGIONPNTS r;
    region::ExtractPnts(mask,r,false);
    cv::Mat regpnts=cv::Mat(r);

    regionhierarchy regprops(img,mask,regpnts);
    std::cout <<"REGION:"<<i<<"\t"
        <<"CENTER:"<<regprops.getboundbox().center.x<<","<<regprops.getboundbox().center.y<<"\t"
        <<"SIZE:"<<regprops.getboundbox().size.width<<","<<regprops.getboundbox().size.height<<"\t";
    f[i].set(mask,regprops);
    std::cout <<"ID:"<<f[i].uid_hash()<<std::endl;
  }
  //    f.recountvalid();
  f.set(img);
}
//#####################
int LoadFrame(const region::REGIONS &regs, size_t nregs, const cv::Mat &img, const HeadClassifier &headclass, const Heads &heads, frame &f) {
  f.unset();

  //$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$
  std::vector<size_t> headidxs;
  std::vector<float> headproportions;
  heads.Find(headidxs,headproportions,regs,nregs);
  int nmotorbikes=headidxs.size();
  //$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$
  cv::Mat mask(img.size(),cv::DataType<uchar>::type);
  IplImage cmask(mask);
  helmetclassifier clfr(headclass);

  region::REGIONPNTS headpnts;
  f.resize(nmotorbikes);
  for (size_t j=0;j<nmotorbikes;j++) {
    size_t i=headidxs[j];

    region::SetBinImg(regs[i],&cmask,true);
    cv::Mat regpnts=cv::Mat(regs[i]);
    regionhierarchy regprops(img,mask,regpnts);
    regprops.set(img,mask,regpnts);

    f[j].set(mask,regprops);

    heads.Make(regs[i],headproportions[j],headpnts);
    region::SetBinImg(headpnts,&cmask,true);
    regpnts=cv::Mat(headpnts);
    regionproperties subregprops(img,mask,regpnts);
    f[j].getregprops().setsubregprops(subregprops);
    discreteclassification c=clfr.classify(f[j].getregprops());
    f[j].getregprops().getsubregprops().setclassification(c);
  }
  f.set(img);

  return nmotorbikes;
}
//#####################
int LoadFrameWithBG(const region::REGIONS &regs, size_t nregs, const cv::Mat &img, const HeadClassifier &headclass, const Heads &heads, frame &f) {
  f.unset();

  //$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$
  std::vector<size_t> headidxs;
  std::vector<float> headproportions;
  heads.Find(headidxs,headproportions,regs,nregs);
  int nmotorbikes=headidxs.size();
  //$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$

  cv::Mat mainmask(img.size(),cv::DataType<uchar>::type);
  IplImage cmainmask(mainmask);
  helmetclassifier clfr(headclass);

  cv::Mat headmask(img.size(),cv::DataType<uchar>::type);
  IplImage cheadmask(headmask);

  cv::Mat bgheadmask(img.size(),cv::DataType<uchar>::type);
  IplImage cbgheadmask(bgheadmask);

  region::REGIONPNTS headpnts;
  region::REGIONPNTS bgheadpnts;

  f.resize(nmotorbikes);
  CvPoint2D32f corners[4];
  cv::Point icorners[4];
  for (size_t j=0;j<nmotorbikes;j++) {
    size_t i=headidxs[j];

    region::SetBinImg(regs[i],&cmainmask,true);
    cv::Mat regpnts=cv::Mat(regs[i]);
    regionhierarchy regprops(img,mainmask,regpnts);
    f[j].set(mainmask,regprops);

    heads.Make(regs[i],headproportions[j],headpnts);
    region::SetBinImg(headpnts,&cheadmask,true);
    regpnts=cv::Mat(headpnts);
    regionproperties subregprops(img,headmask,regpnts);
    f[j].getregprops().setsubregprops(subregprops);
    discreteclassification c=clfr.classify(f[j].getregprops());
    f[j].getregprops().getsubregprops().setclassification(c);

    bgheadmask=cv::Scalar(0);
    cvBoxPoints(subregprops.getboundbox(),corners);
    region::pntsR2I<4>(corners,icorners);
    cv::fillConvexPoly(bgheadmask,icorners,4,cv::Scalar(255));
    cv::dilate(bgheadmask,bgheadmask,cv::Mat(),cv::Point(-1,-1),2);
    cv::bitwise_not(mainmask,mainmask);
    cv::bitwise_and(bgheadmask,mainmask,bgheadmask);
    region::ExtractPnts(bgheadmask,bgheadpnts,true);
    regpnts=cv::Mat(bgheadpnts);
    if (regpnts.rows>0 && regpnts.cols>0) {
      regionproperties bgsubregprops(img,bgheadmask,regpnts);
      f[j].getregprops().setbgsubregprops(bgsubregprops);
    }
  }
  f.set(img);

  return nmotorbikes;
}
//#####################

}

#endif //GGTRACKERFUNCS_NS

//convert a vector of regionpnts (region::REGIONS) to a single frame.
//compute feature vector (check length is correct)
//load feature vector into individual regions
//

