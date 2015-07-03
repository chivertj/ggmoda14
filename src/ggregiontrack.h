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
#ifndef GGREGIONTRACK
#define GGREGIONTRACK

#include "globalCVHeader.h"
#include <deque>
#include "ggutils.h"
#include <iostream>
#include "ggtracker.h"
#include "ggframe.h"

#include <boost/uuid/uuid.hpp>
#include <boost/uuid/uuid_generators.hpp>
#include <boost/uuid/uuid_io.hpp>
#include <boost/functional/hash.hpp>
#include "gguuid.h"


namespace gg {
//############################################
//############################################
CvKalman* CloneKalman(const CvKalman *k);
//############################################
/**
 * Encapsulates deque container of CvKalman pointers.\n
 * Used by gg::exboxtrackproperties
 */
class KalmanMemory {
public:
  typedef std::deque<CvKalman*> KARRAY;
  KalmanMemory(void) {}
  KalmanMemory(const KalmanMemory &_km) {
    operator=(_km);
  }
  ~KalmanMemory(void) {
    this->release();
  }
  void store(CvKalman *k) {
    ks.push_back(CloneKalman(k));
  }
  const CvKalman* operator[] (int idx) const { if (idx<ks.size()) return ks[idx]; return 0; }
  KalmanMemory& operator=(const KalmanMemory &_km) {
    this->release();
    ks=KARRAY(_km.size());
    for (size_t i=0;i<ks.size();i++) {
      ks[i]=CloneKalman(_km[i]);
    }
    return *this;
  }
  size_t size(void) const { return ks.size(); }
  void clear(void) { release();}
  void reverse(void) { std::reverse(ks.begin(),ks.end()); }
protected:
  void release(void) {
    for (size_t i=0;i<ks.size();i++)
      cvReleaseKalman(&ks[i]);
    ks.clear();
  }
  KARRAY ks;
};
//############################################
/**
 * Encapsulates a container of float matrices \n
 * used by gg::exboxtrackproperties to store \n
 * tracking measurements, e.g. position.
 */
class Measurements {
public:
  typedef cv::Mat_<float> MSMNT;
  typedef std::deque<MSMNT> MARRAY;
  Measurements(size_t _N):N(_N) {}
  Measurements(const Measurements &_ms) {
    operator=(_ms);
  }
  ~Measurements(void) {}
  void store(const MSMNT &_m) {
    MSMNT m(N,1);
    _m.copyTo(m);
    ms.push_back(m);
  }
  size_t size(void) const { return ms.size(); }
  const MSMNT& operator[] (int idx) const {
    if (idx<ms.size()) return ms[idx];
    return emptym;
  }
  void clear(void) {
    ms.clear();
  }
  const Measurements& operator=(const Measurements &_ms) {
    this->clear();
    ms=MARRAY(_ms.size());
    N=_ms.N;
    for (size_t i=0;i<_ms.size();i++) {
      MSMNT m(N,1);
      _ms[i].copyTo(m);
      ms[i]=m;
    }
    return *this;
  }
  void reverse(void) { std::reverse(ms.begin(),ms.end()); }
protected:
  Measurements(void);
  MARRAY ms;
  size_t N;
  MSMNT emptym;
};
//############################################
//############################################
/**
 * Encapsulates point tracking with state estimation using CvKalman.
 */
class pnttrackproperties {
protected:
  CvKalman *k;
  float kvar;
  cv::Mat_<float> statepost;
  cv::Mat_<float> statepre;
public:
  void initkalman(const cv::Mat_<float> &initpnt) {
    cvSetIdentity( k->transition_matrix, cvRealScalar(1.) );
    //velocity
    cvmSet(k->transition_matrix,0,2,0.01);
    cvmSet(k->transition_matrix,1,3,0.01);
    //acceleration
    cvmSet(k->transition_matrix,2,4,0.01);
    cvmSet(k->transition_matrix,3,5,0.01);
    cvSetIdentity( k->measurement_matrix, cvRealScalar(1.) );
    //  mk::utils::opencv::PrintMat<float>(k->measurement_matrix);
    //cvSetIdentity( k->process_noise_cov, cvRealScalar(kvar*2.) ); //how believable is the model?
    cvSetIdentity( k->process_noise_cov, cvRealScalar(kvar) ); //how believable is the model?
    //cvSetIdentity( k->measurement_noise_cov, cvRealScalar(kvar) ); //how believable are the measurements? 5.
    cvSetIdentity( k->measurement_noise_cov, cvRealScalar(kvar/5.) ); //how believable are the measurements? 5.
    cvSetIdentity( k->error_cov_post, cvRealScalar(kvar) );
    cvSetIdentity( k->error_cov_pre, cvRealScalar(kvar) );
    cvZero(k->state_pre);
    cvZero(k->state_post);
    k->state_pre->data.fl[0]=k->state_post->data.fl[0]=initpnt(0,0);
    k->state_pre->data.fl[1]=k->state_post->data.fl[1]=initpnt(1,0);

    cvZero(k->gain);
    if (k->control_matrix)
      cvZero(k->control_matrix);
    cvZero(k->temp1);
    cvZero(k->temp2);
    cvZero(k->temp3);
    cvZero(k->temp4);
    cvZero(k->temp5);
  }
  pnttrackproperties(const pnttrackproperties &d) :
    k(cvCreateKalman(6,2)),
    statepost(k->state_post),
    statepre(k->state_pre)
  {
    this->operator=(d);
  }
  ~pnttrackproperties(void) {
    cvReleaseKalman(&k);
  }
  pnttrackproperties(void) :
    k(cvCreateKalman(6,2)),
    kvar(1.),
    statepost(k->state_post),
    statepre(k->state_pre)
  {
    initkalman(cv::Mat::zeros(2,1,CV_32F));
  }
  const pnttrackproperties& operator=(const pnttrackproperties &d) {
    kvar=d.kvar;

    cvCopy(d.k->transition_matrix,k->transition_matrix);
    cvCopy(d.k->measurement_matrix,k->measurement_matrix);
    cvCopy(d.k->process_noise_cov,k->process_noise_cov);
    cvCopy(d.k->measurement_noise_cov,k->measurement_noise_cov);
    cvCopy(d.k->error_cov_post,k->error_cov_post);
    cvCopy(d.k->error_cov_pre,k->error_cov_pre);
    cvCopy(d.k->state_pre,k->state_pre);
    cvCopy(d.k->state_post,k->state_post);

    cvCopy(d.k->gain,k->gain);
    if (d.k->control_matrix)
      cvCopy(d.k->control_matrix,k->control_matrix);
    cvCopy(d.k->temp1,k->temp1);
    cvCopy(d.k->temp2,k->temp2);
    cvCopy(d.k->temp3,k->temp3);
    cvCopy(d.k->temp4,k->temp4);
    cvCopy(d.k->temp5,k->temp5);

    return *this;
  }
  void predict(void) {
    cvKalmanPredict(k);
#if(0)
  const CvMat *tmpprediction=cvKalmanPredict(k);
  prediction=tmpprediction;
  std::cout <<"P:"<<prediction<<"("<<tmpprediction<<")"<<std::endl;
#endif
  }
  void correct(const cv::Mat &m) { //m, measurement cv::Mat_<float>(2,1)
    CvMat _m=CvMat(m);
    cvKalmanCorrect(k,&_m);
#if(0)
    const CvMat *tmpcorrection=cvKalmanCorrect(k,&_m);
    cv::Mat correction(tmpcorrection);
    std::cout <<"C:"<<correction<<"("<<tmpcorrection<<")"<<std::endl;
    std::cout <<"post:"<<statepost<<std::endl;
    std::cout <<"pre:"<<statepre<<std::endl;
    std::cout <<"P(1):"<<prediction<<std::endl;
#endif
  }
#if(0)
  cv::Mat prediction;
#endif
  const cv::Mat_<float>& getstatepost(void) { return statepost; }
  const cv::Mat_<float>& getstatepre(void) { return statepre; }
};
//############################################
//############################################
/** (Abstractly) Encapsulates an estimation (CvKalman) class         \n
 *
 *     State estimation matrix:         \n
 *    1    0    0    0    0    0.01 0    0    0    0    x  center  \n
 *    0    1    0    0    0    0    0.01 0    0    0    y  center  \n
 *    0    0    1    0    0    0    0    0.01 0    0    w  width   \n
 *    0    0    0    1    0    0    0    0    0.01 0    h  height  \n
 *    0    0    0    0    1    0    0    0    0    0.01 a  angle   \n
 *    0    0    0    0    0    1    0    0    0    0    dx         \n
 *    0    0    0    0    0    0    1    0    0    0    dy         \n
 *    0    0    0    0    0    0    0    1    0    0    dw         \n
 *    0    0    0    0    0    0    0    0    1    0    dh         \n
 *    0    0    0    0    0    0    0    0    0    1    da         \n
 */
class boxtrackproperties {
protected:
  CvKalman *k;
  float kvar;
  float k_dt;
  cv::Mat_<float> statepost;
  cv::Mat_<float> statepre;
  static const size_t nmeasure=5;
  static const size_t nstate=10;
#if(0)
  cv::Mat prediction;
#endif
  boost::uuids::uuid uid_tag;
public:
  boxtrackproperties(void);
  boxtrackproperties(const boxtrackproperties &d);
  ~boxtrackproperties(void) { cvReleaseKalman(&k); }
  virtual void initkalman(const cv::Mat_<float> &init);
  virtual const boxtrackproperties& operator=(const boxtrackproperties &d);
  virtual void predict(void);
  virtual void correct(const cv::Mat &m);
  virtual const cv::Mat_<float>& getstatepost(void) const { return statepost; }
  virtual const cv::Mat_<float>& getstatepre(void) const { return statepre; }
  size_t getnmeasure(void) const { return nmeasure; }
  size_t getnstate(void) const { return nstate; }
  const size_t uid_hash(void) const { boost::hash<boost::uuids::uuid> uuid_hasher; return uuid_hasher(uid_tag);}
  const std::string uid_str(void) const { boost::hash<boost::uuids::uuid> uuid_hasher; return boost::uuids::to_string(uid_tag); }
  virtual void clear(void)=0;
};
//############################################
//############################################
typedef int FRAMENO;
typedef std::deque<FRAMENO> FRAMENOS;
//############################################
/** Encapsulates tracking (gg::boxtrackproperties) history \n
 *  Remembers Kalman state info via gg::KalmanMemory;\n
 *  time instances (frame numbers gg::FRAMENOS); and\n
 *  tracking measurements (gg::Measurements).
 */
class exboxtrackproperties : public boxtrackproperties {
protected:
  size_t counter;
  KalmanMemory kmem;
  Measurements ms;
  virtual const exboxtrackproperties& operator++ (int);
  FRAMENOS framenos;
public:
  exboxtrackproperties(void);
  exboxtrackproperties(const exboxtrackproperties &d);
  virtual size_t count(void) const;
  virtual void initkalman(const cv::Mat_<float> &init, size_t frameno);
  virtual const exboxtrackproperties& operator=(const exboxtrackproperties &d);
  virtual void correct(const cv::Mat &m, size_t frameno);
  virtual const FRAMENOS& getframenos(void) const { return framenos; }
  gg::FRAMENO getminframeno(void) const {
    if (framenos.empty())
      return -1;
    if (framenos[0]<framenos[framenos.size()-1])
      return framenos[0];
    return framenos[framenos.size()-1];
  }
  gg::FRAMENO getmaxframeno(void) const {
    if (framenos.empty()) return -1;
    if (framenos[0]<framenos[framenos.size()-1])
      return framenos[framenos.size()-1];
    return framenos[0];
  }
  virtual const Measurements& getmeasurements(void) const { return ms; }
  virtual const KalmanMemory& getkalmanmem(void) const { return kmem; }
  virtual void clear(void) {
    kmem.clear();
    ms.clear();
    framenos.clear();
  }
};
//############################################
//############################################
typedef std::deque<frameregion> REGIONDATA;
//############################################
/**
 * Remembers tracking history:\n
 * tracker (CvKalman) state, (from base gg::exboxtrackproperties)\n
 * frame numbers, (from base gg::exboxtrackproperties)\n
 * and region data (gg::frameregion via the gg::REGIONDATA container).\n
 */
class objecttrackproperties : public exboxtrackproperties {
protected:
  REGIONDATA regiondata;
  bool reversed;
  frameregion nullregion;
public:
  objecttrackproperties(void) :
    exboxtrackproperties::exboxtrackproperties(),
    reversed(false)
  {}
  objecttrackproperties(const objecttrackproperties &d) :
    exboxtrackproperties::exboxtrackproperties(d),
    regiondata(d.regiondata),
    reversed(false)
  {
  }
  virtual void initkalman(const cv::Mat_<float> &init, size_t frameno, const frameregion &fr) {
    exboxtrackproperties::initkalman(init,frameno);
    regiondata.clear();
    regiondata.push_back(fr);
    reversed=false;
  }
  virtual const objecttrackproperties& operator=(const objecttrackproperties &d) {
    exboxtrackproperties::operator=(d);
    regiondata=d.regiondata;
    reversed=d.reversed;
    return *this;
  }
  virtual void correct(const cv::Mat &m, FRAMENO frameno, frameregion &fr) {
    exboxtrackproperties::correct(m,frameno);
    fr.correctboundingbox(region::Rects::measurementToRotatedRect(exboxtrackproperties::getstatepost()));
    regiondata.push_back(fr);
  }
  virtual void correct(const cv::Mat &m, FRAMENO frameno, const frameregion &fr) {
    exboxtrackproperties::correct(m,frameno);
    regiondata.push_back(fr);
  }
  const REGIONDATA& getregiondata(void) const { return regiondata; }
  const frameregion& operator[] (int idx) const { return regiondata[idx]; }
  frameregion& operator[] (int idx) { return regiondata[idx]; }
  const frameregion& getmostrecent(void) const { if (regiondata.size()==0) return nullregion; return regiondata[regiondata.size()-1]; }
  frameregion& getmostrecent(void) { if (regiondata.size()==0) return nullregion; return regiondata[regiondata.size()-1]; }
  virtual void reverse(void) {
    std::reverse(regiondata.begin(),regiondata.end());
    kmem.reverse();
    ms.reverse();
    std::reverse(framenos.begin(),framenos.end());
    reversed=!reversed;
  }
  bool isreversed(void) const {
    return reversed;
  }
  virtual size_t size(void) const { return regiondata.size(); }
  virtual void dump(std::ostream &os) const {
    os <<"*objecttrackproperties*"<<std::endl;
    os <<"UIDHASH:"<<boxtrackproperties::uid_hash()<<",";

    os <<"MINFRAMENO:"<<exboxtrackproperties::getminframeno()<<","
        <<"MAXFRAMENO:"<<exboxtrackproperties::getmaxframeno()<<",";

    os <<"framenos,uid_hash"<<",";
    for (size_t i=0;i<regiondata.size();i++)
      os <<framenos[i]<<":"<<regiondata[i].uid_hash()<<",";
    os <<std::endl;
  }
  virtual void clear(void) {
    exboxtrackproperties::clear();
    // clear individual frameregions?
    regiondata.clear();
    reversed=false;
  }
};
//############################################
//############################################
/**
 * Combines image information with the regional information\n
 * via inheritance from gg::objecttrackproperties.\n
 * Only keeps image data associated with already stored time instances.\n
 */
class imgtrackproperties : public objecttrackproperties {
protected:
  imgsequencedata imgdata;
public:
  imgtrackproperties(void) : objecttrackproperties::objecttrackproperties()  {}
  imgtrackproperties(const imgtrackproperties &d) : objecttrackproperties::objecttrackproperties(d),imgdata(d.imgdata) {}
  imgtrackproperties(const objecttrackproperties &d) : objecttrackproperties::objecttrackproperties(d) {}
  virtual const imgtrackproperties& operator=(const imgtrackproperties &d) {
    objecttrackproperties::operator=(d);
    imgdata=d.imgdata;
    return *this;
  }
  const imgtrackproperties& operator=(const objecttrackproperties &otp) {
    objecttrackproperties::operator=(otp);
  }
  virtual void initkalman(const cv::Mat_<float> &init, size_t frameno, const frameregion &fr, const cv::Mat &img) {
    objecttrackproperties::initkalman(init,frameno,fr);
    imgdata.clear();
    imgdata.add(img,frameno);
  }
  virtual void correct(const cv::Mat &m, size_t frameno, frameregion &fr, const cv::Mat &img) {
    objecttrackproperties::correct(m,frameno,fr);
    imgdata.add(img,frameno);
  }
  virtual void correct(const cv::Mat &m, size_t frameno, const frameregion &fr, const cv::Mat &img) {
    objecttrackproperties::correct(m,frameno,fr);
    imgdata.add(img,frameno);
  }
  virtual void reverse(void) {
    objecttrackproperties::reverse();
    imgdata.reverse();
  }
  const cv::Mat& img(size_t idx) const {
    if (idx>=imgdata.getimgs().size()) {
      std::ostringstream os;
      os <<"ggregiontrack::img error, idx:"<<idx<<" no imgs:"<<imgdata.getimgs().size()<<".";
      throw gg::error(os.str());
    }
    assert(idx<imgdata.getimgs().size());
    return imgdata.getimgs()[idx];
  }
  cv::Mat& img(size_t idx) {
    if (idx>=imgdata.getimgs().size()) {
      std::ostringstream os;
      os <<"ggregiontrack::img error, idx:"<<idx<<" no imgs:"<<imgdata.getimgs().size()<<".";
      throw gg::error(os.str());
    }
    assert(idx<imgdata.getimgs().size());
    return imgdata.getimgs()[idx];
  }
  const imgsequencedata& getimgdata(void) const { return imgdata; }
  imgsequencedata& getimgdata(void) { return imgdata; }
  void setdata(const objecttrackproperties &otp, const imgsequencedata &isd) {
    objecttrackproperties::operator=(otp);
    setimgdata(isd);
  }
  void setimgdata(const imgsequencedata &isd) {
    if (isd.isreversed())
      throw gg::error("ggregiontrack.hL344"); //not implemented
    imgdata.clear();
    for (size_t i=0,nxtimgidx=0,j;i<framenos.size();i++) {
      for (j=nxtimgidx;j<isd.size();j++) {
        if (framenos[i]==isd.gettimes()[j]) {
          //      imgdata.add(isd.getimgs()[j],isd.gettimes()[j]);
          imgdata.add(isd,j);
          nxtimgidx=j;
          break;
        }
      }
      if (j>nxtimgidx)
        throw gg::error("ggregiontrack.hL356"); //img data not available
    }
  }
  virtual void clear(void) {
    objecttrackproperties::clear();
    imgdata.clear();
  }
  virtual void dump(std::ostream &os) const {
    objecttrackproperties::dump(os);
  }
  virtual void dump(cv::VideoWriter &os) {
    std::cout <<"Dumping video: imgtrackproperties"<<std::endl;
    cv::Mat opwriteframe;
    for (size_t i=0;i<regiondata.size();i++) {
      if (regiondata[i].isvalid()) { //should be on i=0!
        opwriteframe=imgdata[i].clone();
        break;
      }
    }
    for (size_t i=0;i<regiondata.size();i++) {
      opwriteframe=cv::Scalar::all(0);
      if (regiondata[i].isvalid())
        cv::bitwise_and(imgdata[i],imgdata[i],opwriteframe,regiondata[i].binarytemp());
      os << opwriteframe;
    }
  }
};

}

#endif
