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
#ifndef CDISTTSKEL_CLASS
#define CDISTTSKEL_CLASS

#include "../globalCVHeader.h"
#include "../ACDefinitions.h"
using namespace ACDefinitions;

class CDistTSkel {
 public:
  CDistTSkel(void);
  virtual ~CDistTSkel(void);
  void Init(IplImage *distT, bool inverse);
  void PerformOp(void);
  IplImage* BorrowSkel(void);
  IplImage *BorrowDistSkel(void);
 protected:
  IplImage *m_distT;
  IplImage *m_skel;
  IplImage *m_distSkel;
  void ZeroBorder(IplImage *image);
  void RemoveIsolatedPoints(void);
  void LocalMax(IplImage *img, float dist1, float dist2, IplImage *localMax, bool inverse, float threshold);
  bool GrowPnts(IplImage *img, float dist1, float dist2, IplImage *distImg, bool inverse, float threshold);
  void ConnectPnts(IplImage *img, float dist1, float dist2, IplImage *distImg, bool inverse, float threshold);

  void CreateDistSkel(void);

  bool m_inverse;

  //thinning functionality
  void GetNineNeighbours(IMAGEBYTE_T *neighbours, IMAGEBYTE_T *data, int x, int y, int Xstep);
  int CountNeighbourTransitions(IMAGEBYTE_T *neighbours);
  int CountNeighbours(IMAGEBYTE_T *neighbours);
  const int BYTEMATRIX;//=1;
  const IMAGEBYTE_T BM_SET;//=255;
  const IMAGEBYTE_T BM_NOTSET;//=0;
  void BinaryThin(IplImage *image);
  void Thin(void);
  const double m_minMaxSkelVal;
  void RemoveBoundaryPnts(double maxSkelVal, bool min);
};
//·····························································
inline
void CDistTSkel::GetNineNeighbours(IMAGEBYTE_T *neighbours, IMAGEBYTE_T *data, int x, int y, int Xstep) {
    neighbours[0]=data[(y-1)*Xstep+x-1];
    neighbours[1]=data[(y)  *Xstep+x-1];
    neighbours[2]=data[(y+1)*Xstep+x-1];
    neighbours[3]=data[(y+1)*Xstep+x];
    neighbours[4]=data[(y+1)*Xstep+x+1];
    neighbours[5]=data[(y)  *Xstep+x+1];
    neighbours[6]=data[(y-1)*Xstep+x+1];
    neighbours[7]=data[(y-1)*Xstep+x];
    neighbours[8]=data[(y-1)*Xstep+x-1];
}
//·····························································
inline
int CDistTSkel::CountNeighbourTransitions(IMAGEBYTE_T *neighbours) {
    int trans = 0;
    for (int p=0;p<=7;p++)
  if (neighbours[p]==BM_NOTSET && neighbours[p+1]==BM_SET) trans++;
    return trans;
}
//·····························································
inline
int CDistTSkel::CountNeighbours(IMAGEBYTE_T *neighbours) {
    int neighboursCnt=0;
    for (int p=0;p<7;p++)
  if (neighbours[p]==BM_SET) neighboursCnt++;
    return neighboursCnt;
}
//·····························································

#endif //CDISTTSKEL_CLASS

