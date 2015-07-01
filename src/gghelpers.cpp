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
#include "gghelpers.h"

//###############################################
void gg::FGOutlines(IplImage *fgmap, IplImage *colorop) {
  IplImage *edges=cvCloneImage(fgmap);
  cvCanny(fgmap,edges,0,30);
  cvDilate(edges,edges);
  CvScalar coloredge=cvScalar(0,255,255);
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
//###############################################
region::IDXT gg::LabelHeads(IplImage *fgmap, region::REGIONS &regs, int numregions) {
  cvZero(fgmap);
  region::IDXT headregs;

  uchar *data=(uchar*)(fgmap->imageData);
  uint widthStep=fgmap->widthStep;

  for (size_t i=0;i<numregions;i++) {
    float headsize=regs[i].size()*0.1;
    CvPoint minxminy=region::MinXMinY(regs[i],headsize);
    CvPoint maxxmaxy=region::MaxXMaxY(regs[i],headsize);
    float xdist=maxxmaxy.x-minxminy.x;
    float ydist=maxxmaxy.y-minxminy.y;
    if ( headsize>50 && headsize<100*100 && ydist>0 && 
   ydist>xdist*.4) {
      headregs.push_back(i);
      for (size_t j=0;float(j)<headsize;j++) {
  data[regs[i][j].y*widthStep+regs[i][j].x]=255;
      }
    }
  }
  return headregs;
}
//###############################################
void gg::AddToPDFs(const cv::Mat &pdf, std::string &label, cv::Mat &helmet_pdf, cv::Mat &nohelmet_pdf, int &helmet_cnt, int &nohelmet_cnt) {
  if (helmet_pdf.empty())
    helmet_pdf=cv::Mat::zeros(pdf.size(),pdf.type());
  if (nohelmet_pdf.empty())
    nohelmet_pdf=cv::Mat::zeros(pdf.size(),pdf.type());

  if (label=="n") {
    nohelmet_pdf+=pdf;
    nohelmet_cnt++;
  }
  else if (label=="h") {
    helmet_pdf+=pdf;
    helmet_cnt++;
  }
}
//###############################################
void gg::AddToPDFs(const cv::Mat &pdf, std::string &label, cv::Mat helmet_pdfs[], cv::Mat nohelmet_pdfs[], int helmet_cnts[], int nohelmet_cnts[], int pdfidx) {
  if (label=="n") {
    nohelmet_pdfs[pdfidx]+=pdf;
    nohelmet_cnts[pdfidx]++;
  }
  else if (label=="h") {
    helmet_pdfs[pdfidx]+=pdf;
    helmet_cnts[pdfidx]++;
  }
}
//###############################################
region::IREGv gg::PreInitSubRegions(const IplImage *region, int nosubregions) {
  assert(region);
  assert(nosubregions==4);
  region::IREGv subregions(nosubregions);
  for (int i=0;i<nosubregions;i++) {
    subregions[i]=cvCloneImage(region);
    cvZero(subregions[i]);
  }
  return subregions;
}
//###############################################
void gg::SubRegions(const IplImage *region, region::IREGv &subregions) {
  assert(region);
  assert(subregions.size()==4);
  assert(subregions[0] && subregions[1] && subregions[2] && subregions[3]);
  int nosubregions=4;
  cv::Point cog=region::COG2D<uchar>(region);
  const uchar *rawregdata=(uchar*)(region->imageData);
  uchar *rawsubregions[nosubregions];
  for (int i=0;i<nosubregions;i++) {
    rawsubregions[i]=(uchar*)(subregions[i]->imageData);
    cvZero(subregions[i]);
  }
  for (int y=0;y<region->height;y++) {
    for (int x=0;x<region->width;x++) {
      if (rawregdata[y*region->widthStep+x]) {
  int quadrant=gg::GetQuadrant(cog,x,y);
  rawsubregions[quadrant][y*subregions[quadrant]->widthStep+x]=(uchar)255;
      }
    }
  }
}
//###############################################
void gg::Normalise(cv::Mat pdfs[], int cnts[], int nopdfs) {
  for (int pdfidx=0;pdfidx<nopdfs;pdfidx++) {
    if (cnts[pdfidx]>0)
      pdfs[pdfidx]*=(1./float(cnts[pdfidx]));
    else
      pdfs[pdfidx]=cv::Scalar(0.);
  }
}
//###############################################
void gg::Normalise(cv::Mat &pdf, int cnt) {
  if (cnt>0)
    pdf*=(1./float(cnt));
  else
    pdf=cv::Scalar(0.);
}
//###############################################
void gg::PreInitPDFs(cv::Mat pdfs[], int nosubregions, cv::Size size, int type) {
  for (int i=0;i<nosubregions; i++)
    pdfs[i]=cv::Mat::zeros(size,type);
}
//###############################################
int gg::GetLabelKey(std::string &label) {
  if (label=="h")
    return 1;
  return 0;
}
//###############################################
void gg::MinMaxScale(const cv::Mat &ipdata, cv::Mat &opdata, double newmax) {
  double min,max,scale,shift;
  cv::minMaxLoc(ipdata,&min,&max);
  scale=newmax/(max-min);
  shift=-min*scale;
  ipdata.convertTo(opdata,opdata.type(),scale,shift);
}
//################################
//###############################################
void gg::normalise(const cv::Mat &_hist, cv::Mat &_pdf) {
  assert(_hist.rows==_pdf.rows && _hist.cols==_pdf.cols);
  double normval=cv::norm(_hist,cv::NORM_L1);
  if (normval>0.)
    _pdf=_hist*(1./normval);
  else
    _pdf=cv::Scalar(0.); 
}
//##########################################
//###############################################

