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
#ifndef GGBGFGSEGMENTERS
#define GGBGFGSEGMENTERS

#include "globalCVHeader.h"
#include "cvaux.h"

namespace gg {
  class MakeBGFGSegmenter {
  public:
    MakeBGFGSegmenter(size_t X, size_t Y): _X(X),_Y(Y),mean(128.),gryimg(0) { gryimg=cvCreateImage(cvSize(X,Y),IPL_DEPTH_8U,1);}
    //################
    virtual ~MakeBGFGSegmenter(void) { cvReleaseImage(&gryimg); }
    //################
    virtual bool operator() (IplImage *img)=0;
    //################
    virtual const IplImage* Get(void) const =0;
    //################
  protected:
    MakeBGFGSegmenter(void) {}
    size_t _X,_Y;
    double mean;
    void adjustmean(IplImage *img, IplImage *mask=0) {
      cvCvtColor(img,gryimg,CV_BGR2GRAY);
      CvScalar currentmean=cvAvg(gryimg,mask);
      cvAddS(img,cvScalarAll(mean-currentmean.val[0]),img);
      mean=(mean+0.01*currentmean.val[0])/1.01;
    }
    IplImage *gryimg;
  };
  //################
  class MakeBGFGSegmenterCodeBook : public MakeBGFGSegmenter {
  public:
    MakeBGFGSegmenterCodeBook(size_t X, size_t Y);
    //################
    virtual ~MakeBGFGSegmenterCodeBook(void);
    //################
    virtual bool operator() (IplImage *img);
    //################
    virtual const IplImage* Get(void) const {return ImaskCodeBookCC;}
    //################
  protected:
    MakeBGFGSegmenterCodeBook(void) :model(0),nframesToLearnBG(150) {}
    CvBGCodeBookModel *model;
    const int nframesToLearnBG;
    int fr;
    IplImage *ImaskCodeBook,*ImaskCodeBookCC;
    IplImage *yuvImg; //=cvCloneImage(queryframe);
  };
  //################
  class MakeBGFGSegmenterStatModel : public MakeBGFGSegmenter {
  public:
    MakeBGFGSegmenterStatModel(IplImage *initimg): MakeBGFGSegmenter(initimg->width,initimg->height) {
      bg_model=cvCreateGaussianBGModel(initimg);
      fgmap=cvCreateImage(cvSize(_X,_Y),IPL_DEPTH_8U,1);
      structelem=cvCreateStructuringElementEx(3,3,1,1,CV_SHAPE_CROSS);
    }
    virtual ~MakeBGFGSegmenterStatModel(void) {
      cvReleaseImage(&fgmap);
      cvReleaseBGStatModel(&bg_model);
      cvReleaseStructuringElement(&structelem);
      std::cout <<"~MakeBGFGSegmenterStatModel"<<std::endl;
    }
    virtual bool operator() (IplImage *img) {
      adjustmean(img);
      cvUpdateBGStatModel(img,bg_model);
      cvCopy(bg_model->foreground,fgmap);
#if(0)
#if(1)
      cvErode(fgmap,fgmap,structelem,2);
      cvDilate(fgmap,fgmap,structelem,2);
#else
      cvDilate(fgmap,fgmap,structelem,2);
      cvErode(fgmap,fgmap,structelem,3);
#endif
#else
#if(0)
      cvDilate(fgmap,fgmap,structelem,2);
      cvErode(fgmap,fgmap,structelem,4);
      cvDilate(fgmap,fgmap,structelem,2);
#else
#endif
#if(0)
      cvDilate(fgmap,fgmap,structelem,2);
      cvErode(fgmap,fgmap,structelem,4);
      cvDilate(fgmap,fgmap,structelem,4);
#else
      cvDilate(fgmap,fgmap,structelem,2);
      cvErode(fgmap,fgmap,structelem,4);
      cvDilate(fgmap,fgmap,structelem,6);
#endif
#endif
      return true;
    }
    virtual const IplImage* Get(void) const {return fgmap;}
  protected:
    CvBGStatModel *bg_model;
    IplImage *fgmap;
    IplConvKernel *structelem;
  };
  //################
}

#endif //GGBGFGSEGMENTERS
