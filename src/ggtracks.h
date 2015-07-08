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
#ifndef GGTRACKS
#define GGTRACKS

#include <deque>
#include "ggregiontrack.h"
#include "ggpropagating.h"
#include "ggclassify.h"
#include "ggclassification.h"
#include "ggframe.h"
#include "ggutils.h"
#include <fstream>
#include "ACDefinitions.h"

#include "shape/ggshapeextractor.h"
#include "shape/ggshapetrackproperties.h"
#include "shape/ggshapetrackfilter.h"

namespace gg {
/**
 * Base class that encapsulates a deque of tracks gg::objecttrackproperties \n
 * after initial processing which can be merged with other \n
 * instances of itself to generate merged sets of tracks. \n
 * It is a container for the objecttrackproperties class. \n
 */
template <class TRACK_T>
class objtracks {
public:
  typedef std::deque<TRACK_T> TRACKPROPS;
  typedef typename TRACKPROPS::iterator OBJTRACKit;
  typedef typename TRACKPROPS::const_iterator OBJTRACKcit;
  //    typedef typename std::deque<TRACK_T>::iterator OBJTRACKit;
  objtracks(size_t size) : tracks(size),opfileclassifications("objtracks.csv",std::ofstream::out|std::ofstream::app) {}
  objtracks(const objtracks &ot) : tracks(ot.tracks),opfileclassifications("objtracks.csv",std::ofstream::out|std::ofstream::app) {}
  virtual ~objtracks(void) { opfileclassifications.close(); }
  void clear(void) {
    for (size_t i=0;i<tracks.size();i++)
      tracks[i].clear();
    tracks.clear();
  }
  bool empty(void) { return tracks.empty(); }
  void push_back(const TRACK_T &t) {
    tracks.push_back(t);
  }
  virtual void addnewtrack(const TRACK_T &t) {
    tracks.push_back(t);
  }
  virtual TRACK_T& operator[] (size_t i) {
    if (i<tracks.size())
      return tracks[i];
    throw gg::error("ggtracks.hL35");
    return nulltrack;
  }
  virtual const TRACK_T& operator[] (size_t i) const {
    if (i<tracks.size())
      return tracks[i];
    throw gg::error("ggtracks.hL41");
    return nulltrack;
  }
  virtual const TRACKPROPS& gettracks(void) const { return tracks; }
  virtual const objtracks& operator=(const objtracks &_ot) {
    tracks=_ot.tracks;
    return *this;
  }
  virtual const size_t size(void) const { return tracks.size(); }
  virtual void erase(size_t i) {
    if (i>=tracks.size()) return;
    OBJTRACKit it=tracks.begin();
    size_t j=0;
    for (;j<i;it++,j++) {}
    tracks.erase(it);
  }
  virtual gg::FRAMENO earliestframeno(void) {
    if (tracks.empty())
      return gg::FRAMENO(0);
    gg::FRAMENO earliest=tracks[0].getminframeno();
    if (tracks.size()>1) {
      gg::FRAMENO min;
      for (size_t i=0;i<tracks.size();i++) {
        min=tracks[i].getminframeno();
        if (earliest<min)
          earliest=min;
      }
    }
    return earliest;
  }
  virtual void remove_merge(const gg::trackpropagateconstraints &constraints, objtracks<TRACK_T> &removedtracks) {
    if (tracks.empty())
      return;
    for (OBJTRACKit it=tracks.begin();it!=tracks.end();++it)
      removedtracks.push_back_merge(constraints, *it);
    tracks.clear();
  }
  virtual void remove_before(const gg::trackpropagateconstraints &constraints, const gg::FRAMENO &earliestframeno, objtracks<TRACK_T> &removedtracks) {
    if (tracks.empty())
      return;
    gg::FRAMENO deletebeforeno=earliestframeno-gg::trackpropagator::MAXPROPAGATE*2;
    if (!constraints.beforeminframeno(deletebeforeno))
      return;
    bool change=false;
    do {
      change=false;
      for (OBJTRACKit it=tracks.begin();it!=tracks.end();++it) {
        if ((*it).getminframeno()<deletebeforeno) {
                      (*it).dump(opfileclassifications);
          removedtracks.push_back(*it);
          tracks.erase(it);
          change=true;
          break;
        }
      }
    } while (change);
  }
  virtual void remove_before(const gg::trackpropagateconstraints &constraints, const gg::FRAMENO &earliestframeno) {
    if (tracks.empty())
      return;
    gg::FRAMENO deletebeforeno=earliestframeno-gg::trackpropagator::MAXPROPAGATE*2;
    if (!constraints.beforeminframeno(deletebeforeno))
      return;
    bool change=false;
    do {
      change=false;
      for (OBJTRACKit it=tracks.begin();it!=tracks.end();++it) {
        if ((*it).getminframeno()<deletebeforeno) {
                (*it).dump(opfileclassifications);
          //(*it).dump(std::cout);
          tracks.erase(it);
          change=true;
          break;
        }
      }
    } while (change);
  }
  virtual void push_back_merge(const gg::trackpropagateconstraints &constraints, const TRACK_T &t) {
    std::cout <<"PERFORMING PUSH_BACK_MERGE"<<std::endl;
    const gg::FRAMENOS &framenos=t.getframenos();
    std::cout <<"frame no size:"<<framenos.size()<<std::endl;
    std::cout <<"BEG'G:"<<framenos[0]<<"\t END'G:"<<framenos[framenos.size()-1]<<std::endl;
    std::cout <<"TRACK IDs:";
    for (size_t tkidx=0;tkidx<t.size();tkidx++)
      std::cout <<framenos[tkidx]<<":"<<t[tkidx].uid_hash()<<",";
    std::cout <<std::endl;
    if (tracks.size()>0) {
      gg::trackpropagator bprop(t,constraints);
      //  bprop.print();
      gg::trackmerger merger;
      bool merging=false;
      for (size_t i=0;i<tracks.size();i++) {
        gg::trackpropagator aprop(tracks[i],constraints);
        //    aprop.print();
        if (merger.aresimilar(aprop,bprop)) {
          std::cout <<"Merging..."<<std::endl;
          //~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
          const gg::FRAMENOS &oldframenos=tracks[i].getframenos();
          std::cout <<"old frame no size:"<<oldframenos.size()<<" BEG'G:"<<oldframenos[0]<<" END:"<<oldframenos[oldframenos.size()-1]<<std::endl;
          //~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
          for (size_t tkidx=0;tkidx<tracks[i].size();tkidx++)
            std::cout <<tracks[i][tkidx].uid_hash()<<",";
          std::cout <<std::endl;
          //~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
          merger.combine(aprop,bprop,false);
          tracks[i]=merger.getmerged();
          merging=true;
          //~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
          //      std::cout <<"MERGED:"<<std::endl;
          const gg::FRAMENOS &newframenos=tracks[i].getframenos();
          std::cout <<"new frame no size:"<<newframenos.size()<<" BEG'G:"<<newframenos[0]<<" END:"<<newframenos[oldframenos.size()-1]<<std::endl;
          for (size_t tkidx=0;tkidx<tracks[i].size();tkidx++)
            std::cout <<tracks[i][tkidx].uid_hash()<<",";
          std::cout <<std::endl;
          //~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
          break;
        }
      }
      if (!merging)
        tracks.push_back(t);
    }
    else
      tracks.push_back(t);
  }
  void mergesimilar(const gg::trackpropagateconstraints &constraints) {
    if (tracks.size()>1) {
      bool merging=false;
      gg::trackmerger merger;
      do {
        merging=false;
        for (size_t i=0;i<tracks.size();i++) {
          gg::trackpropagator aprop(tracks[i],constraints);
          for (size_t j=i+1;j<tracks.size();j++) {
            gg::trackpropagator bprop(tracks[j],constraints);
            if (merger.aresimilar(aprop,bprop)) {
              merger.combine(aprop,bprop,false);
              tracks[i]=merger.getmerged();
              erase(j);
              merging=true;
              break;
            }
          }
          if (merging) break;
        }
      } while (merging);
    }
  }
  virtual void dump(void) {
    dump(opfileclassifications);
  }
protected:
  void dump(std::ostream &os) {
    os << "*objtracks*" << std::endl;
    os << "no of tracks:" <<tracks.size()<<std::endl;
    for (size_t i=0;i<tracks.size();i++) {
      os <<"TRACK:"<<i<<",";
      os <<"START:"<<tracks[i].getframenos()[0]<<","<<tracks[i][0].uid_hash()<<",";
      int last=tracks[i].getframenos().size()-1;
      assert(last>=0);
      os <<"END:"<<tracks[i].getframenos()[last]<<","<<tracks[i][last].uid_hash()<<",";
      int mid=last*0.5;
      os <<"MID:"<<tracks[i].getframenos()[mid]<<","<<tracks[i][mid].uid_hash();
      os <<std::endl;
      tracks[i].dump(os);

      std::string sequencefilename("seq-");
      sequencefilename+=std::to_string(uint(tracks[i].uid_hash()));
      sequencefilename+=".avi";
      std::cout <<"sequencefilename:"<<sequencefilename<<std::endl;
      cv::VideoWriter subvideoout(sequencefilename,CV_FOURCC('x','v','i','d'),25,tracks[i].getregiondata()[0].binarytemp().size(),true);
      std::cout <<"initialising shapetrackproperties"<<std::endl;
      shapetrackproperties shapetrack(tracks[i]);
      std::cout <<"initialising shapetrackextractor:"<<std::endl;
      shapetrackextractor extractor;
      std::cout <<"extracting"<<std::endl;
      extractor(shapetrack);
      std::cout <<"dumping (extracting finished)"<<std::endl;
      shapetrack.dump(subvideoout);
      shapetrackfilter stf;
      stf(shapetrack);

      sequencefilename="box-";
      sequencefilename+=std::to_string(uint(tracks[i].uid_hash()));
      sequencefilename+="-";
      stf.dump(sequencefilename,stf.getboxedshapes());

      sequencefilename="unbox-";
      sequencefilename+=std::to_string(uint(tracks[i].uid_hash()));
      sequencefilename+="-";
      stf.dump(sequencefilename,stf.getunboxedshapes());

      sequencefilename="origroi-";
      sequencefilename+=std::to_string(uint(tracks[i].uid_hash()));
      sequencefilename+="-";
      stf.dump(sequencefilename,stf.getorigrois());
    }
  }
  objtracks(void) {}
  TRACKPROPS tracks;
  TRACK_T nulltrack;
  std::ofstream opfileclassifications;
};
//#######################################################
//#######################################################
/**
 * Inherits from gg::objtracks and implements\n
 * combining the track information with the image\n
 * information.
 */
class imgtracks : public objtracks<imgtrackproperties> {
public:
  typedef objtracks<imgtrackproperties>::TRACKPROPS IMGPROPS;
  typedef objtracks<imgtrackproperties>::OBJTRACKit IMGPROPSit;
  ///constant iterator
  typedef objtracks<imgtrackproperties>::OBJTRACKcit IMGPROPScit;
  using objtracks<imgtrackproperties>::tracks;
  imgtracks(const imgtracks &_imgt) :
    objtracks<imgtrackproperties>::objtracks(_imgt)
    {}
  imgtracks(const objtracks<imgtrackproperties> &_imgt) :
    objtracks<imgtrackproperties>::objtracks(_imgt)
    {}
  imgtracks(const objtracks<objecttrackproperties> &_tracks, const imgsequencedata &_imgs) {
    tracks.resize(_tracks.size());
    for (size_t i=0;i<_tracks.size();i++)
      tracks[i].setdata(_tracks[i],_imgs);
  }
  imgtracks(size_t size) : objtracks<imgtrackproperties>::objtracks(size) {}
  const imgtracks& operator=(const imgtracks &_imgt) {
    objtracks<imgtrackproperties>::operator=(_imgt);
    return *this;
  }
  virtual void operator() (const objtracks<imgtrackproperties> &_tracks) {
    objtracks<imgtrackproperties>::operator=(_tracks);
  }
  virtual void operator() (const objtracks<objecttrackproperties> &_tracks, const imgsequencedata &_imgs) {
    tracks.resize(_tracks.size());
    for (size_t i=0;i<_tracks.size();i++) {
      tracks[i].setdata(_tracks[i],_imgs);
    }
  }
#if(0)
  virtual void dump(void) {
    dump(opfileclassifications);
  }
protected:
  void dump(std::ostream &os) {
    os << "*objtracks*" << std::endl;
    os << "no of tracks:" <<tracks.size()<<std::endl;
    for (size_t i=0;i<tracks.size();i++) {
      os <<"TRACK:"<<i<<",";
      os <<"START:"<<tracks[i].getframenos()[0]<<","<<tracks[i][0].uid_hash()<<",";
      int last=tracks[i].getframenos().size()-1;
      assert(last>=0);
      os <<"END:"<<tracks[i].getframenos()[last]<<","<<tracks[i][last].uid_hash()<<",";
      int mid=last*0.5;
      os <<"MID:"<<tracks[i].getframenos()[mid]<<","<<tracks[i][mid].uid_hash();
      os <<std::endl;
      tracks[i].dump(os);
      std::string sequencefilename("seq-");
      //        sequencefilename+=std::to_string(tracks[i].uid_hash());
      //        sequencefilename+=ACDefinitions::StringAndZeroPad(int(tracks[i].uid_hash()),10);
      sequencefilename+=std::to_string(uint(tracks[i].uid_hash()));
      sequencefilename+=".avi";
      std::cout <<"sequencefilename:"<<sequencefilename<<std::endl;
      cv::VideoWriter subvideoout(sequencefilename,CV_FOURCC('x','v','i','d'),25,tracks[i].getregiondata()[0].binarytemp().size(),true);
      tracks[i].dump(subvideoout);
    }
  }
#endif
};
//#######################################################
//#######################################################
/**
 * Inherited from gg::objtracks (not abstract) and implements\n
 *  a classify function for each of the frame regions.
 */
template <class TRACK_T>
class motorbiketracks : public objtracks<TRACK_T> {
public:
  typedef std::vector<gg::discreteclassification>  CLASSIFICATIONS;
protected:
  motorbiketracks(void) {}
  CLASSIFICATIONS classifications;
  using objtracks<TRACK_T>::tracks;
  using objtracks<TRACK_T>::opfileclassifications;
  std::vector<float> mtb_cnt,not_mtb_cnt,res;
  inline float mtb_calc(size_t i) {
    return mtb_cnt[i]/(mtb_cnt[i]+not_mtb_cnt[i]);
  }
  inline bool ismotorbike(size_t i) {
    if (mtb_calc(i)>0.06)
      return true;
    return false;
  }
public:
  motorbiketracks(const motorbiketracks &_mbt) :
    classifications(_mbt.classifications),
    objtracks<TRACK_T>::objtracks(_mbt) {}
  motorbiketracks(const objtracks<TRACK_T> &_ts) :
    objtracks<TRACK_T>::objtracks(_ts)
    {}
  motorbiketracks(size_t size) :
    objtracks<TRACK_T>::objtracks(size) {}
  const motorbiketracks& operator=(const motorbiketracks &_mbt) {
    classifications=_mbt.classifications;
    objtracks<TRACK_T>::operator=(_mbt);
    return *this;
  }
  ///classifies sequence overall using individual classification results from
  ///gg::motorbikeimgtracks::individualclassification gg::Heads::isHead
  void classify(void) {
    classifications=CLASSIFICATIONS(tracks.size());
    mtb_cnt.resize(tracks.size());
    not_mtb_cnt.resize(tracks.size());
    res.resize(tracks.size());
    for (size_t i=0;i<tracks.size();i++) {
      //first check to see if it is a motorbike...
      mtb_cnt[i]=0,not_mtb_cnt[i]=0,res[i]=0;
      for (size_t j=0;j<tracks[i].size();j++) {
        if (tracks[i][j].getregprops().getregprops().getclassification()==gg::vehicleclassification::MOTORBIKE)
          mtb_cnt[i]++;
        else if (tracks[i][j].getregprops().getregprops().getclassification()==gg::vehicleclassification::NOTMOTORBIKE)
          not_mtb_cnt[i]++;
      }
      if (ismotorbike(i)) {
        float sum=0.,norm=0.,size,wght;
        gg::discreteclassification c;
        for (size_t j=0;j<objtracks<TRACK_T>::tracks[i].count();j++) {
          c=objtracks<TRACK_T>::tracks[i][j].getregprops().getsubregprops().getclassification();
          if (c!=gg::discreteclassification::NOCLASSIFICATION) {
            size=objtracks<TRACK_T>::tracks[i][j].getregprops().getbgsubregprops().getsize();
            if (size<1.) size=1.;
            wght=size;
            sum+=c.get()*wght;
            norm+=wght;
          }
          else {
            //nothing!
            wght=0;
          }
        }
        res[i]=sum/norm;
        classifications[i]=discreteclassification::classT(cvRound(res[i]));
      }
      else
        classifications[i]=vehicleclassification::NOTMOTORBIKE;
    }
  }
  const CLASSIFICATIONS& getclassifications(void) { return classifications; }
  virtual void dump() {
    //      objtracks<TRACK_T>::dump();
    std::ofstream &os=opfileclassifications; //std::cout;
    os <<"*motorbiketracks<imgtrackproperties>*"<<std::endl;
    for (size_t i=0;i<tracks.size();i++) {
      os <<"TRACK:"<<i<<",";
      os <<"START:"<<tracks[i].getframenos()[0]<<","<<tracks[i][0].uid_hash()<<",";
      int last=tracks[i].getframenos().size()-1;
      assert(last>=0);
      os <<"END:"<<tracks[i].getframenos()[last]<<","<<tracks[i][last].uid_hash()<<",";
      int mid=last*0.5;
      os <<"MID:"<<tracks[i].getframenos()[mid]<<","<<tracks[i][mid].uid_hash()<<",";
      os <<"#MTB="<<mtb_cnt[i]<<",#NOT-MTB="<<not_mtb_cnt[i]<<",mtb_calc:"<<mtb_calc(i)<<","<<res[i]<<",";
      os <<"vehicletype:";
      if (vehicleclassification::is(classifications[i].get())) //then either NOTMOTORBIKE or NOCLASSIFICATION
        os <<vehicleclassification::str(classifications[i].get());
      else if (helmetclassification::is(classifications[i].get())) { //then ...
        os <<vehicleclassification::str(vehicleclassification::MOTORBIKE)<<":";
        os <<helmetclassification::str(classifications[i].get());
      }
      os <<std::endl;
    }
  }
};
//#######################################################
//#######################################################
/**
 * Inherited from gg::motorbiketracks<gg::imgtrackproperties>
 * (not abstract) and implements\n
 * a classify function for individual frames then the tracks.
 */
class motorbikeimgtracks : public motorbiketracks<imgtrackproperties> {
  using objtracks<imgtrackproperties>::tracks;
  using motorbiketracks<imgtrackproperties>::classifications;
  using objtracks<imgtrackproperties>::opfileclassifications;
public:
  motorbikeimgtracks(const imgtracks &it) : motorbiketracks<imgtrackproperties>::motorbiketracks(it) {}
  void individualclassification(gg::HeadClassifier &headclass) {
    //for motorbiketracks consisting of some non motorbikes!
    //1. classify individual frame regions from tracks into motorbikes or not
    //2. classify each track as a whole into motorbikes or not
    //3. classify individual frame regions from motorbike tracks into helmet wearing or not
    //4. classify each motorbike track into helmet wearing or not

    Heads headchecker;
    region::REGIONPNTS regpnts;
    float headproportion;
    region::REGIONPNTS headpnts;

    helmetclassifier clfr(headclass);

    for (size_t i=0;i<tracks.size();i++) {
      std::cout <<"TRACK:"<<i<<",";
      for (size_t j=0;j<tracks[i].size();j++) {
        std::cout <<"frame:"<<tracks[i].getframenos()[i]<<","<<tracks[i].uid_hash()<<",";
        if (tracks[i][j].getregprops().getsize()>10) {
          const cv::Mat &img=tracks[i].img(j);
          cv::Mat mask(img.size(),cv::DataType<uchar>::type);
          IplImage cmask(mask);

          regpnts.clear();
          region::cvMat2REGIONPNTS<cv::Point>(tracks[i][j].getregprops().getpnts(),regpnts);
          if (headchecker.isHead(regpnts,headproportion)) {
            std::cout <<"MOTORBIKE"<<",";
            tracks[i][j].getregprops().getregprops().setclassification(vehicleclassification::MOTORBIKE);

            headpnts.clear();
            headchecker.Make(regpnts,headproportion,headpnts);
            region::SetBinImg(headpnts,&cmask,true);

            cv::Mat cvmheadpnts(headpnts);
            regionproperties subregprops(img,mask,cvmheadpnts);

            tracks[i][j].getregprops().setsubregprops(subregprops);
            helmetclassification c=helmetclassification(clfr.classify(tracks[i][j].getregprops()));
            tracks[i][j].getregprops().getsubregprops().setclassification(c);
            std::cout <<"classification:"<<c.str()<<",";
          }
          else {
            std::cout <<"NOTMOTORBIKE"<<",";
            tracks[i][j].getregprops().getregprops().setclassification(vehicleclassification::NOTMOTORBIKE);
          }
        }
        else {
          std::cout <<"NOCLASSIFICATION"<<",";
          tracks[i][j].getregprops().getregprops().setclassification(discreteclassification::NOCLASSIFICATION);
        }
      }
      std::cout <<std::endl;
    }
    std::cout <<"individualclassification-end"<<std::endl;
  }
  void dump(void) {
    motorbiketracks<imgtrackproperties>::dump();
#if(0)
    std::ostream &os=opfileclassifications; //std::cout;
    os <<"*motorbikeimgtracks*"<<std::endl;
    for (size_t i=0;i<tracks.size();i++) {
      os <<"TRACK:"<<i<<std::endl;
      for (size_t j=0;j<tracks[i].size();j++) {
        os <<"frame:"<<tracks[i].getframenos()[j]<<","<<tracks[i][j].uid_hash()<<",";
        vehicleclassification v=tracks[i][j].getregprops().getregprops().getclassification();
        os <<"vehicletype:"<<v.str()<<",";
        if (v==vehicleclassification::MOTORBIKE) {
          helmetclassification h=tracks[i][j].getregprops().getsubregprops().getclassification();
          os <<"classification:"<<h.str()<<",";
        }
      }
      os <<std::endl;
    }
#endif
  }
};
}

#endif //GGTRACKS
