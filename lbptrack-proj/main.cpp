#include<vector>
#include<string>
#include<fstream>
#include<iostream>

#include <stdio.h>
#include <dirent.h>
#include <ios>
#include <stdexcept>
#include <opencv2/opencv.hpp>
#include <opencv2/highgui/highgui.hpp>
#include <opencv2/ml/ml.hpp>
#include <opencv2/objdetect/objdetect.hpp>

#include "src/lbphelpers.hpp"

#include "../src/region.h"

#include "../src/ggtracker.h"
#include "../src/ggframe.h"
#include "../src/ggtimewindow.h"
#include "../src/ggtrackerfuncs.h"
#include "../src/ggtracks.h"
#include "../mainhelperfuncs.h"
#include "../src/ggregiontracker.h"
#include "../src/ggtracks.h"
////////////////////////////////////////////////////////////////////////
int main (int argc, char ** argv) {
  if (argc<4) {
    cout <<"Requires: list of images in a text file (provide text file name), output base filename, image scale for detector >=1 <=1.3 (float)"<<endl;
    return -1;
  }
  CascadeClassifier cascade;

  //  hoghelpers::inithog("genfiles/initfile_0000.jpg",hog);
  //  hog.setSVMDetector(hoghelpers::readDescriptorVectorFromFile("genfiles/descriptorvector.dat"));
  string cascadeName="data/cascade.xml";
  cascade.load(cascadeName);

  string imglistfile(argv[1]);
  string opimgbasename(argv[2]);

  float cascadeimgscale=atof(argv[3]);

  int idx=0;
  fstream File;
  File.open(imglistfile.c_str());
  if (!File.good() || !File.is_open()) {
    cout <<"Unable to open image list file\n";
    File.close();
    return -1;
  }

  string imagefilename;
  getline(File,imagefilename);
  Mat img=imread(imagefilename,CV_LOAD_IMAGE_COLOR);
  if (img.empty()) {
    cout <<"Problem loading:"<<imagefilename<<endl;
    return -1;
  }

  Mat colorop=img.clone();
  IplImage c_colorop(colorop);
  gg::frameproperties frameprop(img.cols,img.rows,0);
  gg::frame framedata(frameprop);
  gg::timewindow trackerdata(frameprop);
  region::REGIONS trimdregs(region::MAXNOREGIONSB4TRIM);

  int imgcountidx=350;
  while (File && imgcountidx--) {
    //    std::cout <<imgcountidx<<"\t"<<idx<<std::endl;
    getline(File,imagefilename);
    if (idx>299 && imagefilename!="" && imagefilename!=" " && imagefilename!="\n") {
      cout <<"PROCESSING:"<<imagefilename<<endl;
      Mat img=imread(imagefilename,CV_LOAD_IMAGE_COLOR);
      if (!img.empty()) {
        img.copyTo(colorop);
        vector<Rect> regionrects=lbphelpers::detectrects(cascade,cascadeimgscale,img);
        int numregions=regionrects.size();
	if (numregions>region::MAXNOREGIONS) {
	  vector<Rect> trimmedrects;
	  size_t ntrimmedrects;
	  mainhelpers::TrimRegions<Rect>(regionrects,numregions,trimmedrects,ntrimmedrects);
	  regionrects=trimmedrects;
	  numregions=ntrimmedrects;
	  //	  regionrects.clear();
	}
	
        cout <<">NUMREGS:"<<numregions<<endl;
        cout <<"LoadFrame..."<<endl;
        gg::LoadFrame(regionrects,numregions,img,framedata);

        cout <<"DrawBoxes..."<<endl;
        mainhelpers::DrawBoxes(&c_colorop,framedata,cv::Scalar(0,255,255));

        cout <<"trackerdata.addframe(framedata)"<<endl;
        trackerdata.addframe(framedata);
        //        trackerdata.getgoodtracks()
        //        const gg::regiontracker::OBJTRACKS_T currenttracks=trackerdata.getcurrenttracks();

        //mainhelpers::DrawBoxes(&c_colorop,trackerdata.getcurrentframe(),cv::Scalar(255,255,0));
        //mainhelpers::DrawBoxes(&c_colorop,trackerdata.getcurrenttracks(),cv::Scalar(255,255,0));

        cout <<"lbphelpers::showDetections"<<endl;
        lbphelpers::showDetections(regionrects,colorop);

        cout <<"LabelRegions with UIDs"<<endl;
        if (numregions>0)
          mainhelpers::LabelRegions(&c_colorop,framedata,cv::Scalar(255,128,255));
        cout <<"ExportImage"<<endl;
        region::ExportImage(colorop,opimgbasename,4,idx);
      }
    }
    idx++;
  }
  File.close();

  trackerdata.finished();

  trackerdata.makeimgtracks();

  const gg::imgtracks &finishedimgtracksX=trackerdata.getimgtracks();
  std::cout <<"finishedimgtracksX.size()="<<finishedimgtracksX.size()<<std::endl;
  const gg::regiontracker::OBJTRACKS_T &finishedtracks=trackerdata.getfinishedtracks();
  //finishedtracks.
//  const gg::imgtracks &finishedtracks=trackerdata.getimgtracks();

//  trackerdata.getimgtracks();
  std::cout <<"Number of good tracks:"<<finishedtracks.size()<<std::endl;
  for (size_t i=0;i<finishedtracks.size();i++) {
    const gg::FRAMENOS &framenos=finishedtracks[i].getframenos();
    std::string trackuid=finishedtracks[i].uid_str();
    std::cout <<"Track "<<trackuid<<"("<<i<<")"<<" has size "<<framenos.size()<<std::endl;
    for (size_t j=0;j<framenos.size();j++)
      std::cout <<framenos[j]<<",";
    std::cout <<std::endl;
  }
  //gg::objtracks<gg::objecttrackproperties> goodobjtracks()
  gg::imgtracks finishedimgtracks(finishedtracks);
  std::cout <<"finishedimgtracks.size()="<<finishedimgtracks.size()<<std::endl;
  finishedimgtracks.dump();

  return 0;
}
////////////////////////////////////////////////////////////////////////

#if(0)
string imglistfile(argv[1]);
string opimgbasename(argv[2]);
int idx=0;
fstream File;
File.open(imglistfile.c_str());
if (!File.good() || !File.is_open()) {
  cout <<"Unable to open image list file\n";
  File.close();
  return -1;
}
while (File) {
  string imagefilename;
  getline(File,imagefilename);
  if (imagefilename!="" && imagefilename!=" " && imagefilename!="\n") {
    cout <<"PROCESSING:"<<imagefilename<<endl;
    Mat img=imread(imagefilename,CV_LOAD_IMAGE_COLOR);
    if (!img.empty()) {
      hoghelpers::detectTest(hog,hitThreshold,img);
      region::ExportImage(img,opimgbasename,4,idx);
    }
    idx++;
  }
}
File.close();
#endif

