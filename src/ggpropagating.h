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
#ifndef GGPROPAGATING
#define GGPROPAGATING

#include "ggtracker.h"
#include "ggregiontrack.h"
#include "ggdefs.h"

namespace gg {
  //####################################################
  //####################################################
  class trackpropagateconstraints { //expects column vectors
  public:
    trackpropagateconstraints(void) :
      mins(0,0),maxs(0,0),
      active(false),
      minframeno(0),maxframeno(0){}
    trackpropagateconstraints(const cv::Mat_<float> &_mins, 
			      const cv::Mat_<float> &_maxs, 
			      int _minframeno, 
			      int _maxframeno ) : 
      mins(_mins),maxs(_maxs),
      active(true),
      minframeno(_minframeno),maxframeno(_maxframeno) {}
    void operator() (const cv::Mat_<float> &_mins, 
		     const cv::Mat_<float> &_maxs, 
		     int _minframeno, int _maxframeno ) {
      mins=_mins; maxs=_maxs;
      minframeno=_minframeno;
      maxframeno=_maxframeno;
    }
    inline bool operator() (const cv::Mat_<float> &meas, int frameno) const { //returns false if beyond constraints
      for (size_t j=0;j<mins.rows;j++) {
	if (meas(j,0)<mins(j,0))
	  return false;
      }
      for (size_t j=0;j<maxs.rows;j++) {
	if (meas(j,0)>=maxs(j,0))
	  return false;
      }
      if (frameno<minframeno)
	return false;
      if (maxframeno>0 && frameno>maxframeno)
	return false;
      return true;
    }
    bool isactive(void) const {return active;}
    inline bool beforeminframeno (gg::FRAMENO frameno) const {
      if (frameno<minframeno)
        return false;
      return true;
    }
    inline bool aftermaxframeno (gg::FRAMENO frameno) const {
      if (frameno>maxframeno && minframeno!=maxframeno)
        return false;
      return true;
    }
  protected:
    cv::Mat_<float> mins,maxs;
    bool active;
    int minframeno,maxframeno;
  };
  //####################################################
  //####################################################
  class trackpropagator {
  public:
    trackpropagator(void) : track(imgtrackproperties()), constraints(trackpropagateconstraints()) {}
    trackpropagator(const imgtrackproperties &_track,
		    const trackpropagateconstraints &_constraints) : 
      track(_track),
      constraints(_constraints)
    {
      forward();
      backward();
    }
    void operator() (void) {
      forward();
      backward();
    }
    friend class trackmerger;
    void dump(void) {
      std::cout <<"OBSERVED";
      printtrack(track,5);
      std::cout <<"FORWARD";
      printtrack(forwardtrack,5);
      std::cout <<"BACKWARD";
      printtrack(backwardtrack,5);
    }
    static const size_t MAXPROPAGATE=_MAXPROPAGATE;
  protected:
//    const objecttrackproperties &track;
    const imgtrackproperties &track;
    const trackpropagateconstraints &constraints;
    //objecttrackproperties forwardtrack,backwardtrack;
    imgtrackproperties forwardtrack,backwardtrack;
    void forward(void) {
      forwardtrack=track;
      frameregion emptyreg;
      cv::Mat emptyimg;
      const FRAMENOS &nos=forwardtrack.getframenos();
      size_t currframeno=nos[nos.size()-1]+1;
      for (size_t i=0;i<MAXPROPAGATE;i++) {
	forwardtrack.predict();
	const cv::Mat_<float> m(forwardtrack.getstatepre()(cv::Range(0,forwardtrack.getnmeasure()),cv::Range(0,1)));
	if (!constraints(m,currframeno))
	  break;
	forwardtrack.correct(m,currframeno++,emptyreg,emptyimg);
      }
    }
    void backward(void) {
      const Measurements &ms=track.getmeasurements();
      const FRAMENOS &nos=track.getframenos();
      //      size_t frameno=track.getmaxframeno();
      int no=ms.size();
      //      backwardtrack.initkalman(ms[no-1]);
      const REGIONDATA &rd=track.getregiondata();
      int i=no-1;
      cv::Mat emptyimg;
      backwardtrack.initkalman(ms[i],nos[i],rd[i],emptyimg);
      //reverse measurements
      for (i=no-2;i>=0;i--) {
	backwardtrack.predict();
	backwardtrack.correct(ms[i],nos[i],rd[i],emptyimg);
      }
      //backward propagate
      frameregion emptyreg;
      for (size_t j=0,frameno=nos[0];j<MAXPROPAGATE;j++,frameno--) {
	backwardtrack.predict();
	const cv::Mat_<float> m(backwardtrack.getstatepre()(cv::Range(0,backwardtrack.getnmeasure()),cv::Range(0,1)));
	if (!constraints(m,frameno))
	  break;
	backwardtrack.correct(m,frameno,emptyreg,emptyimg);
      }
      backwardtrack.reverse();
    }

    enum PROPA_TYPE {OBSERVED,FORWARD,BACKWARD};
    int find(size_t frameno, PROPA_TYPE t) const {
      int idx=-1;
      switch (t) 
	{
	case OBSERVED:
	  idx=find(track,frameno);
	  break;
	case FORWARD:
	  idx=find(forwardtrack,frameno);
	  break;
	case BACKWARD:
	  idx=find(backwardtrack,frameno);
	  break;
	default:
	  break;
	}
      return idx;
    }
    int find(const objecttrackproperties &otp, const size_t frameno) const {
      const FRAMENOS &fs=otp.getframenos();
      size_t i=0;
      while (frameno!=fs[i] && i<fs.size()) i++;
      if (i<fs.size()) 
	return i;
      return -1;
    }
    void printtrack(const objecttrackproperties &otp,const size_t nms) {
      std::cout <<"COUNT:"<<otp.count()<<std::endl;
      for (size_t i=0;i<otp.count();i++) {
	std::cout <<"FRAMENO:"<<otp.getframenos()[i]<<"\t"<<"MEASUREMENTS:";
	for (size_t j=0;j<nms;j++)
	  std::cout <<otp.getmeasurements()[i](j,0)<<",";
	//	if (otp.getregiondata()[i].getregprops().getclassification()==gg::regionhierarchy::HELMET)
	if (otp.getregiondata()[i].getregprops().getsubregprops().getclassification()==gg::helmetclassification::HELMET)
	  std::cout <<"HELMET";
	//	else if (otp.getregiondata()[i].getregprops().getclassification()==gg::regionhierarchy::NOHELMET)
	else if (otp.getregiondata()[i].getregprops().getsubregprops().getclassification()==gg::helmetclassification::NOHELMET)
	  std::cout <<"NOHELMET";
	else
	  std::cout <<"NOCLASSIFICATION";
	std::cout <<"\t HEADSIZE:"<<otp.getregiondata()[i].getregprops().getsubregprops().getsize();
	std::cout <<std::endl;
      }

    }  
  };
  //####################################################
  //####################################################
  class trackmerger { //friend of trackpropagator
  public:
    trackmerger(void) : aidx_mid(-1),bidx_mid(-1),midframeno(-1),mergetracks(false) {}
    bool aresimilar(const trackpropagator &a, const trackpropagator &b) { //c=merge(a,b);
      mergetracks=false;
      if (!mergeintcheck(a,b)) mergeintcheck(b,a);
      return mergetracks;
    }
    void combine(const trackpropagator &a, const trackpropagator &b, bool recalc) {
      if (a.track.getmaxframeno()<b.track.getminframeno()) {
	if (recalc) 
	  recalcparams(a,b);
	combineint(a,b);
      }
      else {
	if (recalc) 
	  recalcparams(b,a);
	combineint(b,a);
      }
    }
    const imgtrackproperties& getmerged(void) const { return otp; }
  protected:
    void recalcparams(const trackpropagator &a, const trackpropagator &b) {
      midframeno=cvRound((a.track.getmaxframeno()+b.track.getminframeno())*0.5);
      aidx_mid=a.find(midframeno,trackpropagator::FORWARD);
      bidx_mid=b.find(midframeno,trackpropagator::BACKWARD);
    }
#if(0)
    bool mergeintcheck(const trackpropagator &a, const trackpropagator &b) {
      mergetracks=false;
      std::cout <<	a.forwardtrack.getmaxframeno()   <<"\t"
		<<      b.backwardtrack.getminframeno()   <<"\t"
		<<      a.track.getmaxframeno()   <<"\t"
		<<      b.track.getminframeno()   <<std::endl;
      if (a.forwardtrack.getmaxframeno()>b.backwardtrack.getminframeno() &&
	  a.track.getmaxframeno()<b.track.getminframeno()) {
        midframeno=cvRound((a.track.getmaxframeno()+b.track.getminframeno())*0.5);
	aidx_mid=a.find(midframeno,trackpropagator::FORWARD);
	bidx_mid=b.find(midframeno,trackpropagator::BACKWARD);
	std::cout <<"<Possible merge> midframeno:"<<midframeno<<" "<<aidx_mid<<" "<<bidx_mid<<std::endl; 
	if (aidx_mid!=-1 && bidx_mid!=-1) { //XXX alternative merge criteria here
	  const cv::Mat_<float> &ameas=a.forwardtrack.getmeasurements()[aidx_mid];
	  const cv::Mat_<float> &bmeas=b.backwardtrack.getmeasurements()[bidx_mid];
	  float dist=sqrt(pow(ameas(0,0)-bmeas(0,0),2.)+pow(ameas(1,0)-bmeas(1,0),2.));
	  std::cout <<"dist:"<<dist<<std::endl;
	  if (dist<MINMERGEDIST) mergetracks=true;
	}
      }
      else
	std::cout <<"merge not possible.\n";
      return mergetracks;
    }
#else
    bool mergeintcheck(const trackpropagator &a, const trackpropagator &b) {
      mergetracks=false;
      std::cout <<      a.forwardtrack.getmaxframeno()   <<"\t"
                <<      b.backwardtrack.getminframeno()   <<"\t"
                <<      a.track.getmaxframeno()   <<"\t"
                <<      b.track.getminframeno()   <<std::endl;
      if (a.forwardtrack.getmaxframeno()>b.backwardtrack.getminframeno() &&
          a.track.getmaxframeno()<b.track.getminframeno()) {
        midframeno=cvRound((a.track.getmaxframeno()+b.track.getminframeno())*0.5);
        aidx_mid=a.find(midframeno,trackpropagator::FORWARD);
        bidx_mid=b.find(midframeno,trackpropagator::BACKWARD);
        std::cout <<"<Possible merge> midframeno:"<<midframeno<<" "<<aidx_mid<<" "<<bidx_mid<<std::endl;
        if (aidx_mid!=-1 && bidx_mid!=-1) { //XXX alternative merge criteria here
          const cv::Mat_<float> &ameas=a.forwardtrack.getmeasurements()[aidx_mid];
          const cv::Mat_<float> &bmeas=b.backwardtrack.getmeasurements()[bidx_mid];

          float dist=sqrt(pow(ameas(0,0)-bmeas(0,0),2.)+pow(ameas(1,0)-bmeas(1,0),2.));
          std::cout <<"dist:"<<dist<<std::endl;
          if (dist<MINMERGEDIST) mergetracks=true;
          else {
            assert(!a.track.isreversed() && !b.track.isreversed());
            size_t aendidx=a.track.size()-1,bbegidx=0;
            const frameregion &aendreg=a.track.getregiondata()[aendidx];
            const frameregion &bbegreg=b.track.getregiondata()[bbegidx];
            const cv::Mat_<float> &aendmeas=a.track.getmeasurements()[aendidx];
            const cv::Mat_<float> &bbegmeas=b.track.getmeasurements()[bbegidx];

            cv::Point adisplace(ameas(0,0)-aendmeas(0,0),ameas(1,0)-aendmeas(1,0));
            cv::Point bdisplace(bmeas(0,0)-bbegmeas(0,0),bmeas(1,0)-bbegmeas(1,0));

            cv::Rect arect=aendreg.getregprops().getboundbox().boundingRect();
            cv::Rect brect=bbegreg.getregprops().getboundbox().boundingRect();

            arect+=adisplace;
            brect+=bdisplace;

            cv::Rect intsect=arect&brect;
            if (intsect.area()>1)
              mergetracks=true;
            std::cout <<"Intersection Area:"<<intsect.area()<<std::endl;
          }
        }
      }
      else
        std::cout <<"merge not possible.\n";
      return mergetracks;
    }
#endif
    void combineint(const trackpropagator &a, const trackpropagator &b) {
      frameregion emptyreg;
      cv::Mat emptyimg;
      otp=a.track;
      for (size_t fr_no=otp.getmaxframeno()+1,i=otp.count();fr_no<midframeno;fr_no++,i++) {
	otp.predict();
	otp.correct(a.forwardtrack.getmeasurements()[i],fr_no,emptyreg,emptyimg);
      }
      for (size_t fr_no=midframeno,i=0;fr_no<b.track.getminframeno();fr_no++,i++) {
	otp.predict();
	otp.correct(b.backwardtrack.getmeasurements()[i],fr_no,emptyreg,emptyimg);
      }
      for (size_t fr_no=b.track.getminframeno(),i=0;fr_no<=b.track.getmaxframeno();fr_no++,i++) {
	otp.predict();
	otp.correct(b.track.getmeasurements()[i],fr_no,b.track.getregiondata()[i],b.track.img(i));
      }
    }
    int aidx_mid,bidx_mid,midframeno;
    static const float MINMERGEDIST;
    imgtrackproperties otp;
    bool mergetracks;
  };
  //####################################################
  //####################################################
}
#endif //GGPROPAGATING
