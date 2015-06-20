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
 * ggshapetrackproperties.h
 *
 *  Created on: Mar 26, 2012
 *      Author: jpc
 */

#ifndef GGSHAPETRACKPROPERTIES_H_
#define GGSHAPETRACKPROPERTIES_H_

#include "../globalCVHeader.h"
#include <deque>
#include "../ggregiontrack.h"
#include "../region.h"

namespace gg {
  typedef uchar SHAPE_T;
//  typedef cv::Mat BINARYSHAPE;
//  typedef std::deque<BINARYSHAPE> SHAPES;
  typedef region::MREGv SHAPES;
  /**
   * Provides functionality to help with extracting shape from
   * gross outlines, such as the bounding box or shape defined
   * by the feature tracker points.
   *
   * This class is meant to be used only with imgtrackproperties
   * that have been built up. gg::shapetrackproperties should
   * not be added to, i.e. can't add new frame information.
   * Hence the relevant functions are privatised.
   */
  class shapetrackproperties : public imgtrackproperties {
  public:
    shapetrackproperties(const imgtrackproperties &itp) :
      imgtrackproperties::imgtrackproperties(itp) {
      if (imgdata.size()==0)
        throw gg::error("imgdata size==0 in shapetrackproperties constructor.");
    }
    virtual void clear(void) {
      imgtrackproperties::clear();
      shapes.clear();
    }
    void setshapes(SHAPES &_shapes) {
      shapes=_shapes;
      calcboundings();
    }
    const SHAPES& getshapes(void) const {
      return shapes;
    }
    virtual void dump(std::ostream &os) const {
      objecttrackproperties::dump(os);
    }
    virtual void dump(cv::VideoWriter &os) {
      std::cout <<"Dumping video: shapetrack"<<std::endl;
      assert(shapes.size()==imgdata.size());
      cv::Mat opwriteframe;
      for (size_t i=0;i<regiondata.size();i++) {
        if (regiondata[i].isvalid()) { //should be on i=0!
          opwriteframe=imgdata[i].clone();
          break;
        }
      }
      for (size_t i=0;i<regiondata.size();i++) {
        opwriteframe=cv::Scalar::all(0);
        if (regiondata[i].isvalid())
          cv::bitwise_and(imgdata[i],imgdata[i],opwriteframe,shapes[i]);
        os << opwriteframe;
      }
    }
    virtual void reverse(void) {
      imgtrackproperties::reverse();
      std::reverse(shapes.begin(),shapes.end());
    }
    region::Rects::RECTS& getshapeboundings(void) { return rotatedrects.getrects(); }
    const region::Rects::RECTS& getshapeboundings(void) const { return rotatedrects.getrects(); }
    region::Rects& getrotatedrects(void) { return rotatedrects; }
    const region::Rects& getrotatedrects(void) const { return rotatedrects; }
  protected:
    SHAPES shapes;
    region::REGIONS pntregions;
    region::Rects rotatedrects;
    void calcboundings(void) {
      std::cout <<"TRACK:"<<uid_hash()<<std::endl;
      std::cout <<"calcboundings #pxls:";
      pntregions.resize(shapes.size());
      for (size_t i=0;i<shapes.size();i++) {
        if (!shapes[i].empty())
          region::ExtractPnts(shapes[i],pntregions[i],true);
        else
          pntregions[i].clear();
        std::cout <<i<<":"<<pntregions[i].size()<<",";
      }
      std::cout <<std::endl;
      std::cout <<"rotatedRects dims:";
      rotatedrects.makeRects(pntregions);
      for (size_t i=0; i<shapes.size(); i++)
        std::cout <<i<<":"<<rotatedrects.getrects()[i].size.width<<","<<rotatedrects.getrects()[i].size.height<<" ";
      std::cout <<std::endl;
    }
    shapetrackproperties(void) {}

    ///privatise (disallow) these functions because this class can
    ///only be created from an existing well defined imgstrackproperties
    virtual const imgtrackproperties& operator=(const imgtrackproperties &d) { return *this; }
    const imgtrackproperties& operator=(const objecttrackproperties &otp) { return *this; }
    virtual void initkalman(const cv::Mat_<float> &init, size_t frameno, const frameregion &fr, const cv::Mat &img) {}
    virtual void correct(const cv::Mat &m, size_t frameno, frameregion &fr, const cv::Mat &img) {}
    virtual void correct(const cv::Mat &m, size_t frameno, const frameregion &fr, const cv::Mat &img) {}
    void setdata(const objecttrackproperties &otp, const imgsequencedata &isd) {}
    void setimgdata(const imgsequencedata &isd) {}
  };
}

#endif /* GGSHAPETRACKPROPERTIES_H_ */
