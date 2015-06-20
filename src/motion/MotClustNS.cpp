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
#include "MotClustNS.h"
#include "../ACDefinitions.h"
#include <sstream>
#include <string>
#include <iostream>

void MotClustNS::SumImgs(IplImage **imgs,int noImgs,IplImage *opImg) {
  IplImage *tmp=cvCloneImage(opImg);
  for (int i=0;i<noImgs;i++)
    cvAdd(imgs[i],tmp,tmp);
  cvCopy(tmp,opImg);
  cvReleaseImage(&tmp);
}
//==================================
void MotClustNS::MultImgs(IplImage **imgs,int noImgs,IplImage *opImg) {
  IplImage *tmp=cvCloneImage(opImg);
  for (int i=0;i<noImgs;i++)
    cvMul(imgs[i],tmp,tmp);
  cvCopy(tmp,opImg);
  cvReleaseImage(&tmp);
}
//==================================
void MotClustNS::ScaleImg(IplImage *ipImg, IplImage *opImg, double scale) {
  double min,max,shift,scalex;
  cvMinMaxLoc(ipImg,&min,&max);
  scalex=(scale/(max-min));
  shift=-min*scalex;
  cvConvertScale( ipImg, opImg, scalex, shift );
}
//==================================
void MotClustNS::RenderPnts(IplImage *img, const CvPoint2D32f *pnts, const int *pntlabels, int nopnts) {
  assert(img);
  assert(pnts);
  assert(pntlabels);
  assert(nopnts>0);
  CvPoint rpnt;
  for (int i=0;i<nopnts;i++) {
    rpnt.x=cvRound(pnts[i].x);
    rpnt.y=cvRound(pnts[i].y);
    if (pntlabels[i]==0)
      cvCircle(img,rpnt,2,CV_RGB(0,0,255),-1,8,0);
    else
      cvCircle(img,rpnt,2,CV_RGB(255,0,0),-1,8,0);
  }
}
//==================================
void MotClustNS::ExportImg(int idx, IplImage *opImg) {
  std::string strIdx=ACDefinitions::StringAndZeroPad(idx,4);
  std::stringstream sstrm1;
  sstrm1 <<"image"<<strIdx<<".jpg";
  std::string currentFrameFilename=sstrm1.str();
  if (opImg->nChannels==3)
    ACDefinitions::ACSaveImgColor(opImg,currentFrameFilename);
  else
    ACDefinitions::ACSaveImg(opImg,currentFrameFilename);
}
//==================================
