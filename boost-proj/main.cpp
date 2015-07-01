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
#include <iostream>
#include <string>
using namespace std;
#include "../src/globalCVHeader.h"
#include "../src/motion/BootStrapProjector.h"

void LocalBSProj(IplImage *img, IplImage *globalmask, IplImage *op);

#if(0)
int main (int argc, char **argv ) {
  if (argc<=3) 
    return -1;
  std::string imagename(argv[1]),fgname(argv[2]),opname(argv[3]);
  cv::Mat image=cv::imread(imagename,1),foreground=cv::imread(fgname,0);
  cv::Mat halo,combined;
  double t=(double)cv::getTickCount();
  cv::dilate(foreground,combined,cv::Mat(),cv::Point(-1,-1),10);
  cv::subtract(combined,foreground,halo);

  IplImage cimage=image,chalo=halo,cforeground=foreground,ccombined=combined;
  CBootStrapProjector bsproject;
  bsproject.Init(&cimage);
  bsproject.PerformOp(&chalo,&cforeground,&ccombined);
  t=((double)cv::getTickCount()-t)/cv::getTickFrequency();
  std::cout <<"Time:\t"<<t<<std::endl;
  const cv::Mat result(bsproject.BorrowResult());
  //cv::imwrite(opname,bsproject.BorrowResult());
  cv::imwrite(opname,result);
  return 0;
}
#else
int main (int argc, char **argv) {
  if (argc<=3) return -1;
  std::string imagename(argv[1]),fgname(argv[2]),opname(argv[3]);
  cv::Mat image=cv::imread(imagename,1),foreground=cv::imread(fgname,0);
  cv::threshold(foreground,foreground,128.,255.,cv::THRESH_BINARY);
  cv::Mat op=foreground.clone();
  op=cv::Scalar(0);
  IplImage cimage=image,cforeground=foreground,cop=op;
  double t=(double)cv::getTickCount();
  LocalBSProj(&cimage,&cforeground,&cop);
  t=((double)cv::getTickCount()-t)/cv::getTickFrequency();
  std::cout <<"Time:\t"<<t<<std::endl;
  cv::imwrite(opname,op);
}
#endif

#include "../src/shape/WJImgNS.h"
#include "../src/shape/SignedDistT.h"
#include "../src/shape/ShapeContour.h"
#include "../src/ACDefinitions.h"
#include "../src/shape/DistTSkel.h"
#include "../src/shape/PntMinDists.h"
#include "../src/shape/Thinning.h"

#define DISTSKEL

void LocalBSProj(IplImage *img, IplImage *globalmask, IplImage *op) {
  assert(img && globalmask && op);
  assert(WJImgNS::CheckDims(globalmask,op));
  assert(img->width==globalmask->width && img->height==globalmask->height);

  CShapeContour<int> contour;
  contour.InitWithMask(globalmask);
  contour.PerformOp();
  int nopnts=0;
  ACDefinitions::ACPnt<int> *contourpnts=contour.BorrowContourPnts(nopnts);

  IplImage *contourimg=cvCloneImage(globalmask);
  cvConvertScale(contour.BorrowContourImg(),contourimg);

  IplImage *skelimg=cvCreateImage(cvGetSize(globalmask),IPL_DEPTH_32F,1);

#if(1)
  CSignedDistT distT;
  distT.Init(contourimg,255);
  distT.PerformOp();


  IplImage *dists=cvCloneImage(distT.BorrowSignedDistT());

  CDistTSkel skel;
  skel.Init(distT.BorrowSignedDistT(),true);
  skel.PerformOp();

  cvConvertScale(skel.BorrowSkel(),skelimg);
#else
  CThinning skel;
  skel.Init(globalmask);
  skel.PerformOp();
  cvConvertScale(skel.BorrowResultImage(),skelimg);
#endif

  ACDefinitions::ACSaveImg(skelimg,"skel.png");

  CPntMinDists mindists;
  mindists.Calc(contourpnts,nopnts,skelimg);

  std::vector<double> contourmindists=mindists.BorrowMinDists();

  IplImage *localmask=cvCloneImage(globalmask);
  IplImage *localfgbg=cvCloneImage(globalmask);
  IplImage *localhalo=cvCloneImage(globalmask);
  IplImage *globalhalo=cvCloneImage(globalmask);
  cvDilate(globalhalo,globalhalo,NULL,5);
  cvSub(globalhalo,globalmask,globalhalo);
  CBootStrapProjector bsproject;
  bsproject.Init(img);

  IplImage *opsum=cvCreateImage(cvGetSize(img),IPL_DEPTH_32F,1);
  cvZero(opsum);
  IplImage *tmpop=cvCreateImage(cvGetSize(img),IPL_DEPTH_32F,1);
  IplImage *div=cvCreateImage(cvGetSize(img),IPL_DEPTH_32F,1);
  IplImage *tmplocalfgbg=cvCreateImage(cvGetSize(img),IPL_DEPTH_32F,1);
  cvZero(div);
  const double fraction=.75; //.75
  const double minradius=10; //20
  const double pntfrac=1./nopnts; //1./nopnts; //55
  std::cout <<nopnts<<"\t"<<pntfrac<<"\t"<<nopnts*pntfrac<<std::endl;

  int incval=std::ceil(nopnts*pntfrac);
  if (incval==0) incval=1;
  for (int i=0;i<nopnts;i+=incval) {
    int radius=cvRound(contourmindists[i])*2.; //*2.;
    if (radius>minradius) {
      CvPoint centre=cvPoint(contourpnts[i].x,contourpnts[i].y);
      cvZero(localmask);
      cvCircle(localmask,centre,radius,cvScalarAll(255),-1);
      cvAnd(localmask,globalmask,localfgbg);
      cvSub(localmask,localfgbg,localhalo);
      cvAnd(localhalo,globalhalo,localhalo);

      bsproject.PerformOp(localhalo,localfgbg,localmask);
      CvScalar sum=cvSum(bsproject.BorrowResult());
      double fgbgcount=cvCountNonZero(localfgbg);
      //      std::cout <<sum.val[0]<<","<<fgbgcount<<","<<fgbgcount*255<<","<<fgbgcount*255*fraction<<std::endl;

      if (sum.val[0]<fgbgcount*255*fraction) { 
  cvConvertScale(bsproject.BorrowResult(),tmpop);
  cvAdd(tmpop,opsum,opsum);
  cvConvertScale(localfgbg,tmplocalfgbg);
  cvAdd(div,tmplocalfgbg,div);
      }
    }
  }
  cvDiv(opsum,div,opsum);
  cvErode(opsum,opsum,NULL,1);

  double min=0,max=0;
  cvMinMaxLoc(opsum,&min,&max);
  double scale=255/(max-min);
  double shift=-min*scale;
  cvConvertScale(opsum,op,scale,shift);

  cvReleaseImage(&div);
  cvReleaseImage(&skelimg);
  cvReleaseImage(&tmpop);
  cvReleaseImage(&opsum);
  cvReleaseImage(&localhalo);
  cvReleaseImage(&localfgbg);
  cvReleaseImage(&localmask);
  cvReleaseImage(&contourimg);
}
