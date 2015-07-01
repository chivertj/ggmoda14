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
#ifndef ROBCOEFF
#define ROBCOEFF

#include "../globalCVHeader.h"

#include <iostream>
#include <algorithm>
#include <vector>

#include "utility.h"
#include <limits>

class robcoeff {
public:
  typedef std::vector<int> IDXS;

  void select(const IDXS points, IDXS &s_points);


  template <class T> void apply(cv::Mat_<T> &m, T (*func)(T)) {
    for (typename cv::Mat_<T>::iterator i=m.begin();i!=m.end();++i)
      (*i)=(*func)(*i);
  }

  template <class T> T selectsum(const cv::Mat_<T> &ipm, const IDXS &idxs) {
    assert(ipm.cols==1);
    T ttl=0.;
    for (size_t i=0;i<idxs.size();i++)
      ttl+=ipm(idxs[i],0);
    return ttl;
  }
  //@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@
  //~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
#if(1)
  template <class T> IDXS select_nonzero(const cv::Mat_<T> &ipm) {
    IDXS nonzero;
    for (size_t j=0;j<ipm.rows;j++) {
      if (ipm(j,0)>std::numeric_limits<T>::epsilon() || (std::numeric_limits<T>::is_signed && ipm(j,0)<-std::numeric_limits<T>::epsilon()))
  nonzero.push_back(j);
#if(0)
      else 
  if (rng(10)>7) 
    nonzero.push_back(j);
#endif
    }
    std::cout <<ipm.rows<<"->\t"<<nonzero.size()<<std::endl;
    return nonzero;
  }
#endif
  //~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
  //@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@
  template <class T> cv::Mat_<T> select(const cv::Mat_<T> &ipm, const IDXS &idxs) {
    assert(idxs.size()<=ipm.rows);
    //    if (idxs.size()!=ipm.rows)
    //      std::cout <<idxs.size()<<","<<ipm.rows<<std::endl;
    if (idxs.size()==ipm.rows) return ipm.clone();
    cv::Mat_<T> opm(idxs.size(),1);
    for (size_t i=0;i<idxs.size();i++)
      opm(i,0)=ipm(idxs[i],0);
    return opm;
  }
  template <class T> cv::Mat_<T> select_rows(const cv::Mat_<T> &ipm, const IDXS &idxs) {
    assert(idxs.size()<=ipm.rows);
    if (idxs.size()==ipm.rows) return ipm.clone();
    cv::Mat_<T> opm(idxs.size(),ipm.cols);
    for (size_t i=0;i<idxs.size();i++) {
      cv::Mat_<T> iprow=ipm.row(idxs[i]),oprow=opm.row(i);
      std::copy(iprow.begin(),iprow.end(),oprow.begin());
    }
    return opm;
  }
  template <class T> IDXS find(const cv::Mat_<T> &err, const T err_marg) {
    IDXS points;
    for (size_t j=0;j<err.rows;j++) {
      if (err(j,0)<=err_marg) 
  points.push_back(j);
    }
    return points;
  }
  //########################################################################
  //X data in columns, Xm mean, U basis vectors, L eigenvalues, 
  template <class T>
  cv::Mat_<T> is2fsrob(const cv::Mat_<T> &X, const cv::Mat_<T> &Xm, const cv::Mat_<T> &U, const cv::Mat_<T> &L, T k=48., T r=0.85, T s=24., T nh=5.) {
#if(0)
    std::cout <<"U:"<<std::endl<<U<<std::endl;
    std::cout <<"X:"<<std::endl<<cv::Mat_<T>(X.t())<<std::endl;
    std::cout <<"Xm:"<<std::endl<<cv::Mat_<T>(Xm.t())<<std::endl;
#endif
    T M=U.rows, K=U.cols;
    std::cout <<"WOULD HAVE BEEN:"<<k*K<<std::endl;
    T N=X.cols;
    cv::Mat_<T> A(K,N);
    A=0.;
    cv::Mat_<T> Acand(K,nh);
    Acand=0.;
    for (size_t j=0;j<N;j++) {
      const cv::Mat_<T> X_col=X.col(j);
      for (size_t i=0;i<nh;i++) {
  std::cout <<"\t genHypo..."<<std::endl;
  cv::Mat_<T> Acand_col=genHypo<T>(X_col,Xm,U,k,r,s);
  std::cout <<"\t copy..."<<std::endl;
  std::copy(Acand_col.begin(),Acand_col.end(),Acand.col(i).begin());
      }
      std::cout <<"selCoef..."<<std::endl;
      cv::Mat_<T> A_col=selCoef<T>(X_col,Xm,U,Acand,L);
      std::cout <<"Copy..."<<std::endl;
      std::copy(A_col.begin(),A_col.end(),A.col(j).begin());
    }
    std::cout <<"END is2fsrob"<<std::endl;
    return A;
  }
  //############################################
  //###################
  //B is a random selection of points from Xd=X-Xm (demeaned data from frow 
  //A is a random selection of rows from U (eigenvectors)
  //want to find sol from B=A*sol
  //where err=A*sol-B <= err_marg
  inline const int& minidx(const int &A, const int &B) { if (A>B) return B; return A; }
  template <class T>
  cv::Mat_<T> genHypo(const cv::Mat_<T> &X, const cv::Mat_<T> &Xm, const cv::Mat_<T> &U, T k=48., T r=0.85, T s=24.) {
    T M=U.rows,K=U.cols;
    cv::Mat Xd=X-Xm;

    IDXS nonzeropnts=select_nonzero(Xm);
    IDXS s_points;

    if (k*K<nonzeropnts.size()) {
      size_t s_points_size=std::floor(k*K);
      cv::Mat_<T> _s_points(s_points_size,1);
      s_points=IDXS(s_points_size);
      rng.fill(_s_points,cv::RNG::UNIFORM,cv::Scalar(0.),cv::Scalar(nonzeropnts.size()));
      for (size_t i=0;i<k*K;i++) 
  s_points[i]=nonzeropnts[_s_points(i,0)];
    }
    else
      s_points=nonzeropnts;
    
    T nbp=s_points.size();

    cv::Mat_<T> B(nbp,1);
    B=select<T>(Xd,s_points);

    cv::Mat_<T> A(nbp,K);
    A=select_rows<T>(U,s_points); //really row ? So eigen vectors in rows? no eigen vectors in columns but only copying parts of the eigenvectors...
    IDXS points(nbp);
    for (size_t i=0;i<nbp;i++)
      points[i]=i;
    cv::Mat_<T> sol;
    cv::solve(A,B,sol,cv::DECOMP_QR);
    cv::Mat_<T> err=cv::abs(A*sol-B); //cv::absdiff(A*sol,B); //

    std::cout <<"BEGIN LOOP"<<std::endl;
    while (nbp>s*K && B.rows!=points.size()) {
      cv::Mat_<T> errh;
      cv::sort(err,errh,CV_SORT_EVERY_COLUMN|CV_SORT_ASCENDING);
      //      std::cout <<"err:"<<cv::Mat_<T>(err.t())<<std::endl;

      std::cout <<"errh:"<<cv::Mat_<T>(errh.rowRange(0,minidx(10,errh.rows)).t())<<std::endl;
      T err_marg=errh(std::floor(r*nbp),0);
      //      std::cout <<err_marg<<",";
      points=find<T>(err,err_marg);
      select(points,s_points);
      B=select<T>(B,points);
      A=select_rows<T>(A,points);
      nbp=points.size();
      for (size_t i=0;i<nbp;i++)
  points[i]=i;
      cv::solve(A,B,sol,cv::DECOMP_QR);
      err=cv::abs(A*sol-B); //cv::absdiff(A*sol,B);
    }
    std::cout <<"END LOOP"<<std::endl;
    return sol;
  }

  //##########################################################
  template <class T>
  cv::Mat_<T> selCoef(const cv::Mat_<T> &X, const cv::Mat_<T> &Xm, const cv::Mat_<T> &U, const cv::Mat_<T> &Acand, const cv::Mat_<T> &L) {
    std::cout <<"selCoef BEGIN"<<std::endl;
    T k1=1.,k2=0.01;
    size_t K=Acand.rows,nh=Acand.cols;
    size_t N=L.rows;
    if (N<L.cols) N=L.cols;
    size_t M=X.rows;
    cv::Mat_<T> Xd=X-Xm;
    cv::Mat_<T> q=cv::Mat_<T>::zeros(nh,1);
    cv::Mat_<T> Y=U*Acand;
    std::cout <<"sum..."<<std::endl;
    std::cout <<L.rows<<","<<L.cols<<"\t"<<K<<"\t"<<N<<std::endl;
    T err_marg;
    if (K+1<=N)
      err_marg=cv::sum(L.rowRange(K+1,N)).val[0]*2./M;
    else
      err_marg=L(N-1,0)*2./M;
    std::cout <<"err_marg:"<<err_marg<<std::endl;
    //    err_marg=319;
    std::cout <<"loop..."<<std::endl;
    for (size_t i=0;i<nh;i++) {
      cv::Mat_<T> err;
      cv::pow(Y.col(i)-Xd,2.,err);
      IDXS points=find<T>(err,err_marg);
      std::cout <<"select sum..."<<std::endl;
      q(i,0)=k1*points.size()-k2*selectsum<T>(err,points);
      std::cout <<"end select sum..."<<std::endl;
    }
    cv::Mat ind;
    cv::sortIdx(q,ind,CV_SORT_EVERY_COLUMN|CV_SORT_ASCENDING);
    int i_sel=ind.at<int>(nh-1,0);
    std::cout <<"selCoef END"<<std::endl;
    return Acand.col(i_sel);
  }

  //members
  cv::RNG rng;

};

#endif //ROBCOEFF

