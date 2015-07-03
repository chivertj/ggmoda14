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
#include "ACDefinitions.h"
#include <iostream>
#include <sstream>
//=============================================================
void ACDefinitions::ACCentralDiff(IplImage *img, IplImage *result, int xorder, int yorder, int aperture_size, IplImage *tmp) {
  cvSobel(img,tmp,xorder,yorder,aperture_size);
  cvConvertScale(tmp, result, 0.5 );
}
//=============================================================
void ACDefinitions::ACSaveImg(const IplImage *img, const std::string &filename) {
  IplImage *scaled = cvCreateImage(cvGetSize(img),IPL_DEPTH_8U,1);
  double min,max;
  cvMinMaxLoc(img,&min,&max);
#if(1) //bad dynamic range check
  if (min < -100.)  min = -100.;
  if (max >  100.)  max =  100.;
#endif
  //    min=-5;
  //    max=5;
  double scale,shift;

  if (min>max) {
    double tmp=min;
    min=max;
    max=tmp;
  }
  if ((max-min)>ACSMALL){
    scale=255/(max-min);
    shift=-min*scale;
    cvConvertScale(img,scaled,scale,shift);
  }
  else {
    cvZero(scaled);
  }
  cvSaveImage(filename.c_str(),scaled);
  cvReleaseImage(&scaled);
}
//=============================================================
void ACDefinitions::ACSaveImgColor(const IplImage *img, const std::string &filename) {
  if (img->nChannels==3) {
    IplImage *scaled = cvCreateImage(cvGetSize(img),IPL_DEPTH_8U,3);
    cvConvertScale(img,scaled);
    cvSaveImage(filename.c_str(),scaled);
    cvReleaseImage(&scaled);
  }
  else { ACSaveImg(img,filename); std::cout <<"Saving grey scale image."<<std::endl; }
}
//=============================================================
void ACDefinitions::ACZeroContourAndColourMergeAndSave(IplImage *image, IplImage *levelSet, const std::string &filename) {
  IplImage *imgToSave=ACZeroContourAndColourMerge(image,levelSet);
  cvSaveImage(filename.c_str(),imgToSave);
  cvReleaseImage(&imgToSave);
}
//=============================================================
IplImage* ACDefinitions::ACZeroContourAndColourMerge(IplImage *image, IplImage *levelSet) {
  assert(levelSet&&image);
  IplImage *zeroContour = cvCreateImage(cvGetSize(levelSet),IPL_DEPTH_8U,3);
  cvConvertScale(image,zeroContour);
  int X=levelSet->width,Y=levelSet->height;
  float *rawLevelSet = (float*)(levelSet->imageData);

  int i;
  //    CvScalar contourCol=cvScalar(255,0,0);
  //    CvScalar contourCol=cvScalar(255,0,255);
  CvScalar contourCol=cvScalar(0,255,255);
  //    CvScalar contourCol=cvScalar(0,0,255);
  for (int y=1;y<Y-1;y++) {
    for (int x=1;x<X-1;x++) {
      i=y*X+x;
      if (rawLevelSet[i]>(-ACLITTLENUMBER) && rawLevelSet[i]<ACLITTLENUMBER ) {
        //    cvSet2D(zeroContour,y,x,cvScalarAll(255));
        cvSet2D(zeroContour,y,x,contourCol);
        cvSet2D(zeroContour,y,x-1,contourCol);
        cvSet2D(zeroContour,y+1,x,contourCol);
      }
    }
  }
  return zeroContour;
}
//=============================================================
IplImage* ACDefinitions::ACPosNegThreshold(IplImage *levelSet, bool negThresh) {
  assert(levelSet);
  IplImage *threshold=cvCreateImage(cvGetSize(levelSet),IPL_DEPTH_8U,1);
  cvZero(threshold);
  int X=levelSet->width,Y=levelSet->height;
  //    IMAGEBYTE_T *rawLevelSet=(IMAGEBYTE_T*)(levelSet->imageData);
  float *rawLevelSet=(float*)(levelSet->imageData);
  IMAGEBYTE_T *rawThreshold=(IMAGEBYTE_T*)(threshold->imageData);
  float levelSetVal;
  for (int y=0;y<Y;y++) {
    for (int x=0;x<X;x++) {
      levelSetVal=rawLevelSet[y*X+x];
      if ((negThresh && levelSetVal<=0.) || (!negThresh && levelSetVal>=0.)) {
        rawThreshold[y*X+x]=255;
      }
    }
  }
  return threshold;
}
//=============================================================
std::string ACDefinitions::StringAndZeroPad(int digit,int noDigits) {
  std::stringstream sstrm;
  int noZeros=noDigits-int(log10(digit)+1.);
  if (digit==0 && noDigits>0) noZeros=noDigits-1; 
  for (int i=0;i<noZeros;i++)
    sstrm<<"0";
  sstrm<<digit;
  return sstrm.str();
}
//=============================================================
std::string ACDefinitions::ExportStr(std::string &idx, const char *fileBase) {
  std::stringstream sstrm;
  sstrm <<fileBase<<idx<<".jpg";
  std::string opFilename=sstrm.str();
  return opFilename;
}
//=============================================================
std::string ACDefinitions::ExportStrSub(std::string &idx, std::string &subIdx, const char *fileBase) {
  std::stringstream sstrm;
  sstrm <<fileBase<<idx<<"_"<<subIdx<<".jpg";
  std::string opFilename=sstrm.str();
  return opFilename;
}
//=============================================================
void ACDefinitions::ExportImg(IplImage *img, const char *filenameBase, bool posNegThresh, int idx) {
  IplImage *tmpImg=img;
  if (posNegThresh)
    tmpImg=ACDefinitions::ACPosNegThreshold(img);
  std::string opFilename(filenameBase);
  opFilename+=ACDefinitions::StringAndZeroPad(idx,3)+".jpg";
  ACDefinitions::ACSaveImg(tmpImg,opFilename);
  if (posNegThresh)
    cvReleaseImage(&tmpImg);
}
//--------------------------
void ACDefinitions::ExportImg(IplImage *img, const char *filenameBase, bool posNegThresh) {
  IplImage *tmpImg=img;
  if (posNegThresh)
    tmpImg=ACDefinitions::ACPosNegThreshold(img);
  std::string opFilename(filenameBase);
  opFilename+=".jpg";
  ACDefinitions::ACSaveImg(tmpImg,opFilename);
  if (posNegThresh)
    cvReleaseImage(&tmpImg);
}
//--------------------------
