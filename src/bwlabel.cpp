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
#include "bwlabel.h"
#include "globalCVHeader.h"
#include <vector>
#include <map>
using std::vector;
using std::make_pair;

typedef unsigned short lblT;
typedef std::pair<lblT,lblT> lblpair;
typedef std::map<lblT,lblT> lblmap;
typedef lblmap::iterator lblit;
#include <iostream>

#include "connected.h"

CBWLabel::CBWLabel(void) :  I(0), IUS(0), width(0), height(0), regions(maxnoregions),  labelimg(0), labelimgUS(0), label_count(0),  labelimgC(0), iregs(maxnoregions) {
  for (int i=0;i<maxnoregions;i++)
    iregs[i]=0;
}

void CBWLabel::Init(IplImage *ipimg) {
  assert(ipimg->nChannels==1);
  assert(ipimg->depth==IPL_DEPTH_8U);
  I=ipimg;

  if (!labelimg) {
    width=ipimg->width;
    height=ipimg->height;

    labelimg=cvCloneImage(ipimg);
    labelimgC=cvCreateImage(cvGetSize(ipimg),IPL_DEPTH_8U,3);

    labelimgUS=cvCreateImage(cvSize(width,height),IPL_DEPTH_16U,1);

    cvZero(labelimg);
    cvZero(labelimgC);
    cvZero(labelimgUS);

    for (int i=0;i<maxnoregions;i++) {
      iregs[i]=cvCreateImage(cvSize(ipimg->width,ipimg->height),IPL_DEPTH_8U,1);
      cvZero(iregs[i]);
    }

    IUS=cvCreateImage(cvSize(width,height),IPL_DEPTH_16U,1);
  }
}

CBWLabel::~CBWLabel(void) {
  cvReleaseImage(&labelimg);
  cvReleaseImage(&labelimgC);
  for (int i=0;i<maxnoregions;i++)
    cvReleaseImage(&iregs[i]);
  cvReleaseImage(&IUS);
  cvReleaseImage(&labelimgUS);
  std::cout <<"~CBWLabel"<<std::endl;
}

IREGv CBWLabel::BorrowIRegions(void) {
  return iregs;
}

IREG CBWLabel::BorrowIRegion(int idx) {
  assert(idx>=0 && idx<label_count);
  return iregs[idx];
}

IplImage* CBWLabel::BorrowLabelImg(void) {
  return labelimg;
}

IplImage* CBWLabel::BorrowCLabelImg(void) {
  return labelimgC;
}

void CBWLabel::MakeIRegs(void) {
  for (int i=0;i<maxnoregions;i++)
    cvZero(iregs[i]);
  uchar *pixel;
  int i,j,ws;
  for (int rx=0;rx<label_count;rx++) {
    pixel=(uchar*)(iregs[rx]->imageData);
    ws=iregs[rx]->widthStep;
    for (uint px=0;px<regions[rx].size();px++) {
      i=regions[rx].at(px).x;
      j=regions[rx].at(px).y;
      pixel[j*ws+i]=255;
    }
  }
}

void CBWLabel::MakeLabelImg(void) {
  int i,j;
  cvZero(labelimg);
  uchar *pixel=(uchar*)(labelimg->imageData);
  for (int rx=0;rx<label_count;rx++) {
    for (uint px=0;px<regions[rx].size();px++) {
      j=regions[rx].at(px).x;
      i=regions[rx].at(px).y;
      pixel[i*labelimg->widthStep+j]=rx+1;
    }
  }
}

void CBWLabel::MakeCLabelImg(void) {
  cvZero(labelimgC);
  int l,h,w,r,g,b;
  uchar *Lpixel=(uchar*)labelimgC->imageData;
  for(l=0;l<label_count;l++){
    r=rand();
    g=rand();
    b=rand();
    for(uint i=0;i<regions[l].size();i++){
      h=regions[l].at(i).y;
      w=regions[l].at(i).x;
      Lpixel[h*labelimgC->widthStep +w*3 +0]=b;
      Lpixel[h*labelimgC->widthStep +w*3 +1]=g;
      Lpixel[h*labelimgC->widthStep +w*3 +2]=r;
    }
  }
}

int CBWLabel::CalcRegions(void){
  cvConvert(I,IUS);

  for (int ridx=0;ridx<maxnoregions;ridx++)
    regions[ridx].clear();
  label_count=0;

  cvConvert(I,IUS);
  cvConvert(labelimg,labelimgUS);

  ConnectedComponents cc(maxnoregions);
  label_count=cc.process( (lblT *)(IUS->imageData),
      (lblT *)(labelimgUS->imageData),
      width, height);

  cvConvertScale(IUS,IUS,1./255.);
  cvMul(IUS,labelimgUS,labelimgUS);

#if(1) //find regions here and relabel using refreshed labels.
  lblmap unique_labels;
  lblit iter;
  CvPoint pt;
  lblT *data=(lblT*)(labelimgUS->imageData);
  lblT tmplblcnt=0;
  for(int i=0;i<height;i++){
    for(int j=0;j<width;j++,data++){
      if ((*data)!=0) {
        pt.y=i;
        pt.x=j;
        iter=unique_labels.find(*data);
        if (iter!=unique_labels.end())
          *data=(*iter).second;
        else {
          tmplblcnt++;
          unique_labels.insert(make_pair(*data,tmplblcnt));
          *data=tmplblcnt;
        }
        if ((*data)<=maxnoregions)
          regions[*data-1].push_back(pt);
      }
    }
  }
  label_count=tmplblcnt;

  if (label_count>maxnoregions)
    label_count=maxnoregions;
#endif

  //  cvConvertScale(labelimgUS,labelimgUS,10);
  cvConvert(labelimgUS,labelimg);

  return label_count;
}

