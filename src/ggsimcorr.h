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
#ifndef GGSIMCORR
#define GGSIMCORR

#include "ggtracker.h"
#include "ggregcompare.h"
#include "ggutils.h"
#include "ggmath.h"
#include "ggframe.h"
#include "ggdefs.h"

namespace gg {
  //############################################
  //############################################
  //############################################
  class statetransitiontable {
  protected:
    cv::Mat_<T_FEAT> table;
  public:
    statetransitiontable(void): 
      table(cv::Mat_<T_FEAT>(RGS::cv_T::channels,RGS::cv_T::channels)) 
    {
      T_FEAT L=FSC::LOW,M=FSC::MEDIUM,H=FSC::HIGH;
      T_FEAT tmptrans[8][8]={ { H , M , L , L , M , M , M , M },
            { M , H , H , H , L , L , L , L },
            { L , H , H , L , L , L , L , L },
            { L , H , L , H , L , L , L , L },
            { M , L , L , L , H , L , L , L },
            { L , L , L , L , H , H , L , L },
            { L , L , L , L , L , L , H , L },
            { L , L , L , L , L , L , L , H } };
      for (size_t j=0;j<RGS::cv_T::channels;j++) {
  T_FEAT *rawtrans=table.ptr<T_FEAT>(j);
  memcpy(rawtrans,tmptrans[j],sizeof(T_FEAT)*RGS::cv_T::channels);
      }
      rownormalise<T_FEAT>(table);
    }
    const cv::Mat_<T_FEAT>& get(void) const { return table; }
  };
  //############################################
  //############################################
  class regsims {
  public:
    regsims(void) :
      simdata(cv::Mat_<FSC::cv_T>(_MAXNREGIONS,_MAXNREGIONS)),
      statedata(cv::Mat_<RGS::cv_T>(_MAXNREGIONS,_MAXNREGIONS)),
      statesimwghts(cv::Mat_<T_FEAT>(RGS::cv_T::channels,FSC::cv_T::channels))
    { 
      //this can be made much shorter - see the transition table constructor
      T_FEAT *rawwghts;

      rawwghts=statesimwghts.ptr<T_FEAT>(RGS::NORM);
      rawwghts[FSC::PHOTO]=FSC::MEDIUM;
      rawwghts[FSC::DIST]=FSC::HIGH;
      rawwghts[FSC::ANG]=FSC::NONE;
      rawwghts[FSC::SIZE]=FSC::MEDIUM;

      rawwghts=statesimwghts.ptr<T_FEAT>(RGS::SOCCLPART);
      rawwghts[FSC::PHOTO]=FSC::HIGH;
      rawwghts[FSC::DIST]=FSC::MEDIUM;
      rawwghts[FSC::ANG]=FSC::MEDIUM;
      rawwghts[FSC::SIZE]=FSC::LOW;

      rawwghts=statesimwghts.ptr<T_FEAT>(RGS::SOCCLSPLIT);
      rawwghts[FSC::PHOTO]=FSC::HIGH;
      rawwghts[FSC::DIST]=FSC::MEDIUM;
      rawwghts[FSC::ANG]=FSC::MEDIUM;
      rawwghts[FSC::SIZE]=FSC::LOW;

      rawwghts=statesimwghts.ptr<T_FEAT>(RGS::SOCCLCOMP);
      rawwghts[FSC::PHOTO]=FSC::MEDIUM;
      rawwghts[FSC::DIST]=FSC::MEDIUM;
      rawwghts[FSC::ANG]=FSC::MEDIUM;
      rawwghts[FSC::SIZE]=FSC::MEDIUM;

      rawwghts=statesimwghts.ptr<T_FEAT>(RGS::MOCCLPART);
      rawwghts[FSC::PHOTO]=FSC::LOW;
      rawwghts[FSC::DIST]=FSC::MEDIUM;
      rawwghts[FSC::ANG]=FSC::MEDIUM;
      rawwghts[FSC::SIZE]=FSC::LOW;

      rawwghts=statesimwghts.ptr<T_FEAT>(RGS::MOCCLCOMP);
      rawwghts[FSC::PHOTO]=FSC::MEDIUM;
      rawwghts[FSC::DIST]=FSC::HIGH;
      rawwghts[FSC::ANG]=FSC::HIGH;
      rawwghts[FSC::SIZE]=FSC::LOW;

      rawwghts=statesimwghts.ptr<T_FEAT>(RGS::UNDERSEG);
      rawwghts[FSC::PHOTO]=FSC::MEDIUM;
      rawwghts[FSC::DIST]=FSC::MEDIUM;
      rawwghts[FSC::ANG]=FSC::MEDIUM;
      rawwghts[FSC::SIZE]=FSC::LOW;

      rawwghts=statesimwghts.ptr<T_FEAT>(RGS::OVERSEG);
      rawwghts[FSC::PHOTO]=FSC::LOW;
      rawwghts[FSC::DIST]=FSC::LOW;
      rawwghts[FSC::ANG]=FSC::MEDIUM;
      rawwghts[FSC::SIZE]=FSC::LOW;

      //      rownormalise(simwghts);
      //      statesimwghts*=1./400.;
      rownormalise<T_FEAT>(statesimwghts);
    }
    void operator() (const frame &fA, const frame &fB) {
      //iterate thru regions
      for (size_t ra=0;ra<fA.size();ra++) {
  for (size_t rb=0;rb<fB.size();rb++) {
    rcompare(fA[ra],fB[rb],simdata(ra,rb)); //single matrix of vectors of values for each similarity function
    //eq.s 2 and 4
    calcstatesims(simdata(ra,rb),statedata(ra,rb)); //single matrix of vectors of values for each state
  }
      }
//      std::cout <<"fA.size():"<<fA.size()<<" fB.size():"<<fB.size()<<std::endl;
      op_cvMat<FSC::cv_T::value_type,FSC::cv_T::channels>(std::cout,simdata(cv::Range(0,fA.size()),cv::Range(0,fB.size())));
    }
    const cv::Mat_<RGS::cv_T>& getstatedata(void) const { return statedata; }
  protected:
    void calcstatesims(FSC::cv_T &simvals, RGS::cv_T &simstatevals) {
      for (size_t stateidx=0;stateidx<RGS::cv_T::channels;stateidx++) {
  T_FEAT *rawwghts=statesimwghts.ptr<T_FEAT>(stateidx);
  simstatevals[stateidx]=0.;
  for (size_t simidx=0;simidx<FSC::cv_T::channels;simidx++)
    simstatevals[stateidx]+=rawwghts[simidx]*simvals[simidx];
      }
    }

    regioncomparer rcompare;
    cv::Mat_<FSC::cv_T> simdata; //overall similarities (matrix of vectors)
    cv::Mat_<RGS::cv_T> statedata; //similarities for each state (matrix of vectors)
    cv::Mat_<T_FEAT> statesimwghts;
  };
  //############################################
  //############################################
    class regioncorrespondences {
    protected:
      cv::Mat_<T_FEAT> regcorrM;
      cv::Mat_<RGS::cv_T> regpostM;
      cv::Mat_<RGS::cv_T> regpriorM;
      cv::Mat_<T_FEAT> regmargM;
      statetransitiontable transtable;
      spectralcorr speccorr;
      size_t Asize,Bsize;
      cv::Mat_<size_t> regstateMv;
      CORRS prev2curr,curr2prev;
      //######
      void makeprior(size_t Asize, const frame &fB) {
  //  std::cout <<"makeprior Asize,fB.size:"<<Asize<<","<<fB.size()<<std::endl;
  regpriorM=RGS::cv_T(0.);
  for (size_t rb=0;rb<fB.size();rb++) {
    REGSTATE fBstate=fB[rb].state();
    for (size_t p=0;p<RGS::cv_T::channels;p++) {
      //      std::cout <<"p,fBstate:"<<p<<","<<fBstate<<" ";
      T_FEAT tabval=transtable.get()(p,fBstate);
      //        regpriorM(ra,rb)[p]=transtable.get()(p,fBstate); //XXX is fB to p the correct way around for this table? yes because fB is the column which is the previous state and p is the next state which is the variable we haven't decided about yet.
      for (size_t ra=0;ra<Asize;ra++) 
        regpriorM(ra,rb)[p]=tabval;
    }
    //    std::cout <<std::endl;
  }
      }
      void findstatesandmakeveccorr(size_t Asize, size_t Bsize) {
  regstateMv=0;
  std::fill(prev2curr.begin(),prev2curr.end(),-1);
  std::fill(curr2prev.begin(),curr2prev.end(),-1);
  for (size_t rb=0;rb<Bsize;rb++) {
    for (size_t ra=0;ra<Asize;ra++) {
      if (speccorr.getcorr()(ra,rb)>0. && regmargM(ra,rb)>0.5) {
        prev2curr[rb]=ra;
        curr2prev[ra]=rb;
        //then find state of region ra
        T_FEAT maxval=0.,val;
        size_t maxp=0;
        for (size_t p=0;p<RGS::cv_T::channels;p++) {
    val=regpostM(ra,rb)[p];
    if (val>maxval) {
      maxval=val; 
      maxp=p;
    }
        }
        //        region state matrix at ra, rb=maxp; //or does this belong somewhere in one of the data structures? probably not yet, let another entity decide as these correspondences will be calculated over a window to give multiple ideas and state values.
        regstateMv(0,ra)=maxp;
      }
    }
  }
  std::cout <<"prev2curr"<<std::endl;
  for (size_t rb=0;rb<Bsize;rb++)
    std::cout <<"["<<rb<<"]="<<prev2curr[rb]<<",";
  std::cout <<std::endl;

  std::cout <<"curr2prev"<<std::endl;
  for (size_t ra=0;ra<Asize;ra++)
    std::cout <<"["<<ra<<"]="<<curr2prev[ra]<<",";
  std::cout <<std::endl;

      }
    public:
      regioncorrespondences(void) 
  :
  regcorrM(cv::Mat_<T_FEAT>::zeros(_MAXNREGIONS,_MAXNREGIONS)),
  regpostM(cv::Mat_<RGS::cv_T>::zeros(_MAXNREGIONS,_MAXNREGIONS)),
  regpriorM(cv::Mat_<RGS::cv_T>::zeros(_MAXNREGIONS,_MAXNREGIONS)),
  regmargM(cv::Mat_<T_FEAT>::zeros(_MAXNREGIONS,_MAXNREGIONS)),
  Asize(0),Bsize(0),
  regstateMv(cv::Mat_<size_t>::zeros(1,_MAXNREGIONS)),
  prev2curr(_MAXNREGIONS,-1),
  curr2prev(_MAXNREGIONS,-1)
      {
  regcorrM=0.f;
  regpostM=RGS::NORM;
  regpriorM=RGS::NORM;
  regmargM=0.f;
  regstateMv=size_t(0);
      }
      void operator() (const regsims &sim, const frame &fA, const frame &fB) {
  //now what about eqs 6,7 and 8. 
  //eq. 8 implicitly needs to know the state of the region but which region? region j of course
  //loop through all previous regions, find their states x_c^j then multiply the transition probability to go to x_b^i
  //each time instance should have an associated state array for each region.
  Asize=fA.size(); Bsize=fB.size();
  if (Asize>0 && Bsize>0) {
    makeprior(fA.size(),fB); //P(x_b^i|x_c^j,m(i,j)) 
    regpostM=regpriorM.mul(sim.getstatedata()); //eq. 6 or 7
    marginalise<RGS::cv_T::value_type,RGS::cv_T::channels>(regpostM,regmargM); //eq. 8

    std::cout <<"Marginal:"<<std::endl;
    op_cvMat<T_FEAT>(std::cout,regmargM(cv::Range(0,Asize),cv::Range(0,Bsize)));

    speccorr(regmargM(cv::Range(0,fA.size()),cv::Range(0,fB.size())));

    std::cout <<"Spect:"<<std::endl;
    op_cvMat<T_FEAT>(std::cout,speccorr.getspect());

    std::cout <<"correspondences"<<std::endl;
    op_cvMat<T_FEAT>(std::cout,speccorr.getcorr());

    findstatesandmakeveccorr(Asize,Bsize);
//    op_cvMat<RGS::cv_T::value_type,RGS::cv_T::channels>(std::cout,regpostM(cv::Range(0,Asize),cv::Range(0,Bsize)));

//          prev2curr[rb]=ra;
//          curr2prev[ra]=rb;
    std::cout <<"prev2curr IDs"<<std::endl;
    for (size_t rb=0;rb<Bsize;rb++) {
      size_t bhash,ahash;
      bhash=fB[rb].uid_hash();
      if (prev2curr[rb]==-1)
        ahash=0;
      else
        ahash=fA[prev2curr[rb]].uid_hash();
      std::cout <<bhash<<"->"<<ahash<<",";
    }
    std::cout <<std::endl;
          std::cout <<"curr2prev IDs"<<std::endl;
          for (size_t ra=0;ra<Asize;ra++) {
            size_t bhash,ahash;
            ahash=fA[ra].uid_hash();
            if (curr2prev[ra]==-1)
              bhash=0;
            else
              bhash=fB[curr2prev[ra]].uid_hash();
            std::cout <<ahash<<"->"<<bhash<<",";
          }
          std::cout <<std::endl;
  }
  else {
    std::fill(prev2curr.begin(),prev2curr.end(),-1);
    std::fill(curr2prev.begin(),curr2prev.end(),-1);
  }
      }
      void setstates(frame &fA) {
  assert(fA.size()==Asize);
  for (size_t ra=0;ra<Asize;ra++) 
    fA[ra].state(REGSTATE(regstateMv(0,ra)));
#if(0)
  for (size_t ra=0;ra<Asize;ra++) 
    std::cout <<fA[ra].state()<<",";
  std::cout <<std::endl;
#endif
      }
      const CORRS& getcurr2prev(void) const {return curr2prev;}
      const CORRS& getprev2curr(void) const {return prev2curr;}
    };
  //############################################
  //############################################
}

#endif //GGSIMCORR
