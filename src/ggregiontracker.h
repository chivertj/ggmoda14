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
#ifndef GGREGIONTRACKER
#define GGREGIONTRACKER

#include "ggtracker.h"
#include "ggsimcorr.h"
#include "ggregiontrack.h"
#include "ggpropagating.h"
#include "ggtracks.h"
#include "ggframe.h"
#include "ggdefs.h"

namespace gg {
/**
 * tracks binary regions only \n
 * gg::frame data not stored (past 1 time history with gg::regiontracker::cframe and gg::regiontracker::pframe) \n
 * gg::frame data stored by container of gg::regiontracker \n
 * which is gg::timewindow::frames\n
 */
class regiontracker {
public:
  //    typedef objecttrackproperties TRACK_T;
  typedef imgtrackproperties TRACK_T;
  typedef objtracks<TRACK_T> OBJTRACKS_T;
protected:
  frame pframe;
  frame cframe;
  OBJTRACKS_T ctrackers,ptrackers,goodtracks,finishedtracks;
  trackpropagateconstraints constraints;
  bool finalised;
  ROIS predictedROIs;
public:
  regiontracker(const frameproperties &props) :
    pframe(props), cframe(props), ctrackers(_MAXNREGIONS),ptrackers(_MAXNREGIONS),goodtracks(0),finishedtracks(0), finalised(false) {
    cv::Mat_<float> minconstraints(0,0),maxconstraints(2,1);
    maxconstraints(0,0)=props.X;
    maxconstraints(1,0)=props.Y;
    constraints(minconstraints,maxconstraints,0,0);
    predictedROIs.reserve(_MAXNREGIONS);
  }
  void predict(void) {
    for (size_t i=0;i<cframe.size();i++)
      ctrackers[i].predict();
  }
  const ROIS& prediction(void) {
    predictedROIs.resize(cframe.size());
    for (size_t i=0;i<cframe.size();i++) {
      const cv::Mat_<float> predm=ctrackers[i].getstatepre();
      predictedROIs[i]=ROI(cv::Point2f(predm[0][0],predm[1][0]),cv::Size2f(predm[2][0],predm[3][0]),predm[4][0]);
    }
    return predictedROIs;
  }
  void correct(frame &_cframe, const regioncorrespondences &corrs) {
    if (finalised)
      throw gg::error("ggregiontrackerL44");
    pframe=cframe;
    cframe=_cframe;
    ptrackers=ctrackers;
    int previdx;
    std::cout <<"***Active track count:"<<cframe.size()<<" ***"<<std::endl;
    for (size_t i=0;i<cframe.size();i++) {
      const cv::RotatedRect &rr=cframe[i].getregprops().getboundbox();
      cv::Mat_<float> m=region::Rects::getMatrixVals(cframe[i].getregprops().getboundbox());
      previdx=corrs.getcurr2prev()[i];
      if (previdx!=-1) { //XXX this is where measurements are logged
        ptrackers[previdx].correct(m,cframe.gettime(),cframe[i],cframe.getimgdata());
        ctrackers[i]=ptrackers[previdx];
      }
      else
        ctrackers[i].initkalman(m,cframe.gettime(),cframe[i],cframe.getimgdata());
      cframe[i].correctboundingbox(ctrackers[i].getmostrecent().getregprops().getboundbox());
      _cframe[i].correctboundingbox(ctrackers[i].getmostrecent().getregprops().getboundbox());
      std::cout <<"Track "<<i<<" length "<<ctrackers[i].count()<<" frameno:"<<ctrackers[i].getmaxframeno()<<std::endl;
    }
    for (size_t i=0;i<pframe.size();i++) {
      int curridx=corrs.getprev2curr()[i];
      if (curridx==-1) {
        if (ptrackers[i].count()>MINSEQLENGTH) {
#if(0)
          goodtracks.push_back_merge(constraints,ptrackers[i]);
          goodtracks.remove_before(constraints,ctrackers.earliestframeno(),finishedtracks);
#else
          goodtracks.push_back(ptrackers[i]);
          goodtracks.remove_before(constraints,ctrackers.earliestframeno(),finishedtracks);
#endif
        }
      }
    }
    //      std::cout <<"FINISHED CORRECT in gg::regiontracker"<<std::endl;
  }
  /// Only call this function when no more frames are to be added.
  void finalise(void) {
    for (size_t i=0;i<cframe.size();i++) {
      if (ctrackers[i].count()>MINSEQLENGTH)
        goodtracks.push_back(ctrackers[i]);
      goodtracks.remove_merge(constraints,finishedtracks);
    }
    finalised=true;
    std::cout <<"NO OF GOOD TRACKS AFTER FINALISE:"<<goodtracks.size()<<std::endl;
  }
  const OBJTRACKS_T& getgoodtracks(void) const { return goodtracks; }
  OBJTRACKS_T& getgoodtracks(void) { return goodtracks; }
  OBJTRACKS_T& getfinishedtracks(void) { return finishedtracks; }
  const OBJTRACKS_T& getfinishedtracks(void) const { return finishedtracks; }
  void clearfinishedtracks(void) { finishedtracks.clear(); }
  const OBJTRACKS_T& getcurrenttracks(void) const {return ctrackers;}
};
}

#endif //GGREGIONTRACKER

