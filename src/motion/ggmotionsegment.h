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
#ifndef GGMOTIONSEGMENT
#define GGMOTIONSEGMENT

#include "../globalCVHeader.h"
#include "../ggbgfgsegmenters.h"
#include "mdShapeCluster.h"

namespace gg {
  class MakeBGFGSegmenterLKMotion : public MakeBGFGSegmenter {
  public:
    //################
    MakeBGFGSegmenterLKMotion(size_t X, size_t Y) : MakeBGFGSegmenter(X,Y),fgmap(Y,X,cv::DataType<uchar>::type),shapecluster(cv::Rect(0,0,X,Y)),tmpcntr(0) {
       cfgmap=IplImage(fgmap);
    }
    //################
    virtual ~MakeBGFGSegmenterLKMotion(void) { }
    //################
    virtual bool operator() (IplImage *img) {
      adjustmean(img);
      shapecluster(img);
#if(0)
      fgmap=cv::Scalar(0);
      for (size_t i=0;i<shapecluster.clusters().NoRegions();i++) {
  shapecluster.backproject(i);
  cv::bitwise_or(shapecluster.getbp(),fgmap,fgmap);
      }
      cv::threshold(fgmap,fgmap,200,255,cv::THRESH_BINARY);
      cfgmap=IplImage(fgmap);
#else
      std::cout <<"No of clusters:"<<shapecluster.clusters().NoRegions()<<std::endl;
      fgmap=cv::Mat(shapecluster.clusters().BorrowMask()).clone();
      cfgmap=IplImage(fgmap);
#endif
    }
    //################
    virtual const IplImage* Get(void) const { return &cfgmap; }
    //################
    void render(IplImage *img) {
      assert(img);
      int nopnts=0;
      const CvPoint2D32f* allpnts=shapecluster.featpnts().BorrowAllPnts(nopnts);
      const int *labels=shapecluster.featpnts().BorrowLabels();
      MotClustNS::RenderPnts(img,allpnts,labels,nopnts);
    }
  protected:
    cv::Mat fgmap;
    IplImage cfgmap;
    md::ShapeCluster shapecluster;    

    int tmpcntr;
  };
  
}
#endif //GGMOTIONSEGMENT
