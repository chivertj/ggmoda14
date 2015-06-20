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
#include "region.h"
#include "ACDefinitions.h"
#include <fstream>

namespace region{
  void ReadImages(IREGv &imgvect, std::string imglistfile, bool iscolor) {
    std::fstream imglist(imglistfile.c_str());
    std::string imgfile;
    IREG img;
    int readflag=CV_LOAD_IMAGE_GRAYSCALE;
    if (iscolor)
      readflag=CV_LOAD_IMAGE_COLOR;
    //    while (imglist>>imgfile) { // && imgvect.size()<40) {
    while (std::getline(imglist,imgfile)) {
      //      std::cout <<imgfile<<std::endl;
      if ((img=cvLoadImage(imgfile.c_str(),readflag))!=0) {
	std::cout <<"Read:"<<imgfile<<std::endl;
	if (!iscolor)
	  cvThreshold(img,img,225,255,CV_THRESH_BINARY);
	imgvect.push_back(img);
      }
    }
  }
  //XXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXX
  void ReadImages(MREGv &imgvect, std::string imglistfile, bool iscolor) {
    std::fstream imglist(imglistfile.c_str());
    std::string imgfile;
    MREG img;
    int readflag=CV_LOAD_IMAGE_GRAYSCALE;
    if (iscolor)
      readflag=CV_LOAD_IMAGE_COLOR;
    while (std::getline(imglist,imgfile)) {
      img=cv::imread(imgfile.c_str(),readflag);
      if (!img.empty()) {
	std::cout <<"Read:"<<imgfile<<std::endl;
	if (!iscolor)
	  cv::threshold(img,img,225,255,CV_THRESH_BINARY);
	imgvect.push_back(img);
      }
      else throw std::exception();
    }
  }
  //XXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXX
  CvPoint CalcCenter(const REGIONPNTS &r) {
    assert(r.size()>0);
    float size=float(r.size());
    float sumx=0.f,sumy=0.f;
    for (uint i=0;i<r.size();i++) {
      sumx+=r[i].x;
      sumy+=r[i].y;
    }
    sumx/=size;
    sumy/=size;
    return  cvPoint(cvRound(sumx),cvRound(sumy));
  }
  //XXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXX
  void Align(const REGIONPNTS &m, REGIONS &d, int nd) {
    assert(d.size()>0);
    assert(m.size()>0);
  
    CvPoint mcent=CalcCenter(m);
    CvPoint center;
    for (int i=0;i<nd;i++) {
      center=CalcCenter(d[i]);
      Translate(d[i],mcent.x-center.x,mcent.y-center.y);
    }
  }
  //XXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXX
  void Translate(REGIONPNTS &r, int h, int v) {
    assert(r.size()>0);
    for (uint i=0;i<r.size();i++) {
      r[i].x+=h;
      r[i].y+=v;
    }
  }
  //XXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXX
  void SetBinImg(const REGIONPNTS &a, IREG img, bool clear) {
    assert(img);
    if (clear)
      cvZero(img);
    uchar *data=(uchar*)(img->imageData);
    int x,y,width=img->width,height=img->height;
    for (REGIONPNTS::const_iterator it=a.begin();it!=a.end();++it) {
      x=(*it).x; y=(*it).y;
      if (x>=0 && x<width && y>=0 && y<height)
	data[y*img->widthStep+x]=255;
    }
  }
  //XXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXX
  void ExtractPnts(const cv::Mat_<uchar> &img, REGIONPNTS &r,bool clear) {
    if (clear) r.clear();
    for (size_t j=0;j<img.rows;j++) {
      const uchar *raw=img.ptr<uchar>(j);
      for (size_t i=0;i<img.cols;i++) {
	if (raw[i]>0) 
	  r.push_back(cv::Point(i,j));
	  
      }
    }
  }
  //XXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXX
  std::ostream& operator<<(std::ostream &os, const IDXT &data) {
    for (int i=0;i<(int)data.size();i++)
      os <<data[i]<<"\t";
    return os;
  }
  void DestroyImages(IREGv &imgvect) {
    for (int i=0;i<imgvect.size();i++)
      cvReleaseImage(&imgvect[i]);
  }
  void ExportImages(const IREGv &imgvect, std::string basefilename, int pad) {
    for (int i=0;i<imgvect.size();i++) {
      std::string opfilename=basefilename+ACDefinitions::StringAndZeroPad(i,pad)+".jpg";
      ACDefinitions::ACSaveImg(imgvect[i],opfilename);
    }
  }
  void ExportImages(std::list<IplImage*> &imglist, std::string basefilename, int pad) {
    std::list<IplImage*>::iterator it1;
    int i;
    for (it1=imglist.begin(),i=0;it1!=imglist.end();++it1,i++) {
      std::string opfilename=basefilename+ACDefinitions::StringAndZeroPad(i,pad)+".jpg";
      ACDefinitions::ACSaveImg(*it1,opfilename);
    }
  }
  void ExportImage(const IplImage *img, const std::string &basefilename, int pad, int no) {
    std::string opfilename=basefilename+ACDefinitions::StringAndZeroPad(no,pad)+".jpg";
    ACDefinitions::ACSaveImg(img,opfilename);
  }
  void ExportImage(const cv::Mat &img, const std::string &basefilename, int pad, int no) {
    std::string opfilename=basefilename+ACDefinitions::StringAndZeroPad(no,pad)+".jpg";
    //    ACDefinitions::ACSaveImg(img,opfilename);
    cv::imwrite(opfilename,img);
  }

  CvPoint2D32f CalcSD(const REGIONPNTS &r, CvPoint center, int nopntstoinclude) {
    CvPoint2D32f sd;
    sd.x=0.; sd.y=0.;
    if (nopntstoinclude<=0)
      nopntstoinclude=r.size();
    for (uint i=0;i<nopntstoinclude;i++) {
      sd.x+=pow(r[i].x-center.x,2.);
      sd.y+=pow(r[i].y-center.y,2.);
    }
    sd.x/=float(nopntstoinclude);
    sd.y/=float(nopntstoinclude);
    return sd;
  }

  CvPoint MinXMinY(const REGIONPNTS &r, int nopntstoinclude) {
    CvPoint minxminy;
    minxminy.x=1e8; minxminy.y=1e8;
    if (nopntstoinclude<=0)
      nopntstoinclude=r.size();
    for (uint i=0;i<nopntstoinclude;i++) {
      if (r[i].x<minxminy.x)
	minxminy.x=r[i].x;
      if (r[i].y<minxminy.y)
	minxminy.y=r[i].y;
    }
    return  minxminy;
  }

  CvPoint MaxXMaxY(const REGIONPNTS &r, int nopntstoinclude) {
    CvPoint maxxmaxy;
    maxxmaxy.x=-1e8; maxxmaxy.y=-1e8;
    if (nopntstoinclude<=0)
      nopntstoinclude=r.size();
    for (uint i=0;i<nopntstoinclude;i++) {
      if (r[i].x>maxxmaxy.x)
	maxxmaxy.x=r[i].x;
      if (r[i].y>maxxmaxy.y)
	maxxmaxy.y=r[i].y;
    }
    return  maxxmaxy;
  }
  void MergePointSets(const cv::Mat &a, cv::Mat &b) {
#if(0)
    cv::Mat newpnts(a.rows+b.rows,a.cols,cv::DataType<T>::type);
    cv::Mat newpntsrange=newpnts.rowRange(0,a.rows);
    a.copyTo(newpntsrange);
    newpntsrange=newpnts.rowRange(a.rows,a.rows+b.rows);
    b.copyTo(newpntsrange);
    newpnts.copyTo(b);
#else
    const cv::Mat_<cv::Point2f> ta(a);
    cv::Mat_<cv::Point2f> tb(b);
//    AppendMatRowDir<cv::Point2f>(const cv::Mat_<cv::Point2f>(a),cv::Mat_<cv::Point2f>(b));
    AppendMatRowDir<cv::Point2f>(ta,tb);
#endif
  }
  cv::RotatedRect mergeRects(const cv::RotatedRect R1, const cv::RotatedRect R2) {
     CvPoint2D32f R1c[4],R2c[4];
     cvBoxPoints(R1,R1c);
     cvBoxPoints(R2,R2c);
     cv::Mat allpntsm(8,1,cv::DataType<cv::Point2f>::type);
     for (size_t i=0;i<4;i++) {
       allpntsm.at<cv::Point2f>(i,0)=R1c[i];
       allpntsm.at<cv::Point2f>(i+4,0)=R2c[i];
     }
//     return cv::minAreaRect(allpntsm);
     return Rects::makeRotatedRect<cv::Point2f>(allpntsm);
   }
}


