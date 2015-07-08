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
 * ggregconsist.h
 *
 *  Created on: Apr 4, 2012
 *      Author: jpc
 */

#ifndef GGREGCONSIST_H_
#define GGREGCONSIST_H_

#include "../region.h"
#include <vector>
#include "../ggidxatt.h"
#include "../ggsettings.h"

namespace gg {
  //#############
  cv::Mat binarize(const cv::Mat &m);
  cv::Mat edge(const cv::Mat &m);
  class imgbinarize {
  public:
    imgbinarize(int _threshold, int _max) : threshold(_threshold),max(_max){}
    cv::Mat operator() (const cv::Mat &m) {
      std::cout <<"thresholds:"<<threshold<<","<<max<<std::endl;
      if (m.empty()) return cv::Mat();
      cv::threshold(m,res,threshold,max,cv::THRESH_BINARY);
      return res.clone();
    }
    imgbinarize(void) : threshold(128),max(255) {}
  protected:
    int threshold,max;
    cv::Mat res;
  };
  struct simpleedge {
    cv::Mat operator() (const cv::Mat &inputimg) {
      cv::threshold(inputimg,outputimg,128,255,cv::THRESH_BINARY);
      cv::dilate(outputimg,outputimg,cv::Mat(),cv::Point(-1,-1),gg::EDGETHICKNESS);
      cv::Sobel(outputimg,res01,CV_32F,0,1);
      cv::Sobel(outputimg,res10,CV_32F,1,0);
      cv::magnitude(res01,res10,res01);
      res01.convertTo(outputimg,outputimg.type());
      return outputimg.clone();
    }
    cv::Mat res01,res10;
    cv::Mat outputimg;
  };
  //#############
  int median(vector<int> &v);
  //#############
  struct imgarea {
    int operator() (const cv::Mat &r) {
      if (r.empty())
        return -1;
      return cv::countNonZero(r);
    }
  };
  //#############
  class regconsist {
  public:
    typedef std::vector<int> REGIONAREAS;
    typedef gg::idxattcollection<int,cv::Mat> REGIDXAREAS;
    regconsist(const region::MREGv &regions, float _percentilemargin=0.35) : percentilemargin(_percentilemargin) {
      operator()(regions);
    }
    regconsist(float _percentilemargin=0.35) : percentilemargin(_percentilemargin) {}
    const std::vector<bool>& operator() (const region::MREGv &regions) {
      arearegions.init(regions.size());
      areas=REGIONAREAS(regions.size());
      std::transform(regions.begin(),regions.end(),areas.begin(),imgarea());
      for (size_t i=0;i<regions.size();i++)
        arearegions[i].attribute=areas[i];
      arearegions.sort();

      int numempty=std::count(areas.begin(),areas.end(),-1);
      int numnonempty=regions.size()-numempty;
      int hfnumnonempty=cvRound(numnonempty/2.);

      int medianarea=(arearegions.v.begin()+numempty+hfnumnonempty)->attribute;

      std::pair<int,int> percentileidxs;
      percentileidxs.first=cvRound(percentilemargin*(numnonempty-1));
      percentileidxs.second=cvRound((1.-percentilemargin)*(numnonempty-1));
      std::pair<int,int> percentileareas;
      percentileareas.first=(arearegions.v.begin()+numempty+percentileidxs.first)->attribute;
      percentileareas.second=(arearegions.v.begin()+numempty+percentileidxs.second)->attribute;

      consistentregions.resize(regions.size(),false);
      for (size_t i=0;i<regions.size();i++) {
        if (areas[i]>=percentileareas.first && areas[i]<=percentileareas.second)
          consistentregions[i]=true;
      }
      return consistentregions;
    }
    const std::vector<bool>& getconsistentregions(void) { return consistentregions; }
  protected:
    REGIONAREAS areas;
    REGIDXAREAS arearegions;
    float percentilemargin;
    std::vector<bool> consistentregions;
  };
}

#endif /* GGREGCONSIST_H_ */
