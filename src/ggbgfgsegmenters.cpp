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
#include "ggbgfgsegmenters.h"

namespace gg {
MakeBGFGSegmenterCodeBook::MakeBGFGSegmenterCodeBook(size_t X, size_t Y) : MakeBGFGSegmenter(X,Y),model(0),nframesToLearnBG(150),fr(0), ImaskCodeBook(0),ImaskCodeBookCC(0){
  model=cvCreateBGCodeBookModel();
  model->modMin[0]=3;
  model->modMin[1]=model->modMin[2]=3;
  model->modMax[0]=10;
  model->cbBounds[0]=model->cbBounds[1]=model->cbBounds[2]=10;
  ImaskCodeBook=cvCreateImage(cvSize(X,Y),IPL_DEPTH_8U,1);
  cvSet(ImaskCodeBook,cvScalar(255));
  ImaskCodeBookCC=cvCreateImage(cvSize(X,Y),IPL_DEPTH_8U,1);
  yuvImg=cvCreateImage(cvSize(X,Y),IPL_DEPTH_8U,3);
}
//################
MakeBGFGSegmenterCodeBook::~MakeBGFGSegmenterCodeBook(void) {
  cvReleaseImage(&ImaskCodeBook);
  cvReleaseImage(&ImaskCodeBookCC);
  cvReleaseImage(&yuvImg);
  cvReleaseBGCodeBookModel(&model);
}
//################
bool MakeBGFGSegmenterCodeBook::operator() (IplImage *img) {
  bool start=false;
  adjustmean(img);
  cvCvtColor(img,yuvImg,CV_BGR2YCrCb);
  if (fr<nframesToLearnBG)
    cvBGCodeBookUpdate(model,yuvImg);
  else if (fr==nframesToLearnBG)
    cvBGCodeBookClearStale(model,model->t/2);
  if (fr>=nframesToLearnBG) {
    cvBGCodeBookDiff(model,yuvImg,ImaskCodeBook);
    cvCopy(ImaskCodeBook,ImaskCodeBookCC);
#if(1)
    cvErode(ImaskCodeBookCC,ImaskCodeBookCC);
    cvErode(ImaskCodeBookCC,ImaskCodeBookCC);
    cvErode(ImaskCodeBookCC,ImaskCodeBookCC);
    cvDilate(ImaskCodeBookCC,ImaskCodeBookCC);
    cvDilate(ImaskCodeBookCC,ImaskCodeBookCC);
    cvDilate(ImaskCodeBookCC,ImaskCodeBookCC);
#endif
    start=true;
  }
  fr++;
  return start;
}
//################
}
