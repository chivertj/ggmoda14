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
/*
 * ggshapetrackfilter.h
 *
 *  Created on: Mar 28, 2012
 *      Author: jpc
 */

#ifndef GGSHAPETRACKFILTER_H_
#define GGSHAPETRACKFILTER_H_

#include "ggshapetrackproperties.h"
#include "../pca/utility.h"
#include "../pca/ggpca.h"
#include "ggregconsist.h"
#include "../ggexporters.h"
#include "../ggutils.h"
#include "../ggmath.h"
#include <ostream>

namespace gg {
  //##############################################################################################
  template <typename _Tp, int cn> std::ostream& cvVecdump(std::ostream &os, const cv::Vec<_Tp,cn> &val) {
    for (int i=0;i<cn-1;i++)
      os <<val[i]<<",";
    os <<val[cn-1];
    return os;
  }
  //##############################################################################################

  typedef float PCA_T;

  class shapetrackfilter {
  public:
    shapetrackfilter(void) {}
    virtual void operator() (shapetrackproperties &stp) {
      origrois=stp.getshapes();
      imgdata=stp.getimgdata().getimgs();
      imgsize=imgdata[0].size();

      std::cout <<"rankthenboxshapes"<<std::endl;
      rankthenboxshapes(stp);
#ifdef ENABLE_SHAPEEXTRACTION
      std::cout <<"rankfilterboxedshapes"<<std::endl;
      rankfilterboxedshapes();
      //XXX Not complete yet! Even if this compiles etc. It isn't in the right place yet.
      //Need to perform PCA on only the consistent shapes, not the jam packed shapes that this function will get.
      std::cout <<"postprocessrankthenboxshapes"<<std::endl;
      postprocessrankthenboxshapes(stp);
//      performpca(boxedshapes,filteredshapes);
//      performpca(rankfilteredshapes,filteredshapes);
      std::cout <<"performpca"<<std::endl;
      performpca(rankfilteredshapes,boxedshapes,filteredshapes);
//      performpca_color(rankfilteredshapes,boxedshapes,imgdata,filteredshapes);
#else
      filteredshapes=boxedshapes;
#endif
      initshapes(filteredshapes,stp,unboxedshapes);
      unboxshapes(filteredshapes,unboxedshapes);

#if(1)
      imgdatamerge(stp);
#endif
    }
    const SHAPES& getorigrois(void) { return origrois; }
    const SHAPES& getboxedshapes(void) { return boxedshapes; }
    const SHAPES& getfilteredshapes(void) { return filteredshapes; }
    const SHAPES& getrankfilteredshapes(void) { return rankfilteredshapes; }
    const SHAPES& getunboxedshapes(void) { return unboxedshapes; }
    //added to enable dumping out of images with appropriate framenos!
    virtual void dump(const std::string &basename, const region::MREGv &shapes, const std::deque<int> &framenos) {
      std::cout <<"Dumping video: shapetrackfilter"<<std::endl;
      assert(shapes.size()>0);
      cv::Mat imgtowrite;
      for (size_t i=0;i<shapes.size();i++) {
        if (!shapes[i].empty()) {
          imgtowrite=cv::Mat(shapes[i].size(),CV_8UC3);
          break;
        }
      }
      for (size_t i=0;i<shapes.size();i++) {
        if (!shapes[i].empty() && shapes[i].channels()==1)
          cv::cvtColor(shapes[i],imgtowrite,CV_GRAY2BGR);
        else if (shapes[i].empty())
          imgtowrite=cv::Scalar::all(0);
        else
          shapes[i].copyTo(imgtowrite);
        region::ExportImage(imgtowrite,basename,3,framenos[i]);
      }
      std::cout <<"Finished dumping video."<<std::endl;
    }
    //dumping out of images
    virtual void dump(const std::string &basename, const region::MREGv &shapes) {
      std::cout <<"Dumping video: shapetrackfilter"<<std::endl;
      assert(shapes.size()>0);
      cv::Mat imgtowrite;
      for (size_t i=0;i<shapes.size();i++) {
        if (!shapes[i].empty()) {
          imgtowrite=cv::Mat(shapes[i].size(),CV_8UC3);
          break;
        }
      }
      for (size_t i=0;i<shapes.size();i++) {
        if (!shapes[i].empty() && shapes[i].channels()==1)
          cv::cvtColor(shapes[i],imgtowrite,CV_GRAY2BGR);
        else if (shapes[i].empty())
          imgtowrite=cv::Scalar::all(0);
        else
          shapes[i].copyTo(imgtowrite);
        region::ExportImage(imgtowrite,basename,3,i);
      }
      std::cout <<"Finished dumping video."<<std::endl;
    }

  protected:
    SHAPES boxedshapes;
    SHAPES filteredshapes;
    SHAPES rankfilteredshapes;
    SHAPES unboxedshapes;
    SHAPES origrois;
    /// bounding rectangles taken from shapetrackproperties.
    std::vector<cv::Rect> currentbounds;
    /// offset into larger bounding for all shapes.
    std::vector<cv::Rect> boxedbounds;

    cv::Size maxsize;
    cv::Size imgsize;
//"""""""""""""""""""""""""""""""""""""""""""""""""""""""""""""""""""""""""""""""""
    virtual void constraintomaxsize(cv::Vec4f &v) {
      if (v[0]<0)
        v[0]=0;
      if (v[1]<0)
        v[1]=0;

      if (v[2]<=0)
        v[2]=maxsize.width;
      if (v[3]<=0)
        v[3]=maxsize.height;

      if (v[0]+v[2]>imgsize.width)
        v[2]=imgsize.width-v[0];
      if (v[1]+v[3]>imgsize.height)
        v[3]=imgsize.height-v[1];

      if (v[2]>maxsize.width)
        v[2]=maxsize.width;
      if (v[3]>maxsize.height)
        v[3]=maxsize.height;
    }
//"""""""""""""""""""""""""""""""""""""""""""""""""""""""""""""""""""""""""""""""""
    /// postprocessrankthenboxshapes: only call if rankthenboxshapes called
    /// immediately before.
#if(1)
    virtual void postprocessrankthenboxshapes(shapetrackproperties &stp) {
      if (boxedshapes.size()<3)
        return;
      std::vector<cv::Vec4f > interpolated(boxedshapes.size());
      for (int i=0;i<boxedshapes.size();i++) {
        if (!boxedshapes[i].empty())
          interpolated[i]=cv::Vec4f(currentbounds[i].x,currentbounds[i].y,currentbounds[i].width,currentbounds[i].height);
        else
          interpolated[i]=cv::Vec4f(0.f,0.f,0.f,0.f);
      }

#if(0)
      //probably not worth doing as need to interpolate values!
      //the values from region::Rects::R are not consistent
      for (int i=0;i<boxedshapes.size();i++) {
        region::Rects::R r=stp.getshapeboundings()[i];
        if (boxedshapes[i].empty() && !region::Rects::isempty(r)) {
        }
      }
#endif

      //find first non empty shapes
      int firstnonempty_boxedshape=-1,firstnonempty_origshape=-1;
      for (int i=0;i<stp.size();i++) {
        region::Rects::R r=stp.getshapeboundings()[i];
        if (!region::Rects::isempty(r) && firstnonempty_origshape==-1)
          firstnonempty_origshape=i;
        if (!boxedshapes[i].empty() && firstnonempty_boxedshape==-1)
          firstnonempty_boxedshape=i;
        if (firstnonempty_origshape>=0 && firstnonempty_boxedshape>=0)
          break;
      }

      assert(firstnonempty_boxedshape>=0 && firstnonempty_origshape>=0);

      cv::Vec4f grad;
      for (int i=firstnonempty_boxedshape;i<stp.size()-1;i++) {
        if (boxedshapes[i].empty()) {
          std::cout <<"interpolating shape:"<<i<<"\t";
          if (i>0 && interpolated[i-1][3]>0.5) {
            if (interpolated[i+1][3]>0.5) //then can use central difference to calculate interpolated
              { grad=gg::centraldiff(interpolated[i-1],interpolated[i+1]);
              std::cout <<"CentralD:";
              cvVecdump(std::cout,interpolated[i-1]);
              std::cout <<" ";
              cvVecdump(std::cout,interpolated[i+1]);
              }
#if(1)
//            else if (i>1 && interpolated[i-2][3]>0.5)
            else if (i>1 && !boxedshapes[i-2].empty())
              { grad=gg::diff(interpolated[i-2],interpolated[i-1]);
              std::cout <<"Upwind:";
              cvVecdump(std::cout,interpolated[i-2]);
              std::cout <<" ";
              cvVecdump(std::cout,interpolated[i-1]);
              }
#endif
            else
              continue;
            interpolated[i]=gg::interpolate(interpolated[i-1],grad);
            std::cout <<"->";
            cvVecdump(std::cout,interpolated[i]);
            constraintomaxsize(interpolated[i]);
            std::cout <<"->";
            cvVecdump(std::cout,interpolated[i]);
            std::cout <<std::endl;
          }
        }
      }
      for (size_t i=0;i<stp.size();i++) {
        std::cout <<"stp shape:"<<i<<std::endl;
        region::Rects::R r=stp.getshapeboundings()[i];
        if (boxedshapes[i].empty() && interpolated[i][3]>0.5) { //this time empty only - rankthenbox copies on non empty regions
          std::cout <<"is empty"<<std::endl;
          currentbounds[i]=cv::Rect(cvRound(interpolated[i][0]),cvRound(interpolated[i][1]),cvRound(interpolated[i][2]),cvRound(interpolated[i][3]));
          cv::Mat boxeddata(maxsize,cv::DataType<SHAPE_T>::type);
          calcboxdata(stp.getshapes()[i], boxeddata, currentbounds[i], boxedbounds[i]);
          boxedshapes[i]=boxeddata;
        }
      }
    }
#endif
//"""""""""""""""""""""""""""""""""""""""""""""""""""""""""""""""""""""""""""""""""
    ///given shape and currentbounds, this calculates the boxeddata
    ///and boxedbounds
    virtual void calcboxdata(const cv::Mat &shape, cv::Mat &boxeddata, cv::Rect &currentbounds, cv::Rect &boxedbounds) {
std::cout <<"Y1"<<std::endl;
      boxeddata=cv::Scalar::all(0);
      cv::Point boxedorigin((maxsize.width-currentbounds.width)*.5,(maxsize.height-currentbounds.height)*.5);
      boxedbounds=cv::Rect(boxedorigin.x,boxedorigin.y,currentbounds.width,currentbounds.height);
      std::cout <<"Y2"<<std::endl;
      std::cout <<maxsize.width<<","<<maxsize.height<<" "<<currentbounds.x<<","<<currentbounds.y<<","<<currentbounds.width<<","<<currentbounds.height<<" "<<boxeddata.cols<<","<<boxeddata.rows<<" "<<boxedbounds.x<<","<<boxedbounds.y<<","<<boxedbounds.width<<","<<boxedbounds.height<<std::endl;
      cv::Mat subboxeddata(boxeddata,boxedbounds);
std::cout <<"Y2.i"<<std::endl;
      region::Rects::constrainrectvals(currentbounds, shape.cols, shape.rows);
      std::cout <<"Y3"<<std::endl;
      cv::Mat subcurrentimg(shape,currentbounds);
      std::cout <<"Y4"<<std::endl;
      subcurrentimg.copyTo(subboxeddata);
      std::cout <<"Y5"<<std::endl;
    }
//"""""""""""""""""""""""""""""""""""""""""""""""""""""""""""""""""""""""""""""""""
    /// rankthenboxshapes finds which shapes to calculate a suitable bounding
    /// otherwise large outlier shapes may adversely affect the positioning
    virtual void rankthenboxshapes(shapetrackproperties &stp) {
      region::MREGv thresholdshapes(stp.getshapes().size());
      std::transform(stp.getshapes().begin(),stp.getshapes().end(),thresholdshapes.begin(),binarize);
      regconsist regcheck(thresholdshapes,0.35);
      std::vector<bool> consistentshapes=regcheck.getconsistentregions();

      //now have to find upright bounding rectangles... and then max dims ... or no we don't, maybe can get from regconsist::regcheck...
      region::Rects onlyvalidrects;
      int nonemptyidx=-1;
      for (size_t i=0;i<stp.size();i++) {
        region::Rects::R r=stp.getshapeboundings()[i];
        if (!region::Rects::isempty(r) && consistentshapes[i]) {
          onlyvalidrects.addrect(r);
          if (nonemptyidx<0)
            nonemptyidx=i;
        }
      }

      maxsize=onlyvalidrects.getuprightmaxdims();

      boxedshapes.resize(stp.size());
      currentbounds.resize(stp.size());
      boxedbounds.resize(stp.size());

      for (size_t i=0;i<stp.size();i++) {
        region::Rects::R r=stp.getshapeboundings()[i];
        if (!region::Rects::isempty(r) && consistentshapes[i]) {
          currentbounds[i]=r.boundingRect();
          cv::Mat boxeddata(maxsize,cv::DataType<SHAPE_T>::type);
          calcboxdata(stp.getshapes()[i], boxeddata, currentbounds[i], boxedbounds[i]);
          boxedshapes[i]=boxeddata;
        }
        else {
          boxedshapes[i]=cv::Mat();
          currentbounds[i]=cv::Rect(-1,-1,-1,-1);
          boxedbounds[i]=cv::Rect(-1,-1,-1,-1);
        }
      }
    }
//"""""""""""""""""""""""""""""""""""""""""""""""""""""""""""""""""""""""""""""""""
    virtual void boxshapes(shapetrackproperties &stp) {
      maxsize=stp.getrotatedrects().getuprightmaxdims();
      int nonemptyidx=stp.getimgdata().getfirstnonemptyidx();

      boxedshapes.resize(stp.size());
      currentbounds.resize(stp.size());
      boxedbounds.resize(stp.size());
      for (size_t i=0;i<stp.size();i++) {
        region::Rects::R r=stp.getshapeboundings()[i];
        if (!region::Rects::isempty(r)) {

          cv::Mat boxeddata(maxsize,cv::DataType<SHAPE_T>::type);
          boxeddata=cv::Scalar::all(0);
          currentbounds[i]=r.boundingRect();

          assert(maxsize.width>=currentbounds[i].width && maxsize.height>=currentbounds[i].height);

          cv::Point boxedorigin(((maxsize.width-currentbounds[i].width)*.5),((maxsize.height-currentbounds[i].height)*.5));
          boxedbounds[i]=cv::Rect(boxedorigin.x,boxedorigin.y,currentbounds[i].width,currentbounds[i].height);

          cv::Mat subboxeddata(boxeddata,boxedbounds[i]);
          region::Rects::constrainrectvals(currentbounds[i], stp.getshapes()[i].cols, stp.getshapes()[i].rows);
          cv::Mat subcurrentimg(stp.getshapes()[i],currentbounds[i]);
          subcurrentimg.copyTo(subboxeddata);
          boxedshapes[i]=boxeddata;
        }
        else {
          boxedshapes[i]=cv::Mat();
          currentbounds[i]=cv::Rect(-1,-1,-1,-1);
          boxedbounds[i]=cv::Rect(-1,-1,-1,-1);
        }
      }
    }
 //"""""""""""""""""""""""""""""""""""""""""""""""""""""""""""""""""""""""""""""""""
    virtual void unboxshapes(const SHAPES &_boxedshapes, SHAPES &_unboxedshapes) {
      assert(_boxedshapes.size()>0);
      assert(_boxedshapes.size()==currentbounds.size());
      assert(_boxedshapes.size()==_unboxedshapes.size());
      for (size_t i=0;i<_boxedshapes.size();i++) {
        assert(_boxedshapes[i].empty()==_unboxedshapes[i].empty());
        if (!_boxedshapes[i].empty()) {
          cv::Mat subunboxedshape(_unboxedshapes[i],currentbounds[i]);
          cv::Mat subboxedshape(_boxedshapes[i],boxedbounds[i]);
          subboxedshape.copyTo(subunboxedshape);
        }
      }
    }
    void initshapes(const SHAPES &existingshapes, const shapetrackproperties &stp, SHAPES &newoutboundshapes) {
      newoutboundshapes.resize(existingshapes.size());
      size_t X,Y,N;
      int type=cvutils::getdims(stp.getshapes(),X,Y,N);
      for (size_t i=0;i<existingshapes.size();i++) {
        if (!existingshapes[i].empty()) {
          newoutboundshapes[i]=cv::Mat(cv::Size(X,Y),type,cv::Scalar::all(0));
        }
      }
    }
    virtual void rankfilterboxedshapes(void) {
      region::MREGv thresholdshapes(boxedshapes.size());
      std::transform(boxedshapes.begin(),boxedshapes.end(),thresholdshapes.begin(),binarize);
      regconsist regcheck(thresholdshapes);
      std::vector<bool> consistentshapes=regcheck.getconsistentregions();

      rankfilteredshapes.resize(boxedshapes.size());
      for (size_t i=0;i<boxedshapes.size();i++) {
        if (consistentshapes[i])
          rankfilteredshapes[i]=boxedshapes[i];
        else
          rankfilteredshapes[i]=cv::Mat();
      }
    }
    virtual void performpca (const SHAPES &_boxedshapes, SHAPES &_filteredshapes) {
      region::MREGv boxedshapesT,filteredshapesT;
      cvutils::convertvectT<PCA_T>(_boxedshapes,boxedshapesT);
      cv::Mat_<PCA_T> coldata=cvutils::imgs2columndata<PCA_T>(boxedshapesT);
      cv::Mat_<PCA_T> compressed,reconstructed(coldata.rows,coldata.cols);
      gg::robcompressPCA(coldata,10,coldata,compressed,reconstructed);
      size_t X,Y,N;
      cvutils::getdims(_boxedshapes,X,Y,N);
      cvutils::columndata2imgs(reconstructed,boxedshapesT,Y);
      cvutils::convertvectT<uchar>(boxedshapesT,_filteredshapes);
      cvutils::EMPTYIMGS emptyimgs=cvutils::getemptyimgs(_boxedshapes);
      cvutils::insertemptyimgs(emptyimgs,_filteredshapes);
    }
    virtual void performpca (const SHAPES &_rankedshapes, const SHAPES &_allshapes, SHAPES &_filteredshapes) {
      region::MREGv rankedshapesT,allshapesT,filteredshapesT;

      cvutils::convertvectT<PCA_T>(_rankedshapes,rankedshapesT);
      cvutils::convertvectT<PCA_T>(_allshapes,allshapesT);

      cv::Mat_<PCA_T> colrankeddata=cvutils::imgs2columndata<PCA_T>(rankedshapesT);
      cv::Mat_<PCA_T> colalldata=cvutils::imgs2columndata<PCA_T>(allshapesT);

      cv::Mat_<PCA_T> compressed,reconstructed(colalldata.rows,colalldata.cols);

      gg::robcompressPCA(colrankeddata,10,colalldata,compressed,reconstructed);
//      gg::compressPCA(colrankeddata,10,colalldata,compressed,reconstructed);

      size_t X,Y,N;

      cvutils::getdims(_allshapes,X,Y,N);
      cvutils::columndata2imgs(reconstructed,allshapesT,Y);
      cvutils::convertvectT<uchar>(allshapesT,_filteredshapes);

      cvutils::EMPTYIMGS emptyimgs=cvutils::getemptyimgs(_allshapes);
      cvutils::insertemptyimgs(emptyimgs,_filteredshapes);
    }
    virtual void performpca_color(const SHAPES &_rankedshapes, const SHAPES &_allshapes, const region::MREGv &_imgdata, SHAPES &_filteredshapes) {
      region::MREGv rankedshapesT,allshapesT,filteredshapesT,imgdataT;

      std::cout <<"convertvectT"<<std::endl;
      cvutils::convertvectT<PCA_T>(_rankedshapes,rankedshapesT);
      cvutils::convertvectT<PCA_T>(_allshapes,allshapesT);
      cvutils::convertvectT<PCA_T>(_imgdata,imgdataT);

      std::cout <<"imgs2columndata"<<std::endl;
      cv::Mat_<PCA_T> colrankeddata=cvutils::imgs2columndata<PCA_T>(rankedshapesT,imgdataT);
      cv::Mat_<PCA_T> colalldata=cvutils::imgs2columndata<PCA_T>(allshapesT,imgdataT);

      cv::Mat_<PCA_T> compressed,reconstructed(colalldata.rows,colalldata.cols);

      std::cout <<"compressPCA"<<std::endl;
#if(0)
      gg::compressPCA(colrankeddata,10,colalldata,compressed,reconstructed);
#else
      reconstructed=cv::Mat_<PCA_T>::ones(colalldata.rows,colalldata.cols)*255;
#endif
      size_t X,Y,N;
      int nchannels;

      std::cout <<"getdims"<<std::endl;
      cvutils::getdims(_imgdata,X,Y,N,nchannels);
      reconstructed=colalldata;

      std::cout <<"columndata2imgs"<<std::endl;
      cvutils::columndata2imgs<PCA_T>(reconstructed,allshapesT,Y,nchannels);

      for (size_t i=0;i<allshapesT.size();i++)
        allshapesT[i]=cv::Scalar::all(255);

      std::cout <<"convertvectT"<<std::endl;
      cvutils::convertvectT<uchar>(allshapesT,_filteredshapes,nchannels);
#if(0)
      _filteredshapes=_allshapes;
      int nempty=0;
      for (size_t i=0;i<_filteredshapes.size();i++) {
        if (_filteredshapes[i].empty())
          nempty++;
        else
          _filteredshapes[i]=cv::Scalar::all(255);
      }
      std::cout <<"no empty:"<<nempty<<"/"<<_filteredshapes.size()<<std::endl;
#endif
#if(1)
      std::cout <<"insertemptyimgs"<<std::endl;
      cvutils::EMPTYIMGS emptyimgs=cvutils::getemptyimgs(_allshapes);
      cvutils::insertemptyimgs(emptyimgs,_filteredshapes);
#endif
    }
    void imgdatamerge(shapetrackproperties &stp) {
      std::transform(unboxedshapes.begin(),unboxedshapes.end(),unboxedshapes.begin(),gg::imgbinarize(128,255));
      std::transform(unboxedshapes.begin(),unboxedshapes.end(),unboxedshapes.begin(),gg::simpleedge());
      std::transform(unboxedshapes.begin(),unboxedshapes.end(),unboxedshapes.begin(),gg::cvtColorFunctor(CV_GRAY2BGR));
      std::transform(unboxedshapes.begin(),unboxedshapes.end(),unboxedshapes.begin(),
          gg::changeColorFunctor<cv::Vec3b>(cv::Vec3b(225,220,45),cv::Vec3b(0,0,0)));
      std::transform(unboxedshapes.begin(),unboxedshapes.end(),
                     imgdata.begin(),unboxedshapes.begin(),
                     gg::BinaryPtrFunctorIO(cv::bitwise_or));
    }
    region::MREGv imgdata;
  };
}

#endif /* GGSHAPETRACKFILTER_H_ */
