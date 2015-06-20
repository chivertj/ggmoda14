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
#ifndef GGFRAME
#define GGFRAME

#include "ggtracker.h"
#include "ggutils.h"
#include "ggexceptions.h"
#include "ggdefs.h"
#include <deque>

namespace gg {
  /**
   * Frame concept. \n
   * Encapsulates via gg::frameproperties\n
   * and includes division into image regions (via vector of gg::frameregion) \n
   */
  class frame {
  public:
    typedef std::vector<frameregion> FRAMEREGIONS;
    frame (const frameproperties &_props) : 
      //      regions(_MAXNREGIONS,frameregion(_props)), 
      regions(_MAXNREGIONS),
      img(cv::Mat::zeros(_props.Y,_props.X,CVT_IMG)), 
      nvalidregions(0),
      props(_props)
    { 
      for (size_t i=0;i<regions.size();i++)
	regions[i]=frameregion(_props);
    }
    frame(const frame &f) :
        regions(_MAXNREGIONS),
        img(cv::Mat::zeros(f.props.Y,f.props.X,CVT_IMG)),
        nvalidregions(f.nvalidregions),
        props(f.props)
    {
      set(f);
    }
    frame& operator=(const frame &f) {
      set(f);
      return *this;
    }
    void set(const frame &f) {
      nvalidregions=f.nvalidregions;
      for (size_t i=0;i<nvalidregions;i++)
        regions[i].set(f[i]);
      for (size_t i=nvalidregions;i<nregions;i++)
        regions[i].unset();
      //f.img.copyTo(img);
      img=f.img.clone();
      props=f.props;
    }
    void set(const cv::Mat &_img) {
    //  _img.copyTo(img);
      img=_img.clone();
    }
    void settime(const size_t &time) { props.framet=time; }
    const size_t gettime(void) const { return props.framet; }
    const frameproperties& getproperties(void) const { return props; }
    size_t size(void) const {return nvalidregions;}
    const frameregion& operator[] (size_t idx) const { if (idx>nvalidregions) throw gg::error("ggframe.hL38"); return regions[idx]; }
    frameregion& operator[] (size_t idx) { 
      //      std::cout <<"ggframeL40:"<<idx<<","<<nvalidregions<<std::endl; 
      if (idx>nregions) 
	throw gg::error("ggframe.hL39"); 
      return regions[idx]; 
    }
    void unset(void) { for (size_t i=0;i<nvalidregions;i++) regions[i].unset(); nvalidregions=0; }
    void resize(size_t _nvalidregions) {
      //      std::cout <<"ggframe.hL48:"<<nregions<<","<<_nvalidregions<<std::endl;
      if (_nvalidregions>nregions)
	throw gg::error("ggframe.hL49");
      unset();
      nvalidregions=_nvalidregions;
    }
    //    void recountvalid(void) { nvalidregions=0; for (size_t i=0;i<nregions;i++) if (regions[i].isvalid()) nvalidregions++; else break; }
    const cv::Mat& getimgdata(void) const { return img; }
    cv::Mat& getimgdata(void) { return img; }
    void mergeregions(const ROIS &predictedrois) {
      if (nvalidregions<=0) return;
      //
      std::vector<cv::Rect> uprightpredrects(predictedrois.size());
      for (size_t i=0;i<predictedrois.size();i++)
        uprightpredrects[i]=predictedrois[i].boundingRect();
      std::vector<cv::Rect> uprightprevrecs(nvalidregions);
      for (size_t i=0;i<nvalidregions;i++)
        uprightprevrecs[i]=regions[i].getregprops().getboundbox().boundingRect();
      //iterate through predicted rois to find regions that have split
      std::deque<int> idxs(nvalidregions,-1);
      for (size_t i=0;i<nvalidregions;i++) {
        for (size_t j=0;j<predictedrois.size();j++) {
          cv::Rect intsect=uprightprevrecs[i] & uprightpredrects[j];
//          float proportion=intsect.width*intsect.height/float(uprightprevrecs[j].width*uprightprevrecs[j].height);
          /// should be normalized by the predicted region, not possibly much larger non error region.
          float proportion=intsect.width*intsect.height/float(uprightpredrects[j].width*uprightpredrects[j].height);
          std::cout <<"("<<i<<","<<j<<") intersection:"<<intsect.width<<","<<intsect.height<<"->"<<proportion<<std::endl;
          if (proportion>REGINTERSECTPROP) {
            idxs[i]=j;
            break;
          }
        }
      }
      for (size_t i=0;i<idxs.size();i++)
        std::cout <<idxs[i]<<",";
      std::cout <<std::endl;
      //find out if any (previous) region corresponds to the same predicted region
      //as another (previous) region
      //and if so merge the two.
      size_t nremoved=0;
      bool change=false;
      do {
        size_t i,j;
        std::deque<int>::iterator j_it,i_it;
        FRAMEREGIONS::iterator j_rit,i_rit;
        change=false;
        for (i=0,i_it=idxs.begin(),i_rit=regions.begin();i<idxs.size()-1;i++,++i_it,++i_rit) {
          for (j=i+1,j_it=i_it+1,j_rit=i_rit+1;j<idxs.size();j++,++j_it,++j_rit) {
            if (idxs[i]!=-1 && idxs[i]==idxs[j]) {
              regions[i].merge(regions[j]);
              change=true;
              idxs.erase(j_it);
              regions.erase(j_rit);
              nvalidregions--;
              nremoved++;
              break;
            }
          }
          if (change)
            break;
        }
      } while(change);
      if (nremoved>0)
        regions.resize(_MAXNREGIONS);
//      throw gg::error("gg::frame::mergeregions not implemented (yet)");
    }
  protected:
    frame(void);
    FRAMEREGIONS regions;
    cv::Mat img;
    static const size_t nregions=_MAXNREGIONS;
    size_t nvalidregions;
    frameproperties props;
  };
  //############################################
  //############################################
  /**
   *  Container for gg::frame.
   */
  class framesequence {
  public:
    framesequence(size_t size, const frame &f) :
      frames(size,f.getproperties()),nullframe(frameproperties(0,0,0))
    {}
    //    framesequence(size_t size) :
    //      frames(size),nullframe(0,0,0)
    //    {}
    frame& operator[] (size_t i) {
      if (i<frames.size())
	return frames[i];
      throw gg::error("ggframe.hL68");
      return nullframe;
    }
    const frame& operator[] (size_t i) const {
      if (i<frames.size())
	return frames[i];
      throw gg::error("ggframe.hL74");
      return nullframe;
    }
  protected:
    framesequence(void);
    framesequence(const framesequence&);
    void operator= (const framesequence&);
    std::vector<frame> frames;
    frame nullframe;
  };
  //##############################################
  //##############################################
  /**
   *  container for image sequence data
   */
  class imgsequencedata {
  public:
    imgsequencedata(void) : reversed(false) {}
    virtual ~imgsequencedata(void) { }
    void add(const cv::Mat &img) {
      imgdata.push_back(img);
      if (times.size()>0) {
    	  int timessize=times.size();
    	  times.push_back(times[timessize-1]+1);
      }
      else times.push_back(0);
    }
    void add(const cv::Mat &img, int d) {
      imgdata.push_back(img);
      times.push_back(d);
    }
    typedef region::MREGv IMGDATA;
    typedef std::vector<int> SEQUENCETIMES;
    const IMGDATA& getimgs(void) const { return imgdata; }
    IMGDATA& getimgs(void) { return imgdata; }
    const cv::Mat& operator[](size_t i) {
      assert(i<imgdata.size());
      return imgdata[i];
    }
    const SEQUENCETIMES& gettimes(void) const { return times; }
    void clear(void) {
      if (imgdata.size()>0) {
        for (size_t i=0;i<imgdata.size();i++) {
          if (!imgdata[i].empty()) {
            int *d=imgdata[i].ptr<int>(0);
          }
        }
      }
      imgdata.clear();
      times.clear();
    }
    void reverse(void) {
      std::reverse(imgdata.begin(),imgdata.end());
      std::reverse(times.begin(),times.end());
      reversed=!reversed;
    }
    bool isreversed(void) const { return reversed; }
    size_t size(void) const { assert(imgdata.size()==times.size()); return imgdata.size(); }
    //    size_t operator[](size_t idx) const { if (idx>=size()) throw gg::error; return times[idx]; }
    void add(const imgsequencedata &isd, size_t idx) {
      imgdata.push_back(isd.imgdata[idx]);
      times.push_back(isd.times[idx]);
    }
    int getfirstnonemptyidx(void) {
      int nonemptyidx=-1;
      for (int i=0;i<imgdata.size();i++) {
        if (!imgdata[i].empty()) {
          nonemptyidx=i;
          break;
        }
      }
      return nonemptyidx;
    }
  protected:
    IMGDATA imgdata;
    SEQUENCETIMES times;
    bool reversed;
  };
  //##############################################
  //##############################################
}

#endif //GGFRAME
