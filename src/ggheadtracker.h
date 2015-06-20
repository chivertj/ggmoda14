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
#ifndef GGHEADTRACKER_NS
#define GGHEADTRACKER_NS

#include "globalCVHeader.h"
#include "cxmisc.h"
#include "ml.h"

#include "csvfiling.h"
#include "region.h"

namespace gg {
  class Heads {
  protected:
    //float headproportion; //=0.1;
    const float minheadsize; //=50, //20 before 29th January -> from tracker0.016?
    const float maxheadsize; //=100*100;
    const float xyratio; //=0.4; //0.22 before 29th January -> from tracker0.016?
    const float minregsize;
    region::Rects rects;
  public:
    Heads(void);
    void Find(std::vector<size_t> &heads, std::vector<float> &headproportions, const region::REGIONS &regs, size_t numregions) const;
    void Make(const region::REGIONPNTS &allpnts, float headproportion, region::REGIONPNTS &headpnts) const;
    bool isHead(const region::REGIONPNTS &reg, float &headproportion) const;
  };
  //#################################
  class HeadClassifier {
  public:
    HeadClassifier(void) {}
    ~HeadClassifier(void) {}
    //features in rows
    //labels in rows
    void train(const cv::Mat &trainingdata, const cv::Mat &labels);
    //feature in row
    float classify(const cv::Mat &feature) const;
  protected:
    CvSVM svm;
    CvSVMParams params;
    CvTermCriteria criteria;
  };
  //#################################
  class MakeHeadClassifier {
  public:
    MakeHeadClassifier(const std::string &trainfilename);
    ~MakeHeadClassifier(void) {}
    const HeadClassifier& Get(void) const {return headclass;}
    HeadClassifier& Get(void) { return headclass; }
    const size_t GetNoAttributes(void) const {return ATTRIBUTES_PER_SAMPLE;}
  protected:
    MakeHeadClassifier(void) {}
    HeadClassifier headclass;
    cv::Mat training_data,training_classifications;
    cv::Mat_<float> csvtraindata;
    size_t NUMBER_OF_TRAINING_SAMPLES,ATTRIBUTES_PER_SAMPLE;
  };
  //################
  //################
}
#endif // GGHEADTRACKER_NS
