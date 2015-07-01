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
#ifndef GGREGIONPROPS
#define GGREGIONPROPS

#include "globalCVHeader.h"
#include "gglpanalysis.h"
#include "region.h"
#include "ggutils.h"
#include "ggclassification.h"
#include "ggheadtracker.h"

namespace gg {
  /**
   * Concept of feature gg::feature::pdf and \n
   * method of computation gg::LPAnalysis
   */
  class feature {
  public:
    feature(void) {}
    feature(const cv::Mat &img, const cv::Mat &mask) {
      (*this)(img,mask);
    }
    feature(const feature &_f) {
//      pdf=_f.pdf;
      _f.pdf.copyTo(pdf);
    }
    void operator()(const cv::Mat &img, const cv::Mat &mask) {
      computer(mask,img);
      computer.AllPDFs().copyTo(pdf);
    }
    const feature& operator=(const feature&f) {
      set(f); return *this;
    }
    void set(const feature &f) {
      f.pdf.copyTo(pdf);
    }
    T_FEAT compare(const feature &f) const {
      assert(!pdf.empty());
      assert(!f.pdf.empty());
      T_FEAT val=0; 
      const T_PDF *rawpdf=pdf.ptr<T_PDF>(0);
      const T_PDF *rawfpdf=f.pdf.ptr<T_PDF>(0); 
      for (size_t i=0;i<pdf.cols;i++) 
	val+=pow(rawpdf[i]-rawfpdf[i],2.);
      val/=float(pdf.cols);
      val=sqrt(val);
      return val;
    }
    const cv::Mat& getpdf(void) const {return pdf;}
    /// combine two pdfs into self
    /// weight*self->pdf[0]+(1-weight)*notself->pdf[0]
    void combine(const feature &_f, float weight) {
      if (_f.pdf.empty()) return;
      if (pdf.empty()) {
        _f.pdf.copyTo(pdf);
      }
      else {
        assert(pdf.size()==_f.pdf.size());
        T_PDF *rawpdf=pdf.ptr<T_PDF>(0);
        const T_PDF *rawfpdf=_f.pdf.ptr<T_PDF>(0);
        for (size_t i=0;i<pdf.cols;i++)
          rawpdf[i]=weight*rawpdf[i]+(1.-weight)*rawfpdf[i];
      }
    }
  protected:
    cv::Mat pdf;
    LPAnalysis computer;
  };
  //############################################
  /**
   * Encapsulates feature and region geometric properties
   */
  class regionproperties {
  public:
    regionproperties(void) : cog(cv::Point(-1,-1)),size(0), classval(discreteclassification::NOCLASSIFICATION) { }
    regionproperties(const regionproperties &rps) : classval(discreteclassification::NOCLASSIFICATION) { set(rps);}
    regionproperties(const cv::Mat &img, const cv::Mat &mask, const cv::Mat &_regpnts) : classval(discreteclassification::NOCLASSIFICATION) {
      set(img,mask,_regpnts);
    }
    void set(const cv::Mat &img, const cv::Mat &mask, const cv::Mat &_regpnts) {
      photo(img,mask);
      cog=region::COG2D<T_BINT>(mask);
      size=cv::countNonZero(mask);
      _regpnts.copyTo(regpnts);
      r=region::Rects::makeRotatedRect(regpnts);
    }
    virtual ~regionproperties(void) { }
    void set(const regionproperties &rps) {
      photo.set(rps.photo);
      cog=rps.cog; 
      size=rps.size;
      rps.regpnts.copyTo(regpnts);
      r=rps.r;
      classval=rps.classval;
    }
    const regionproperties& operator=(const regionproperties &rps) {set(rps); return *this;}
    const feature& getphoto(void) const {return photo;}
    const feature& getphoto(void) {return photo;}
    const cv::Point& getcog(void) const {return cog;}
    size_t getsize(void) const {return size;}
    const cv::Mat& getpnts(void) const {return regpnts;}
    const cv::RotatedRect& getboundbox(void) const {return r;}
    cv::RotatedRect& getboundbox(void) {return r;}
    void setboundbox(const cv::RotatedRect &_r) {
      r.size=_r.size;
      r.center=_r.center;
      r.angle=_r.angle;
    }
    void merge(const regionproperties &rp) {
      float prop=float(size)/float(size+rp.size);
      region::MeanWeightedPoint(rp.cog,prop,cog);
      //template <class T> void MergePointSets(const cv::Mat_<T> &a, cv::Mat_<T> &b) {
      region::MergePointSets(rp.regpnts,regpnts);
      r=region::mergeRects(r,rp.r);
      photo.combine(rp.photo,prop);
      size+=rp.size;
      if (rp.classval!=classval)
        classval=discreteclassification::NOCLASSIFICATION;
    }
    const discreteclassification& getclassification(void) const {return classval;}
    void setclassification(const discreteclassification &_classval) {classval=_classval;}
  protected:
    feature photo;
    cv::Point cog;
    size_t size;
    cv::Mat regpnts;
    cv::RotatedRect r;
    discreteclassification classval;
  };
}

#endif //GGREGIONPROPS
