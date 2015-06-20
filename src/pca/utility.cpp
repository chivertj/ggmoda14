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
#include "utility.h"


std::ostream& operator<<(std::ostream& os, const cv::Mat &m) {
  for (int j=0;j<m.rows;j++) {
    for (int i=0;i<m.cols;i++) {
      os << m.at<double>(j,i) << ",";
    }
    os << std::endl;
  }
  return os;
}

std::ostream& operator<<(std::ostream& os, const cv::Size &s) {
  os <<s.height<<","<<s.width<<std::endl;
  return os;
}

namespace cvutils {
  //*******************************
  int getdims(const region::MREGv &imgvect, size_t &X, size_t &Y, size_t &N) {
    X=0; Y=0; N=0;
    int type;
    for (size_t i=0;i<imgvect.size();i++) {
      if (!imgvect[i].empty()) {
        if (X==0) {
          X=imgvect[i].cols;
          Y=imgvect[i].rows;
          type=imgvect[i].type();
        }
        else
          assert(X==imgvect[i].cols && Y==imgvect[i].rows && type==imgvect[i].type());
        N++;
      }
    }
    return type;
  }
  //*******************************
  int getdims(const region::MREGv &imgvect, size_t &X, size_t &Y, size_t &N, int &nchannels) {
    X=0; Y=0; N=0;
    int type;
    for (size_t i=0;i<imgvect.size();i++) {
      if (!imgvect[i].empty()) {
        if (X==0) {
          X=imgvect[i].cols;
          Y=imgvect[i].rows;
          type=imgvect[i].type();
          nchannels=imgvect[i].channels();
        }
        else
          assert(X==imgvect[i].cols && Y==imgvect[i].rows && type==imgvect[i].type() && nchannels==imgvect[i].channels());
        N++;
      }
    }
    return type;
  }
  //*******************************
  EMPTYIMGS getemptyimgs(const region::MREGv &imgvect) {
    EMPTYIMGS emptyimgs(imgvect.size());
    for (size_t i=0;i<imgvect.size();i++) {
      if (imgvect[i].empty())
        emptyimgs[i]=true;
      else
        emptyimgs[i]=false;
    }
    return emptyimgs;
  }
  //*******************************
#if(0)
  void insertemptyimgs(const EMPTYIMGS &emptyimgs, region::MREGv &imgvect) {
   bool change=false;
   region::MREGit imgit;
   size_t e;
   int nchanges=0;
   do {
     change=false;
     for (imgit=imgvect.begin(),e=0; imgit!=imgvect.end(); ++imgit,++e) {
       if (emptyimgs[e] && !imgit->empty()) {
         imgit=imgvect.insert(imgit,cv::Mat());
         change=true;
         nchanges++;
         break;
       }
     }
   } while(change);
   std::cout <<"insertemptyimgs nchanges:"<<nchanges<<std::endl;
  }
#else
  void insertemptyimgs(const EMPTYIMGS &emptyimgs, region::MREGv &imgvect) {
    if (imgvect.size()==emptyimgs.size())
      return;
    region::MREGv altimgvect(emptyimgs.size());
    for (size_t i=0,j=0;i<altimgvect.size();i++) {
      if (!emptyimgs[i]) {
        altimgvect[i]=imgvect[j];
        j++;
      }
    }
    imgvect=altimgvect;
  }
#endif
  //*******************************
  void removeemptyimgs(const region::MREGv &imgvect, region::MREGv &nonemptyimgvect, EMPTYIMGS &emptyimgs, bool copy) {
    if (emptyimgs.size()!=imgvect.size())
      emptyimgs=getemptyimgs(imgvect);
    int numnonemptyimgs=std::count(emptyimgs.begin(),emptyimgs.end(),false);
    nonemptyimgvect.resize(numnonemptyimgs);
    for (size_t i=0,j=0;i<imgvect.size();i++) {
      if (!imgvect.empty()) {
        if (copy)
          nonemptyimgvect[j]=imgvect[i].clone();
        else
          nonemptyimgvect[j]=imgvect[i];
        j++;
      }
    }
  }
  //*******************************

}

