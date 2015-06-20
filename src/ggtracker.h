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
#ifndef GGTRACKER
#define GGTRACKER

#include "globalCVHeader.h"
#include "region.h"
#include <vector>
#include "gghelpers.h"
#include "ggutils.h"
#include "ggregionprops.h"
#include "ggregionhierarchy.h"

#include <boost/uuid/uuid.hpp>
#include <boost/uuid/uuid_generators.hpp>
#include <boost/uuid/uuid_io.hpp>
#include <boost/functional/hash.hpp>
#include "gguuid.h"

namespace gg {
  //############################################
  //############################################
  class frameproperties {
  public:
    frameproperties(size_t _X, size_t _Y, size_t _framet) : X(_X), Y(_Y), framet(_framet) {}
    frameproperties(const frameproperties &props) : X(props.X), Y(props.Y), framet(props.framet) {}
    size_t X,Y;
    size_t framet;
  protected:
    frameproperties(void);
  };
  //############################################
  //############################################
  struct regionstates {
    T_FEAT norm,socclpart,socclsplit,socclcomp,mocclpart,mocclcomp,underseg,overseg;
    typedef cv::Vec<T_FEAT,8> cv_T;
    operator cv_T(void) { return structtocvvec<regionstates,T_FEAT,8>(*this); }
    enum idx {NORM=0,SOCCLPART=1,SOCCLSPLIT=2,SOCCLCOMP=3,MOCCLPART=4,MOCCLCOMP=5,UNDERSEG=6,OVERSEG=7};
  };
  typedef regionstates RGS;
  typedef RGS::idx REGSTATE;
  //############################################
  //############################################
  class frameregion {
  protected:
    cv::Mat binarytemplate;
    regionhierarchy regprops;
    bool valid;
    REGSTATE _state;
    boost::uuids::uuid uid_tag;
  public:
    frameregion(const frameproperties &props) : 
      binarytemplate(cv::Mat::zeros(props.Y,props.X,CVT_BINT)), 
      valid(false),
      _state(RGS::NORM),
      uid_tag(boost::uuids::random_generator(ran)())
      {}
    frameregion(void) :
      valid(false),
      _state(RGS::NORM),
      uid_tag(boost::uuids::nil_uuid())
    {}
    frameregion(const frameregion &fr) {
      if (fr.valid)
        set(fr);
      else {
        valid=false;
        uid_tag=boost::uuids::nil_uuid();
      }
    }
    frameregion& operator=(const frameregion &r) {
      set(r);
      return *this;
    }
    void set(const cv::Mat &_binarytemplate, const regionhierarchy &_regprops) {
      //      _binarytemplate.copyTo(binarytemplate); 
      _binarytemplate.convertTo(binarytemplate,binarytemplate.type()); 
      regprops.set(_regprops);
      _state=RGS::NORM;
      valid=true;
      uid_tag=boost::uuids::random_generator(ran)();
    }
    void set(const frameregion &r) { 
      r.binarytemplate.copyTo(binarytemplate); 
      regprops.set(r.regprops); 
      _state=r._state;
      valid=r.valid; 
      uid_tag=r.uid_tag;
    }
    void unset(void) {
        valid=false;
        uid_tag=boost::uuids::nil_uuid();
    }
    void merge(const frameregion &_fr) {
      assert(_fr.isvalid());
      if (!_fr.isvalid()) return;
      if (!valid)
        this->set(_fr);
      else {
        cv::bitwise_or(binarytemplate,_fr.binarytemplate,binarytemplate);
        _state=_fr._state;
        regprops.merge(_fr.regprops);
        uid_tag=_fr.uid_tag;
      }
    }
    void correctboundingbox(const region::Rects::R &_r) {
      regprops.setboundbox(_r);
    }
    bool isvalid(void) const {return valid;}
    const regionhierarchy& getregprops(void) const {return regprops;}
    regionhierarchy& getregprops(void) {return regprops;}
    const cv::Mat& binarytemp(void) const {return binarytemplate;}
    const cv::Mat& binarytemp(void) {return binarytemplate;}
    const REGSTATE& state(void) const { return _state; }
    void state(const REGSTATE &__state) {_state=__state;}
//    const boost::uuids::uuid& uid_tag(void) const { return uid_tag; }
    const size_t uid_hash(void) const { boost::hash<boost::uuids::uuid> uuid_hasher; return uuid_hasher(uid_tag);}
  };
  //############################################
  //############################################
}
#endif //GGTRACKER
