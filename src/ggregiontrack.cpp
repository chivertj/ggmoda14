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
#include "ggregiontrack.h"
#include "gguuid.h"
//extern boost::mt19937 ran;

namespace gg {
  CvKalman* CloneKalman(const CvKalman *k) {
    CvKalman *newk=cvCreateKalman(k->DP,k->MP,k->CP);
    cvCopy(k->transition_matrix,newk->transition_matrix);
    cvCopy(k->measurement_matrix,newk->measurement_matrix);
    cvCopy(k->process_noise_cov,newk->process_noise_cov);
    cvCopy(k->measurement_noise_cov,newk->measurement_noise_cov);
    cvCopy(k->error_cov_post,newk->error_cov_post);
    cvCopy(k->error_cov_pre,newk->error_cov_pre);
    cvCopy(k->state_pre,newk->state_pre);
    cvCopy(k->state_post,newk->state_post);      

    cvCopy(k->gain,newk->gain);
    if (k->control_matrix)
      cvCopy(k->control_matrix,newk->control_matrix);
    cvCopy(k->temp1,newk->temp1);
    cvCopy(k->temp2,newk->temp2);
    cvCopy(k->temp3,newk->temp3);
    cvCopy(k->temp4,newk->temp4);
    cvCopy(k->temp5,newk->temp5);
    return newk;
  }

  //#################################
  //#################################
  //boxtrackproperties
  //#################################
  //#################################
  boxtrackproperties::
  boxtrackproperties(const boxtrackproperties &d) :
    k(cvCreateKalman(nstate,nmeasure)),
    statepost(k->state_post),
    statepre(k->state_pre),
    uid_tag(d.uid_tag)
  {
    this->operator=(d);
  }
  boxtrackproperties::
  boxtrackproperties(void) :
    k(cvCreateKalman(nstate,nmeasure)),
    kvar(9.),
    //      k_dt(0.067),
    k_dt(0.1),
    statepost(k->state_post),
    statepre(k->state_pre),
    uid_tag(boost::uuids::random_generator(ran)())
  {
    initkalman(cv::Mat::zeros(nmeasure,1,CV_32F));
  }
  void   boxtrackproperties::
  initkalman(const cv::Mat_<float> &init) { //init 5x1
    cvSetIdentity( k->transition_matrix, cvRealScalar(1.) );
    //rates of change
#if(0)
      //center
    cvmSet(k->transition_matrix,0,5,k_dt*5.);
    cvmSet(k->transition_matrix,1,6,k_dt*5.);
      //size
    cvmSet(k->transition_matrix,2,7,k_dt*0.1);
    cvmSet(k->transition_matrix,3,8,k_dt*0.1);
      //angle
    cvmSet(k->transition_matrix,4,9,k_dt*0.05);
#else
    //center
  cvmSet(k->transition_matrix,0,5,k_dt*10.);
  cvmSet(k->transition_matrix,1,6,k_dt*10.);
    //size
  cvmSet(k->transition_matrix,2,7,k_dt);
  cvmSet(k->transition_matrix,3,8,k_dt);
    //angle
  cvmSet(k->transition_matrix,4,9,k_dt*0.5);

#endif

    cvSetIdentity( k->measurement_matrix, cvRealScalar(1.) );
    cvSetIdentity( k->process_noise_cov, cvRealScalar(1e-4) ); //1. //how believable is the model? //0.5
    cvSetIdentity( k->measurement_noise_cov, cvRealScalar(1e-1)); //how believable are the measurements? 5. //5.

    cvSetIdentity( k->error_cov_post, cvRealScalar(1.) );
    cvSetIdentity( k->error_cov_pre, cvRealScalar(1.) );
    cvZero(k->state_pre);
    cvZero(k->state_post);
    for (size_t i=0; i<nmeasure; i++) 
      k->state_pre->data.fl[i]=k->state_post->data.fl[i]=init(i,0);
    cvZero(k->gain);
    if (k->control_matrix)
      cvZero(k->control_matrix);
    cvZero(k->temp1);
    cvZero(k->temp2);
    cvZero(k->temp3);
    cvZero(k->temp4);
    cvZero(k->temp5);

    uid_tag=boost::uuids::random_generator(ran)();
  }
  const boxtrackproperties&   boxtrackproperties::
  operator=(const boxtrackproperties &d) {
    kvar=d.kvar;
    k_dt=d.k_dt;
    
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

    uid_tag=d.uid_tag;

    return *this;
  }
  void   boxtrackproperties::
  predict(void) {
    cvKalmanPredict(k);
  }
  void   boxtrackproperties::
  correct(const cv::Mat &m) { //m, measurement cv::Mat_<float>(5,1)
    CvMat _z=CvMat(m);
    cvKalmanCorrect(k,&_z);
  }
  //#################################
  //#################################
  //boxtrackproperties end
  //#################################
  //#################################

  //#################################
  //#################################
  //exboxtrackproperties
  //#################################
  //#################################
  const exboxtrackproperties& exboxtrackproperties::operator++ (int) { 
    counter++; 
    return *this;
  }
  exboxtrackproperties::exboxtrackproperties(void) : 
    exboxtrackproperties::boxtrackproperties(),
    counter(0),
    ms(nmeasure) 
  {}
  exboxtrackproperties::exboxtrackproperties(const exboxtrackproperties &d) : 
    exboxtrackproperties::boxtrackproperties(d),
    counter(d.counter),
    kmem(d.kmem),
    ms(d.ms),
    framenos(d.framenos) 
  {}
  size_t exboxtrackproperties::count(void) const { return counter; }
  void exboxtrackproperties::initkalman(const cv::Mat_<float> &init, size_t frameno) { 
    boxtrackproperties::initkalman(init); 
    counter=0; 
    ms.clear(); 
    kmem.clear(); 
    framenos.clear();
    ms.store(init);
    kmem.store(k);
    framenos.push_back(frameno);
    (*this)++;
  }
  const exboxtrackproperties& exboxtrackproperties::operator=(const exboxtrackproperties &d) {
    boxtrackproperties::operator=(d);
    counter=d.counter;
    kmem=d.kmem;
    ms=d.ms;
    framenos=d.framenos;
  }
  void exboxtrackproperties::correct(const cv::Mat &m, size_t frameno) { 
    ms.store(m);
    boxtrackproperties::correct(m); 
    (*this)++; 
    kmem.store(k); 
    framenos.push_back(frameno); 
  }
  //#################################
  //#################################
  //exboxtrackproperties end
  //#################################
  //#################################
}
