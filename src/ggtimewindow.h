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
#ifndef GGTIMEWINDOW
#define GGTIMEWINDOW

#include "globalCVHeader.h"

#include "ggutils.h"
#include "ggtracker.h"
#include "ggsimcorr.h"
#include "ggregiontrack.h"
#include "ggregiontracker.h"
#include "ggframe.h"

namespace gg {
class timewindow {
protected:
  timewindow(void);
  timewindow(const timewindow &);
  void operator= (const timewindow &);
  /// circular buffer
  framesequence frames;
  size_t idx,pidx;
  const size_t NFRAMES;
  bool init;
  std::vector<regioncorrespondences> corrs;
  std::vector<regsims> sims;
  size_t timeval;
  /// Probably same value as gg::timewindow::timeval\n
  /// but could be different in future versions
  size_t framecntr;
  regiontracker regtracker;
  /// copy of image data held in gg::timewindow::imgdata and circular buffer gg::timewindow::frames
  imgsequencedata imgdata;
  imgtracks imgtrackdata;
public:
  timewindow(const frameproperties &props)
: idx(1),pidx(0), NFRAMES(_WINDOWSIZE), frames(_WINDOWSIZE,frame(props)), init(false), corrs(_WINDOWSIZE),sims(_WINDOWSIZE),timeval(1), regtracker(props), framecntr(1),imgtrackdata(0) {}
  virtual ~timewindow(void) {
    const imgsequencedata::IMGDATA &d=imgdata.getimgs();
    if (d.size()>0) {
      cv::VideoWriter videoout;
      videoout.open(std::string("timewindowimgseq.avi"),CV_FOURCC('x','v','i','d'),25,d[0].size(),true);
      for (size_t i=0;i<d.size();i++) {
        videoout << d[i];
      }
    }
  }
  void addframe(frame &f) {
    if (init) {
      timeval++;
      framecntr++;
      pidx=idx;
      idx=inc(NFRAMES,idx);

      frames[idx].set(f);

      regtracker.predict();

      frames[idx].mergeregions(regtracker.prediction());

      sims[idx](frames[idx],frames[pidx]);
      corrs[idx](sims[idx],frames[idx],frames[pidx]);
      corrs[idx].setstates(frames[idx]);

      //  imgdata.add(f.getimgdata(),timeval);
      frames[idx].settime(timeval);

      regtracker.correct(frames[idx],corrs[idx]);
    }
    else {
      init=true;
      frames[idx].set(f);

      //  imgdata.add(f.getimgdata(),timeval);
      frames[idx].settime(timeval);
    }
    //      region::ExportImage(f.getimgdata(),"timwin",3,framecntr);
    std::cout <<"ggtimewindow timeval:"<<timeval<<std::endl;
  }
  void finished(void) {
    regtracker.finalise();
  }
  const regiontracker::OBJTRACKS_T& getgoodtracks(void) const {
    return regtracker.getgoodtracks();
  }
  const regiontracker getregiontracker(void) const {
    return regtracker;
  }
  const regiontracker::OBJTRACKS_T& getcurrenttracks(void) const {
    return regtracker.getcurrenttracks();
  }
  const frame& getcurrentframe(void) const { return frames[idx]; }
  void makeimgtracks(void) {
    imgtrackdata(regtracker.getgoodtracks());
    //      imgtrackdata(regtracker.getgoodtracks(),imgdata);
  }
  const imgtracks& getimgtracks(void) const { return imgtrackdata; }
  const imgsequencedata& getimgdata(void) const { return imgdata; }
  regiontracker::OBJTRACKS_T& getfinishedtracks(void) { return regtracker.getfinishedtracks(); }
  const regiontracker::OBJTRACKS_T& getfinishedtracks(void) const { return regtracker.getfinishedtracks(); }
  void clearfinishedtracks(void) { regtracker.clearfinishedtracks(); }
};
//#########################################
//#########################################
}

#endif //GGTIMEWINDOW

