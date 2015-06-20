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
#ifndef GGREGCOMPARE
#define GGREGCOMPARE

#include "ggtracker.h"
#include "ggutils.h"

namespace gg {
  //############################################
  //############################################
  float dist(const cv::Point &a, const cv::Point &b);
  float ang(const cv::Point &a, const cv::Point &b);
  //############################################
  //############################################
  struct featurescompare {
    T_FEAT photo,dist,ang,size;
    typedef cv::Vec<T_FEAT,4> cv_T;
    operator cv_T(void) { return structtocvvec<featurescompare,T_FEAT,4>(*this); }
    enum idx { PHOTO=0,DIST=1,ANG=2,SIZE=3 };
    enum weights { NONE=0, LOW=25,MEDIUM=50,HIGH=75};
  };
  typedef featurescompare FSC;
  //############################################
  //############################################
  class regioncomparer {
  protected:
    cv::Point cogA,cogB;
    T_FEAT distdiff,angdiff;
    T_FEAT sizeA, sizeB;
    T_FEAT sizediff;
    T_FEAT photodiff;

    T_FEAT avsize,minsize,maxsize;
    const T_FEAT photoinvvar,distinvvar,anginvvar,sizeinvvar;
    //    featurescompare featcomp;
  public:
    regioncomparer(void) : photoinvvar(100.),distinvvar(100.),anginvvar(1./150.),sizeinvvar(20.) {}
    void operator() (const frameregion &fA, const frameregion &fB, FSC::cv_T &_featcomp)  {
      cogA=fA.getregprops().getcog();
      cogB=fB.getregprops().getcog();
      minsize=fA.getregprops().getsize();
      maxsize=fB.getregprops().getsize();
      if (minsize>fB.getregprops().getsize()) {
        maxsize=minsize;
        minsize=fB.getregprops().getsize();
      }
      avsize=fA.getregprops().getsize()+fB.getregprops().getsize()/2.f;
      distdiff=dist(cogA,cogB)/minsize;
      angdiff=ang(cogA,cogB);
      sizediff=abs(fA.getregprops().getsize()-fB.getregprops().getsize())/maxsize;
      photodiff=fA.getregprops().getphoto().compare(fB.getregprops().getphoto());
      _featcomp[FSC::PHOTO]=exp(-pow(photodiff,2.)*0.5*photoinvvar);
      _featcomp[FSC::DIST]=exp(-pow(distdiff,2.)*0.5*distinvvar);
      _featcomp[FSC::ANG]=exp(-pow(angdiff,2.)*0.5*anginvvar);
      _featcomp[FSC::SIZE]=exp(-pow(sizediff,2.)*0.5*sizeinvvar);
    }
    //    const featurescompare& get(void) const {return featcomp;}
  };
  //############################################
  //############################################
}

#endif //GGREGCOMPARE


