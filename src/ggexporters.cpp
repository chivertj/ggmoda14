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
 * ggexporters.cpp
 *
 *  Created on: Mar 25, 2012
 *      Author: jpc
 */

#include "ggtimewindow.h"
#include "ggregiontrack.h"
#include "ACDefinitions.h"
#include "../mainhelperfuncs.h"
#include <iostream>
#include <fstream>

namespace gg {
  std::ostream& operator<<(std::ostream &os, const cv::RotatedRect &r) {
    os <<r.angle<<","<<r.center.x<<","<<r.center.y<<","<<r.size.height<<","<<r.size.width;
    return os;
  }
  std::ostream& operator<<(std::ostream &os, const cv::Rect &r) {
    os <<"("<<r.x<<","<<r.y<<"),["<<r.width<<","<<r.height<<"]";
    return os;
  }
  //@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@
  //@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@
#if(0)
  void dumptrackdata(const gg::timewindow &trackerdata) {
    //get extracted tracks
    const gg::regiontracker::OBJTRACKS_T &goodtracks=trackerdata.getgoodtracks();
    std::cout <<"No of good tracks:"<<goodtracks.size()<<std::endl;
    for (size_t i=0;i<goodtracks.size();i++) {
      const gg::FRAMENOS &framenos=goodtracks[i].getframenos();
      std::cout <<"Track "<<i<<" has size "<<framenos.size()<<std::endl;
      for (size_t j=0;j<framenos.size();j++)
        std::cout <<framenos[j]<<",";
      std::cout <<std::endl;
    }
  }
  //@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@
  //@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@
  void exportforegrounddata(gg::timewindow &trackerdata) {
    //merge image data with foreground data
    trackerdata.makeimgtracks();
    const gg::imgtracks& imgtracks=trackerdata.getimgtracks();
    cv::Mat combinedimg;
    std::string sequencefilename,basesequencefilename("seq"); //("myvideo.avi");
    for (size_t i=0;i<imgtracks.size();i++) {
      const gg::imgsequencedata &imgsequence=imgtracks[i].getimgdata();
      const gg::REGIONDATA& regiondata=imgtracks[i].getregiondata(); //std::deque<frameregion>
      const gg::KalmanMemory km=imgtracks[i].getkalmanmem();
      const gg::imgsequencedata::IMGDATA &imgdata=imgsequence.getimgs(); //std::vector<cv::Mat>
      //then combine regiondata[j] and imgdata[j]
      if (regiondata.size()!=imgdata.size())
        throw gg::error("region data size not match img data size.");
      //      cv::VideoWriter videoout(sequencefilename,CV_FOURCC('x','v','i','d'),25,regiondata[0].binarytemp().size(),true);
      sequencefilename=basesequencefilename+ACDefinitions::StringAndZeroPad(i,3)+".avi";
      cv::VideoWriter subvideoout(sequencefilename,CV_FOURCC('x','v','i','d'),25,regiondata[0].binarytemp().size(),true);
      for (size_t j=0;j<regiondata.size();j++) {
        combinedimg=cv::Scalar(0);
        if (regiondata[j].isvalid())
          cv::bitwise_and(imgdata[j],imgdata[j],combinedimg,regiondata[j].binarytemp());
        const cv::RotatedRect &r=regiondata[j].getregprops().getboundbox();
        IplImage i_combinedimg=IplImage(combinedimg);
        mainhelpers::DrawBox(r,cv::Scalar(0,255,0),&i_combinedimg);
        cv::Mat_<float> predictm=cv::Mat(km[j]->state_pre);
        cv::RotatedRect predr(cv::Point2f(predictm[0][0],predictm[1][0]),cv::Size(predictm[2][0],predictm[3][0]),predictm[4][0]);
        if (regiondata[j].isvalid())
          mainhelpers::DrawBox(predr,cv::Scalar(255,0,0),&i_combinedimg);
        cv::Mat_<float> corrm=cv::Mat(km[j]->state_post);
        cv::RotatedRect corrr(cv::Point2f(corrm[0][0],corrm[1][0]),cv::Size(corrm[2][0],corrm[3][0]),corrm[4][0]);
        mainhelpers::DrawBox(corrr,cv::Scalar(0,255,255),&i_combinedimg);

        std::cout <<"REG:"<<i;
        std::cout <<":"<<regiondata[j].uid_hash();
        std::cout <<"|"<<corrr<<"|"<<predr<<"|";
        std::cout <<r<<" "<<j<<"/"<<regiondata.size()<<std::endl;

        //regiondata[j].uid_hash();
        mainhelpers::LabelRegion(combinedimg,regiondata[j],cv::Scalar(255,128,255));

        subvideoout << combinedimg;
      }
    }
  }
#endif
  //@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@
  //@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@

}
