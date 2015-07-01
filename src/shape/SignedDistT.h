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
#ifndef CSIGNEDDISTT_CLASS
#define CSIGNEDDISTT_CLASS

#include "../globalCVHeader.h"

#include "../ACDefinitions.h"
using namespace ACDefinitions;
#include <vector>
//img should be a 8 bit grey level image with 
const IMAGEDATA_T SIGNDIST_INFINITY=99999.f;
const int SIGNDIST_POSITIVE=100;

class CSignedDistT {
 public:
    CSignedDistT(void);
    virtual ~CSignedDistT(void);
    void Init(IplImage *img, IMAGEBYTE_T contourVal); 
    void PerformOp(void);
    IplImage* BorrowSignedDistT(void);
 protected:
    void CalcSignMap(void);
    IMAGEDATA_T minForward(int pos, int i, int j);
    IMAGEDATA_T minBackward(int pos, int i, int j);
    
    IplImage *m_img;
    IplImage *m_sgndDistT;
    IplImage *m_sgnMap;
    IplImage *m_initSgndDistT;
    IplImage *m_initCurve;

    int m_X,m_Y;
    IMAGEBYTE_T m_contourVal;
    std::vector<IMAGEDATA_T> m_mask;
    IMAGEDATA_T *m_rawInitLevelSet;
};
//---------------------------------------
inline IMAGEDATA_T CSignedDistT::minForward(int pos, int i, int j) {
    IMAGEDATA_T min = SIGNDIST_INFINITY;

    if(i>0 && j>0)
  m_mask[0] = m_rawInitLevelSet[pos-m_X-1] + DIST2;
    else m_mask[0] = SIGNDIST_INFINITY;
    
    if(j>0)
  m_mask[1] = m_rawInitLevelSet[pos-m_X] + DIST1;
    else m_mask[1] = SIGNDIST_INFINITY;
    
    if((i<m_X-1) && (j>0))
  m_mask[2] = m_rawInitLevelSet[pos-m_X+1] + DIST2;
    else m_mask[2] = SIGNDIST_INFINITY;

    m_mask[3] = m_rawInitLevelSet[pos];
    
    if(i>0)
  m_mask[4] = m_rawInitLevelSet[pos-1] + DIST1;
    else m_mask[4] = SIGNDIST_INFINITY;
    
    for(int k=0;k<5;k++)
  if(m_mask[k]<min) min = m_mask[k];

    return min;    
}
//---------------------------------------
inline IMAGEDATA_T CSignedDistT::minBackward(int pos, int i, int j) {
    IMAGEDATA_T min = SIGNDIST_INFINITY;

    m_mask[0] = m_rawInitLevelSet[pos];

    if(i<m_X-1)
  m_mask[1] = m_rawInitLevelSet[pos+1] + DIST1;
    else m_mask[1] = SIGNDIST_INFINITY;

    if((j<m_Y-1) && (i<m_X-1))
  m_mask[2] = m_rawInitLevelSet[pos+m_X+1] + DIST2;
    else m_mask[2] = SIGNDIST_INFINITY;

    if(j<m_Y-1)
  m_mask[3] = m_rawInitLevelSet[pos+m_X] + DIST1;
    else m_mask[3] = SIGNDIST_INFINITY;

    if((i>0) && (j<m_Y-1))
  m_mask[4] = m_rawInitLevelSet[pos+m_X-1] + DIST2;
    else m_mask[4] = SIGNDIST_INFINITY;

    for(int k=0;k<5;k++)
  if(m_mask[k]<min) min = m_mask[k];

    return min;
}
//---------------------------------------

#endif //CSIGNEDDISTT_CLASS
