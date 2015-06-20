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
#include "../src/globalCVHeader.h"
#include <iostream>


#include "../src/bwlabel.h"
#include "../src/region.h"
#include "../src/csvfiling.h"
#include "../src/gglpanalysis.h"
#include "../src/ggheadtracker.h"

#include "../src/ggtrackerfuncs.h"
#include "../src/ggtracker.h"

#include "../src/ggtimewindow.h"

#include <sstream>

#include "../src/ggregiontracker.h"
#include "../mainhelperfuncs.h"
using namespace mainhelpers;

#include "../src/ggpropagating.h"
#include "../src/ggtracks.h"

#include "../src/ggregionprops.h"

const int gg::discreteclassification::NOCLASSIFICATION;
const int gg::helmetclassification::HELMET;
const int gg::helmetclassification::NOHELMET;

#include "../src/ggbgfgsegmenters.h"
#include "../src/ggregiontrack.h"

int main(int argc, char** argv)
{
    IplImage*       queryframe = NULL,*origqueryframe=NULL;
    CvCapture*      cap = NULL;
#if(1)
    if( argc < 2 ) {
        printf("please specify video file name \n");
        exit(0);
    }
    cap = cvCaptureFromFile(argv[1]);
#else
    cap = cvCreateCameraCapture(0);
#endif
    origqueryframe = cvQueryFrame(cap);
    if(!origqueryframe)
    {
        printf("bad video \n");
        exit(0);
    }
#if(1)
    queryframe=cvCloneImage(origqueryframe);
#else
    queryframe=cvCreateImage(
          cvSize(origqueryframe->width*0.5,origqueryframe->height*0.5),
          origqueryframe->depth,
          origqueryframe->nChannels);
#endif

    IplImage *fgmask=0;
    if (argc==5 && !(fgmask=cvLoadImage(argv[4],CV_LOAD_IMAGE_GRAYSCALE))) {
      printf("bad mask:%s\n",argv[4]);
      exit(0);
    }

    cvNamedWindow( "Raw", 1 );
    cvNamedWindow( "ConnectComp",1);

    //create BG model
    //    gg::MakeBGFGSegmenterCodeBook bgfg_segmenter(queryframe->width,queryframe->height);
    gg::MakeBGFGSegmenterStatModel bgfg_segmenter(queryframe);
    gg::MakeHeadClassifier headclass(argv[2]);
    gg::LPAnalysis featureext;

    CvVideoWriter *videoout=cvCreateVideoWriter(argv[3],CV_FOURCC('x','v','i','d'),25,cvGetSize(queryframe),1);
    IplImage *colorop=cvCloneImage(queryframe);
    IplImage *tmpfgmap=cvCreateImage(cvGetSize(queryframe),IPL_DEPTH_8U,1);
    IplImage *ttlfgmap=cvCreateImage(cvGetSize(queryframe),IPL_DEPTH_8U,1);

    IplImage *trimdregsImgs=cvCreateImage(cvGetSize(queryframe),IPL_DEPTH_8U,1);

    CBWLabel labeller;
    labeller.Init(ttlfgmap);
    gg::Heads heads;

    gg::frameproperties frameprop(queryframe->width,queryframe->height,0);
    gg::frame framedata(frameprop);
    gg::timewindow trackerdata(frameprop);

    region::REGIONS trimdregs(region::MAXNOREGIONSB4TRIM); // gg::_MAXNREGIONS);
    size_t trimdnregs=0,fr;
    for(fr = 1;origqueryframe; origqueryframe = cvQueryFrame(cap), fr++ ) {
//      cvCopy(origqueryframe,queryframe);
      cvResize(origqueryframe,queryframe);
      cvCopy(queryframe,colorop);
      std::cout <<"*****************************************"<<std::endl;
      std::cout <<"FRAME NO:"<<fr<<std::endl;
      //###################
      if (bgfg_segmenter(queryframe)) {
        cvCopy(bgfg_segmenter.Get(),ttlfgmap);
	if (fgmask){
	  cvCopy(bgfg_segmenter.Get(),ttlfgmap);
	  cvAnd(ttlfgmap,fgmask,ttlfgmap);
	}
	
	int numregions=labeller.CalcRegions();
	std::cout <<">NUMREGS:"<<numregions<<std::endl;

	region::REGIONS regs=labeller.BorrowRegions();
	TrimRegions(regs,numregions,trimdregs,trimdnregs);
	gg::LoadFrame(trimdregs,trimdnregs,queryframe,framedata);
//	mainhelpers::DrawBoxes(colorop,framedata);
        mainhelpers::DrawBoxes(colorop,framedata,cv::Scalar(0,255,255));
	trackerdata.addframe(framedata);
//	mainhelpers::DrawBoxes(colorop,trackerdata.getcurrentframe(),cv::Scalar(255,255,0));

	cvZero(trimdregsImgs);
	for (size_t tx=0;tx<trimdnregs;tx++) 
	  region::SetBinImg(trimdregs[tx],trimdregsImgs,false);
	
	FGOutlines(trimdregsImgs,colorop,cvScalar(255,255,255),false);
	if (trimdnregs>0)
	  LabelRegions(colorop,framedata,cv::Scalar(255,128,255));
      }

      cvWriteFrame(videoout,colorop);
      cvShowImage( "Raw", colorop );
      cvShowImage( "ConnectComp",bgfg_segmenter.Get());
//################################################################
      std::cout <<"*****FINISHED TRACKS**********"<<std::endl;
      if (!trackerdata.getfinishedtracks().empty()) {
        std::cout <<"making motorbikeimgtracks"<<std::endl;
        gg::motorbikeimgtracks mitracks(trackerdata.getfinishedtracks());
        std::cout <<"making individual classifications"<<std::endl;
        mitracks.individualclassification(headclass.Get());
        std::cout <<"classifying each track"<<std::endl;
        mitracks.classify();
        mitracks.dump();
        std::cout <<"clearing finished tracks"<<std::endl;
        trackerdata.clearfinishedtracks();
      }
      std::cout <<"*****END FINISHED TRACKS**********"<<std::endl;
//###############################################################
      char k = cvWaitKey(2);
      if( k == 27 ) break;
      std::cout <<"END FRAME NO:"<<fr<<std::endl;
      std::cout <<"*****************************************"<<std::endl;
    }

//    gg::motorbiketracks<gg::objecttrackproperties> goodtracks(trackerdata.getgoodtracks());
    gg::motorbiketracks<gg::imgtrackproperties> goodtracks(trackerdata.getgoodtracks());
    std::cout <<"No of good tracks:"<<goodtracks.size()<<std::endl;
    for (size_t i=0;i<goodtracks.size();i++) {
      std::cout <<"Good track:"<<i<<std::endl;
      const gg::FRAMENOS &framenos=goodtracks[i].getframenos();
      std::cout <<"frame no size:"<<framenos.size()<<std::endl;
      for (size_t j=0;j<framenos.size();j++)  
	std::cout <<framenos[j]<<",";
      std::cout <<std::endl;
    }

    cvReleaseImage(&trimdregsImgs);
    cvReleaseImage(&ttlfgmap);
    cvReleaseImage(&tmpfgmap);
    cvReleaseImage(&colorop);
    cvReleaseVideoWriter(&videoout);
    cvReleaseCapture(&cap);
    cvReleaseImage(&fgmask);
    cvReleaseImage(&queryframe);

    return 0;
}

