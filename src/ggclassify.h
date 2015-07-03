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
#ifndef GGCLASSIFY
#define GGCLASSIFY

#include "globalCVHeader.h"
#include "ggheadtracker.h"
#include "ggregionhierarchy.h"
#include "ggclassification.h"

namespace gg {
//############################################
//############################################
/**
 *  Abstract base class for classifiers.
 */
class classifier {
public:
  classifier(void) {}
  virtual ~classifier(void) {}
  virtual void train(const cv::Mat &trainingdata, const cv::Mat &labels) =0;
  //    virtual const discreteclassification& classify(const cv::Mat &feature) =0;
  virtual const discreteclassification& classify(regionhierarchy &reg) =0;
protected:
  discreteclassification c;
};
//############################################
//############################################
/**
 * Classifier specialization for helmets.\n
 * gg::helmetclassifier::c = 0 (gg::discreteclassification) for no classification\n
 * gg::helmetclassifier::c = 1 (gg::discreteclassification) for helmet present\n
 * gg::helmetclassifier::c = 2 (gg::discreteclassification) for helmet not present\n
 */
class helmetclassifier : public classifier {
public:
  helmetclassifier(const HeadClassifier &_headclassifier) : headclassifier(_headclassifier) {}
  virtual ~helmetclassifier(void) {}
  //    const discreteclassification& classify(const cv::Mat &feature) {
  virtual const discreteclassification& classify(regionhierarchy &reg) {
    float res=headclassifier.classify(reg.getsubregprops().getphoto().getpdf());
    if (res>0.0001)
      c.set(helmetclassification::HELMET);  //classification=HELMET;
    else
      c.set(helmetclassification::NOHELMET);  //classification=NOHELMET
    return c;
  }
protected:
  helmetclassifier(void);
  void train(const cv::Mat &trainingdata, const cv::Mat &labels) {}
  const HeadClassifier &headclassifier;
};

}
#endif //GGCLASSIFY
