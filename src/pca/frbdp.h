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
#ifndef FRBDP_NS
#define FRBDP_NS

#include "../globalCVHeader.h"
#include <iostream>
#include "xjlinalg.h"
#include "utility.h"

namespace frbdp {

  template <class T> struct eig_sys {
    eig_sys(const cv::Mat_<T> &_U, const cv::Mat_<T> &_D, const cv::Mat_<T> &_mu, const size_t _n, const cv::Mat_<T> &_C) : U(_U),D(_D),mu(_mu),n(_n),C(_C) {}
    eig_sys(void) {}
    cv::Mat_<T> U;
    cv::Mat_<T> D;
    cv::Mat_<T> mu;
    size_t n;
    cv::Mat_<T> C;
  };

  template<class T> class incASM {
  public:
    incASM(const eig_sys<T> &now, const cv::Mat_<T> &s, eig_sys<T> &nxt) {this->operator()(now,s,nxt);}
    void operator() (const eig_sys<T> &now, const cv::Mat_<T> &s, eig_sys<T> &nxt) {
      //project shape s to current eigenspace
      c=now.U.t()*(s-now.mu);
      
      //reconstruct shape
      _s=now.U*c+now.mu;

      //compute residual vector
      r=s-_s;

      //append r as new basis vector to U
      double rnorm=cv::norm(r);
      r*=rnorm;
      _U=xjlinalg::appendcols(now.U,r);

      //determine the coefficients in the new basis
      _C=xjlinalg::tile(now.C,c,cv::Mat::zeros(1,now.C.cols,cv::DataType<T>::type),cv::Mat_<T>::ones(1,1)*T(rnorm));

      //perform PCA on C' and obtain the mean u'', the eignevectors U'' and the eigenvalues lambda''
      pca(_C,cv::Mat(),CV_PCA_DATA_AS_COL,_C.cols);
      
      //project coefficients to new basis
      meanrep=cv::repeat(pca.mean,1,_C.cols);

      nxt.C=pca.eigenvectors*(_C-meanrep);

      //rotate subspace
      nxt.U=_U*pca.eigenvectors.t();

      //update mean
      nxt.mu=now.mu+_U*pca.mean;

      //update eigenvalues
      nxt.D=pca.eigenvalues;
    }

    //######
    //members
    cv::Mat_<T> c;
    cv::Mat_<T> _s;
    cv::Mat_<T> _U;
    cv::Mat_<T> r;
    cv::Mat_<T> _C;
    cv::Mat_<T> _U_;

    cv::PCA pca;
    cv::Mat_<T> eigenvectorsT;
    cv::Mat_<T> meanrep;
  };
}

template <class T> std::ostream& operator<<(std::ostream &os, const frbdp::eig_sys<T> &e) {
  std::cout <<"***********"<<std::endl;
  std::cout <<"eig_sys::U"<<std::endl;
  std::cout <<e.U;
  std::cout <<"eig_sys::D"<<std::endl;
  std::cout <<e.D;
  std::cout <<"eig_sys::mu"<<std::endl;
  std::cout <<e.mu;
  std::cout <<"eig_sys::n"<<std::endl<<e.n<<std::endl;
  std::cout <<"eig_sys::C"<<std::endl;
  std::cout <<e.C;
  std::cout <<"***********"<<std::endl;
  return os;
}

#endif //FRBDP_NS
