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
#include "WJImgNS.h"
//#include "FiniteDiffExNS.h"
#include "SignedDistT.h"
#include "ShapeContour.h"

//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
void WJImgNS::SetBorder(IplImage *img, CvScalar borderVal, int borderSize) {
  assert(img);
  int width=img->width,height=img->height;
  int x,y;
  for (y=0;y<height;y++) {
    for (int i=0;i<borderSize;i++) {
      x=i;
      cvSet2D(img,y,x,borderVal);
      x=width-i-1;
      cvSet2D(img,y,x,borderVal);
    }
  }
  for (x=0;x<width;x++) {
    for (int i=0;i<borderSize;i++) {
      y=i;
      cvSet2D(img,y,x,borderVal);
      y=height-1-i;
      cvSet2D(img,y,x,borderVal);
    }
  }
}
//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
void WJImgNS::MasksToSDs(region::IREGv &masks, region::IREGv &masksds) {
  assert(masks.size()>0);
  if (masksds.size()>0)
    region::DestroyImages(masksds);
  masksds.resize(masks.size());
  for (int i=0;i<masks.size();i++)
    masksds[i]=cvCreateImage(cvGetSize(masks[i]),ACTIVE_IPLDEPTH,1);
  IplImage *tmpbound=cvCreateImage(cvGetSize(masks[0]),IPL_DEPTH_8U,1);

  CSignedDistT distT;
  distT.Init(tmpbound,255);
  CShapeContour<float> contour;
  for (int i=0;i<masks.size();i++) {
    contour.InitWithMask(masks[i]);
    contour.PerformOp();
    cvConvertScale(contour.BorrowContourImg(),tmpbound);
    distT.PerformOp();
    cvCopy(distT.BorrowSignedDistT(),masksds[i]);
  }
  cvReleaseImage(&tmpbound);
}
//~#~#~#~#~#~#~#~#~#~#~#~#~#~#~#~#~#~#~#~#~#~#~#~#~#~#~#~#~#~#~#~#~
bool WJImgNS::CheckImages(const region::IREGv &imgs) {
  bool match=true;
  for (uint i=0;i<imgs.size()-1;i++) {
    if ((match=CheckDims(imgs[i],imgs[i+1]))==false)
      break;
  }
  return match;
}
//~#~#~#~#~#~#~#~#~#~#~#~#~#~#~#~#~#~#~#~#~#~#~#~#~#~#~#~#~#~#~#~#~
bool WJImgNS::CheckDims(IplImage *im1, IplImage *im2) {
  if (!im1 || !im2 ||
      im1->width != im2->width ||
      im1->height != im2->height ||
      im1->depth != im2->depth ||
      im1->nChannels != im2->nChannels) 
    return false;
  return true;
}
//~#~#~#~#~#~#~#~#~#~#~#~#~#~#~#~#~#~#~#~#~#~#~#~#~#~#~#~#~#~#~#~#~
void WJImgNS::MaskToSD(IplImage *mask, IplImage *sd) {
  assert(mask);
  assert(sd);
  IplImage *tmpbound=cvCreateImage(cvGetSize(mask),IPL_DEPTH_8U,1);
  CShapeContour<float> contour;
  contour.InitWithMask(mask);
  contour.PerformOp();
  cvConvertScale(contour.BorrowContourImg(),tmpbound);
  CSignedDistT distT;
  distT.Init(tmpbound,255);
  distT.PerformOp();
  cvCopy(distT.BorrowSignedDistT(),sd);
  cvReleaseImage(&tmpbound);
}
//~#~#~#~#~#~#~#~#~#~#~#~#~#~#~#~#~#~#~#~#~#~#~#~#~#~#~#~#~#~#~#~#~
void WJImgNS::Intersect(IplImage *in_sd1, IplImage *in_sd2, IplImage *out_sd) {
  assert(in_sd1 && in_sd2 && out_sd);
  assert(CheckDims(in_sd1,in_sd2) && CheckDims(in_sd2,out_sd));
  IplImage *tmp1=cvCloneImage(in_sd1);
  IplImage *tmp2=cvCloneImage(in_sd2);
  float zerothreshold=0; //this is used by xkStatic but ShPredict uses 0.2
  cvThreshold(in_sd1,tmp1,zerothreshold,255,CV_THRESH_BINARY_INV);
  cvThreshold(in_sd2,tmp2,zerothreshold,255,CV_THRESH_BINARY_INV);
  cvMul(tmp1,tmp2,out_sd);
  WJImgNS::MaskToSD(out_sd,out_sd);
  cvReleaseImage(&tmp1);
  cvReleaseImage(&tmp2);
}
//~#~#~#~#~#~#~#~#~#~#~#~#~#~#~#~#~#~#~#~#~#~#~#~#~#~#~#~#~#~#~#~#~
