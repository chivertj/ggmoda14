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
#include "mainhelperfuncs.h"

namespace mainhelpers {
  //@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@
  //@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@
  void FGOutlines(IplImage *fgmap, IplImage *colorop, const CvScalar &coloredge, bool thick) {
    IplImage *edges=cvCloneImage(fgmap);
    cvCanny(fgmap,edges,0,30);
    if (thick)
      cvDilate(edges,edges);
    uchar *edgedata=(uchar*)(edges->imageData);
    uint widthStep=edges->widthStep;
    for (int y=0;y<edges->height;y++) {
      for (int x=0;x<edges->width;x++) {
	if (edgedata[y*widthStep+x]>0) 
	  cvSet2D(colorop,y,x,coloredge);
      }
    }
    cvReleaseImage(&edges);
  }
  //@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@
  //@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@
  void LabelRegions(IplImage *img, const gg::frame &f, cv::Scalar color) {
    for (size_t i=0;i<f.size();i++) {
      cv::Mat cvi(img);
      LabelRegion(cvi,f[i],color);
    }
  }
  //@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@
  //@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@
  void LabelRegion(cv::Mat &img, const gg::frameregion &freg, cv::Scalar color) {
    cv::putText(img,std::to_string(freg.uid_hash()),freg.getregprops().getcog(),cv::FONT_HERSHEY_PLAIN,1.,color);
  }
  //@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@
  //@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@
  void DrawBox(const cv::RotatedRect &R, cv::Scalar color, IplImage *colorop) {
    CvPoint2D32f Rvts[4];
    cvBoxPoints(R,Rvts);
    cvLine(colorop,R2Ipnt(Rvts[0]),R2Ipnt(Rvts[1]),color,3);
    cvLine(colorop,R2Ipnt(Rvts[1]),R2Ipnt(Rvts[2]),color,3);
    cvLine(colorop,R2Ipnt(Rvts[2]),R2Ipnt(Rvts[3]),color,3);
    cvLine(colorop,R2Ipnt(Rvts[3]),R2Ipnt(Rvts[0]),color,3);
  }
  //@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@
  //@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@
  void TrimRegions(const region::REGIONS &regs, const size_t nregs, region::REGIONS &newregs, size_t &new_nregs) {
    new_nregs=0;
    for (size_t i=0;i<nregs;i++) {
      if (regs[i].size()>region::MINREGIONSIZE) {
	newregs[new_nregs++]=regs[i];
      }
    }
    if (new_nregs>=region::MAXNOREGIONS) {
      std::vector<bool> sampled(new_nregs);
      region::REGIONS newnewregs(region::MAXNOREGIONSB4TRIM);
      for (size_t i=0;i<new_nregs;i++)
	sampled[i]=false;
      size_t newnew_nregs=0;
      std::srand(time(NULL));
      size_t nloops=0,maxnloops=region::MAXNOREGIONSB4TRIM;
      do {
	size_t sidx=std::rand()%new_nregs;
	if (!sampled[sidx]) {
	  sampled[sidx]=true;
	  newnewregs[newnew_nregs]=newregs[sidx];
	  newnew_nregs++;
	}
	nloops++;
      }while (newnew_nregs<(region::MAXNOREGIONS-1) && nloops<maxnloops);
      newregs=newnewregs;
      new_nregs=newnew_nregs;
    }
  }
  //@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@
  //@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@
  CvPoint R2Ipnt(CvPoint2D32f &r) {
    CvPoint i;
    i.x=cvRound(r.x);
    i.y=cvRound(r.y);
    return i;
  }
  //@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@
  //@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@
  void DrawBoxes(IplImage *img, const gg::frame &framedata, cv::Scalar color) {
    for (size_t i=0;i<framedata.size();i++) {
      const cv::RotatedRect &r=framedata[i].getregprops().getboundbox();
      DrawBox(r,color,img);
      std::cout <<"Reg BOX:"<<r.center.x<<","<<r.center.y<<","<<r.size.width<<","<<r.size.height<<","<<r.angle<<std::endl;
    }
  }
  //@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@
  //@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@
  void Draw_bgheads(IplImage *img, const gg::frame &framedata) {
    region::REGIONPNTS rps;
    IplImage *tmpbinimg=cvCreateImage(cvGetSize(img),IPL_DEPTH_8U,1);
    std::cout <<"Draw_bgheads"<<std::endl;
    for (size_t i=0;i<framedata.size();i++) {
      const cv::Mat &rpsM=
	framedata[i].getregprops().getbgsubregprops().getpnts();
      std::cout <<rpsM.rows<<","<<rpsM.cols<<"\t";
      region::cvMat2REGIONPNTS<int>(rpsM,rps);
	//      region::cvMat2REGIONPNTS<int>(framedata[i].getregprops().getbgsubregprops().getpnts(),rps);
      std::cout <<rps.size()<<std::endl;
      region::SetBinImg(rps,tmpbinimg,true);
      FGOutlines(tmpbinimg,img,cvScalar(0,0,255));
    }
    cvReleaseImage(&tmpbinimg);
  }
  //@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@
  //@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@
}
