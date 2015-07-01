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
#include "cvaux.h"
#include "cxmisc.h"
#include "src/globalCVHeader.h"
#include <iostream>


#include "src/bwlabel.h"
#include "src/region.h"
#include "src/csvfiling.h"
#include "src/gglpanalysis.h"

#include "src/ggtrackerfuncs.h"
#include "src/ggtracker.h"

#include "src/ggtimewindow.h"

#include <sstream>

#include "src/ggregiontracker.h"
#include "mainhelperfuncs.h"
using namespace mainhelpers;

#include "src/ggpropagating.h"
#include "src/ggtracks.h"

#include "src/ggregionprops.h"

#include "src/ggbgfgsegmenters.h"
#include "src/motion/ggmotionsegment.h"

#include "src/ggclassification.h"
const int gg::discreteclassification::NOCLASSIFICATION;
const int gg::helmetclassification::HELMET;
const int gg::helmetclassification::NOHELMET;

#include "src/ggexceptions.h"
#include "src/ggutils.h"

void dump_uidhash(const gg::frame &framedata);

int main(int argc, char** argv)
{
  IplImage*       queryframe = NULL,*origqueryframe=NULL;
    CvCapture*      cap = NULL;
    if( argc < 2 )
    {
        printf("please specify video file name \n");
        exit(0);
    }
    cap = cvCaptureFromFile(argv[1]);
    origqueryframe = cvQueryFrame(cap);
    if(!origqueryframe)
    {
        printf("bad video \n");
        exit(0);
    }
    //    cvCopy(origqueryframe,queryframe);
//    queryframe=cvCloneImage(origqueryframe);
    float scale=0.75;
    if (argc==6)
      scale=std::atof(argv[5]);
    queryframe=cvCreateImage(
//          cvSize(origqueryframe->width/2.,origqueryframe->height/2.),
        cvSize(origqueryframe->width*scale,origqueryframe->height*scale),
          origqueryframe->depth,
          origqueryframe->nChannels);

    IplImage *fgmask=0;
    if (argc==5 && argv[4]!="NOMASK" && !(fgmask=cvLoadImage(argv[4],CV_LOAD_IMAGE_GRAYSCALE))) {
      printf("bad mask:%s\n",argv[4]);
      exit(0);
    }

    cvNamedWindow( "Raw", 1 );
    cvNamedWindow( "ConnectComp",1);

    //create BG model
    //    gg::MakeBGFGSegmenterCodeBook bgfg_segmenter(queryframe->width,queryframe->height);
//    gg::MakeBGFGSegmenterStatModel bgfg_segmenter(queryframe);
    gg::MakeBGFGSegmenterLKMotion bgfg_segmenter(queryframe->width,queryframe->height);
    //    gg::MakeHeadClassifier headclass(argv[2]);
    CvVideoWriter *videoout=cvCreateVideoWriter(argv[3],CV_FOURCC('x','v','i','d'),25,cvGetSize(queryframe),1);
    CvVideoWriter *binaryout=cvCreateVideoWriter("binaryvideo.avi",CV_FOURCC('x','v','i','d'),25,cvGetSize(queryframe),1);
    CvVideoWriter *labelvidout=cvCreateVideoWriter("colorlabel.avi",CV_FOURCC('x','v','i','d'),25,cvGetSize(queryframe),1);
    IplImage *colorop=cvCloneImage(queryframe);
    IplImage *tmpfgmap=cvCreateImage(cvGetSize(queryframe),IPL_DEPTH_8U,1);//cvCloneImage(helmetfgmap);
    IplImage *ttlfgmap=cvCloneImage(tmpfgmap);
    IplImage *binaryimg=cvCloneImage(colorop); //color for the output video format - seems to be better

    IplImage *trimdregsImgs=cvCloneImage(tmpfgmap);

    region::REGIONS trimdregs(region::MAXNOREGIONSB4TRIM); // gg::_MAXNREGIONS);

    CBWLabel labeller;
    labeller.Init(ttlfgmap);

    gg::frameproperties frameprop(queryframe->width,queryframe->height,0);
    gg::frame framedata(frameprop);

    gg::timewindow trackerdata(frameprop);


    size_t trimdnregs=0,fr;
    for(fr = 1;origqueryframe; origqueryframe = cvQueryFrame(cap), fr++ ) {
//      cvCopy(origqueryframe,queryframe);
      cvResize(origqueryframe,queryframe);
      cvCopy(queryframe,colorop);
      std::cout <<"*****************************************"<<std::endl;
      std::cout <<"FRAME:"<<fr<<" processing"<<std::endl;
      std::cout <<"*****************************************"<<std::endl;
      //###################
      if (bgfg_segmenter(queryframe)) {// && fr>1500) {
  cvCopy(bgfg_segmenter.Get(),ttlfgmap);
  if (fgmask) 
    cvAnd(ttlfgmap,fgmask,ttlfgmap);

  int numregions=labeller.CalcRegions();
  std::cout <<"NUMREGS:"<<numregions<<std::endl;
  //###############################
  labeller.MakeCLabelImg();
  cvWriteFrame(labelvidout,labeller.BorrowCLabelImg());
  //###############################
  region::REGIONS regs=labeller.BorrowRegions();
  TrimRegions(regs,numregions,trimdregs,trimdnregs);
  std::cout <<"trimdnregs:"<<trimdnregs<<std::endl;
  gg::LoadFrame(trimdregs,trimdnregs,queryframe,framedata);

  std::cout <<"Before Add Frame:";
  dump_uidhash(framedata);

        mainhelpers::DrawBoxes(colorop,framedata,cv::Scalar(100,100,255 ));
        trackerdata.addframe(framedata);
        mainhelpers::DrawBoxes(colorop,trackerdata.getcurrentframe(),cv::Scalar(255,255,0));

        std::cout <<"After Add Frame:";
        dump_uidhash(framedata);

  cvZero(trimdregsImgs);
  for (size_t tx=0;tx<trimdnregs;tx++) 
    region::SetBinImg(trimdregs[tx],trimdregsImgs,false);
  
  FGOutlines(trimdregsImgs,colorop,cvScalar(255,10,255),false);
  bgfg_segmenter.render(colorop);
  mainhelpers::LabelRegions(colorop,framedata,cv::Scalar(255,128,255));
      }
      cvWriteFrame(videoout,colorop);
      cvCvtColor(ttlfgmap,binaryimg,CV_GRAY2BGR);
      cvWriteFrame(binaryout,binaryimg);
      cvShowImage( "Raw", colorop );
      cvShowImage( "ConnectComp",bgfg_segmenter.Get());

      if (!trackerdata.getfinishedtracks().empty()) {
        trackerdata.getfinishedtracks().dump();
        trackerdata.clearfinishedtracks();
      }

      char k = cvWaitKey(2);
      if( k == 27 ) break;
      std::cout <<"FRAME:"<<fr<<" FINISHED"<<std::endl;
      std::cout <<"*****************************************"<<std::endl;
    }

    trackerdata.finished();
    if (!trackerdata.getfinishedtracks().empty()) {
      trackerdata.getfinishedtracks().dump();
      trackerdata.clearfinishedtracks();
    }



    cvReleaseImage(&trimdregsImgs);

    cvReleaseImage(&binaryimg);
    cvReleaseImage(&ttlfgmap);
    cvReleaseImage(&tmpfgmap);
    cvReleaseImage(&colorop);
    cvReleaseVideoWriter(&videoout);
    cvReleaseVideoWriter(&binaryout);
    cvReleaseCapture(&cap);
    cvReleaseImage(&fgmask);
    cvReleaseImage(&queryframe);

    cvDestroyAllWindows();

    return 0;
}

void dump_uidhash(const gg::frame &framedata) {
  for (size_t fdidx=0;fdidx<framedata.size();fdidx++)
    std::cout <<framedata[fdidx].uid_hash()<<",";
  std::cout <<std::endl;
}

