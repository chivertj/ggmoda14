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
 * ggshapeextractor.h
 *
 *  Created on: Mar 26, 2012
 *      Author: jpc
 */

#ifndef GGSHAPEEXTRACTOR_H_
#define GGSHAPEEXTRACTOR_H_

#include "../ggregiontrack.h"
#include "../ggutils.h"
#include "../motion/mdBackProjector.h"
#include "../ggsettings.h"
#include "ggshapetrackproperties.h"
namespace gg {
  class shapetrackextractor {
  public:
    shapetrackextractor(void) {
    }
#ifdef BOUNDINGBOX_SHAPEBOOSTING
    /// Use bounding box around shape of isolated extracted feature points
    /// but re-positioned using the motion estimation...
    void operator () (shapetrackproperties &track) {
#ifdef ENABLE_SHAPEEXTRACTION // removed to try to remove shape extraction process
      const REGIONDATA &regiondata=track.getregiondata();
      assert(regiondata[0].isvalid() && !track.img(0).empty());

      md::BackProjector bp(cv::Rect(0,0,regiondata[0].binarytemp().cols,regiondata[0].binarytemp().rows));
      cv::Mat binaryimg=regiondata[0].binarytemp().clone();
      shapes.resize(regiondata.size());

      for (size_t i=0; i<regiondata.size();i++) {
        if (regiondata[i].isvalid()) {
          binaryimg=cv::Scalar::all(0);
          region::Rects::drawrect(regiondata[i].getregprops().getboundbox(),binaryimg);
          shapes[i]=extractshape(track.img(i),binaryimg,bp);
        }
      }
      track.setshapes(shapes);
#else
      const REGIONDATA &regiondata=track.getregiondata();
      assert(regiondata[0].isvalid() && !track.img(0).empty());

      md::BackProjector bp(cv::Rect(0,0,regiondata[0].binarytemp().cols,regiondata[0].binarytemp().rows));
      cv::Mat binaryimg=regiondata[0].binarytemp().clone();
      shapes.resize(regiondata.size());

      for (size_t i=0; i<regiondata.size();i++) {
        if (regiondata[i].isvalid()) {
          binaryimg=cv::Scalar::all(0);
          region::Rects::drawrect(regiondata[i].getregprops().getboundbox(),binaryimg);
          shapes[i]=binaryimg.clone();
        }
      }
      track.setshapes(shapes);
#endif
    }
#endif //BOUNDINGBOX_SHAPEBOOSTING

#ifdef ALPHAHULL_SHAPEBOOSTING
    /// Use binarytemp - shape of isolated extracted feature points
    void operator () (shapetrackproperties &track) {
      const REGIONDATA &regiondata=track.getregiondata();
      assert(regiondata[0].isvalid() && !track.img(0).empty());
      md::BackProjector bp(cv::Rect(0,0,regiondata[0].binarytemp().cols,regiondata[0].binarytemp().rows));
      shapes.resize(regiondata.size());
      for (size_t i=0; i<regiondata.size();i++) {
        if (regiondata[i].isvalid()) {
          shapes[i]=extractshape(track.img(i),regiondata[i].binarytemp(),bp);
        }
      }
      track.setshapes(shapes);
    }
#endif //ALPHAHULL_SHAPEBOOSTING
    cv::Mat extractshape(const cv::Mat &img, const cv::Mat &binary, md::BackProjector &bp) {
      assert(binary.type()==CVT_BINT);
      cv::Mat result=binary.clone();
      result=cv::Scalar::all(0);
      bp.setimg(img);
      bp(binary);
      bp().copyTo(result,binary);
      return result;
    }
    const SHAPES& get(void) { return shapes; }
  protected:
    SHAPES shapes;
  };
}

#endif /* GGSHAPEEXTRACTOR_H_ */
