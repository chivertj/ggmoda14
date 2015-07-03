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
#ifndef ACDEFINITIONS_HEADER
#define ACDEFINITIONS_HEADER

#include "globalCVHeader.h"

#include <string>

//#define KERNELDENSITYESTIMATION
//#define EXPORTDATA
//#define EXPORTITERATIONDATA
//#define EXPORTITERLEVSET
//#define EXPORTLEVELSET

const int UPDATESHAPEITER=10;
const int REINITLEVSET=10;

namespace ACDefinitions {
const int ACTIVE_IPLDEPTH = IPL_DEPTH_32F;
typedef float IMAGEDATA_T;
typedef unsigned char IMAGEBYTE_T;
typedef int IMAGEINT32S_T;
typedef unsigned int HISTOBIN_CNT;
typedef int ACHISTOIMG_T;
typedef double ACNORM_T;
const int ACNORM_IPLDEPTH = IPL_DEPTH_64F;

const IMAGEDATA_T DIST1=1.f;
//    const IMAGEDATA_T DIST2=1.4142f;
const IMAGEDATA_T DIST2=1.351f;
const IMAGEDATA_T CONTOURVAL=255.f;
const IMAGEDATA_T REINIT_INFINITE=99999.f;
const IMAGEBYTE_T INITCONTOURVAL=255;
const IMAGEDATA_T ACSMALL=0.00000000000001;
const IMAGEDATA_T ACLITTLENUMBER=0.00001f;
const IMAGEDATA_T LEVSET_CHANGE_THRESHOLD=ACSMALL;//.05f;//.05f;
void ACCentralDiff(IplImage *img,IplImage *result, int xorder, int yorder, int aperture_size, IplImage *tmp); //if xorder=1 & yorder=1 then x axis central difference otherwise y axis central difference

void ACSaveImg(const IplImage *img, const std::string &filename);
void ACSaveImgColor(const IplImage *img, const std::string &filename);

IplImage* ACZeroContourAndColourMerge(IplImage *image, IplImage *levelSet);
void ACZeroContourAndColourMergeAndSave(IplImage *image, IplImage *levelSet, const std::string &filename);

IplImage* ACPosNegThreshold(IplImage *levelSet, bool negThresh=true);
template<class T> IplImage* ACPosNeg(IplImage *levelSet, bool negThresh=true) { return 0; }
template<> IplImage* ACPosNeg<float>(IplImage *levelSet, bool negThresh);
template<> IplImage* ACPosNeg<double>(IplImage *levelSet, bool negThresh);

//><-><-><-><-><-><-><-><-><-><-><-><-><-><-><-><-><-><
template <class ACTUPLE_T>
struct AC3TUPLE {
  ACTUPLE_T R;
  ACTUPLE_T G;
  ACTUPLE_T B;
  AC3TUPLE(void) : R(0), G(0),B(0) {}
  AC3TUPLE(ACTUPLE_T r, ACTUPLE_T g, ACTUPLE_T b) : R(r), G(g),B(b) {}
};
//><-><-><-><-><-><-><-><-><-><-><-><-><-><-><-><-><-><
template <class ACPNT_T>
struct ACPnt {
  ACPNT_T x;
  ACPNT_T y;
};

template <class ACPNT_T, class ACDIST_T>
struct ACPntPair {
  ACPnt<ACPNT_T> srcPnt;
  ACPnt<ACPNT_T> destPnt;
  ACDIST_T distance;
};

typedef ACPntPair<int,double>* PntDistances;
typedef ACPntPair<int,double> PntDistance;

template<class ARRAY_T> ARRAY_T* PruneArray(ARRAY_T *, int size, int finalSize);

typedef ACPnt<float> PntFL;
typedef PntFL* PntFLs;
std::string StringAndZeroPad(int digit,int noDigits);

const IMAGEDATA_T FARNEGDIST=-0.;
const IMAGEDATA_T KERNELSMOOTHINGVAL=10.;
enum ZHANGFREEDMAN_MEASURE {KULLBACKLEIBLER};
enum SHAPECONTEXT {FG_MODEL,FG_CURRENT,BG_CURRENT};
enum SHAPEABSTRACTION {SKELETON,PROBMAP,SKELPNTS,RDMAP};

template<class T> T* ReAllocate(T *data,int newSize);
template<class T> T* ReAllocateAndCopy(T *prevData, T *newData, int newSize);

std::string ExportStr(std::string &idx, const char *fileBase);
std::string ExportStrSub(std::string &idx, std::string &subIdx, const char *fileBase);

template <class T> void SaveImgs(T &imgs, const char *base, IplImage* (*ImgFunc)(IplImage*,bool)=0);

void ExportImg(IplImage *img, const char *filenameBase, bool posNegThresh, int idx);
void ExportImg(IplImage *img, const char *filenameBase, bool posNegThresh);
}

template <class T> void ACDefinitions::SaveImgs(T &imgs, const char *base, IplImage* (*ImgFunc)(IplImage*,bool)) {
  int imgIdx;
  typename T::iterator imgIt;
  for (imgIdx=1,imgIt=imgs.begin();imgIt!=imgs.end();imgIt++,imgIdx++) {
    std::string opFilename=base+ACDefinitions::StringAndZeroPad(imgIdx,3)+".jpg";
    if (ImgFunc) {
      IplImage *tmpImg=(*ImgFunc)((IplImage*)(*imgIt),true);
      ACDefinitions::ACSaveImg(tmpImg,opFilename);
      cvReleaseImage(&tmpImg);
    }
    else
      ACDefinitions::ACSaveImg((IplImage*)(*imgIt),opFilename);
  }
}

template<class ARRAY_T> ARRAY_T* ACDefinitions::PruneArray(ARRAY_T *data, int size, int finalSize) {
  assert(data);
  assert(size>finalSize);

  ARRAY_T *newData = new ARRAY_T [finalSize];
  int step=int(float(size)/float(finalSize)+.5);

  for (int i=0,j=0;j<finalSize && i<size;i+=step,j++)
    newData[j]=data[i];

  return newData;
}

template<class T> T* ACDefinitions::ReAllocate(T *data,int newSize) {
  if (data) delete [] data;
  data = new T[newSize];
  memset(data,0,newSize*sizeof(T));
  return data;
}

template<class T> T* ACDefinitions::ReAllocateAndCopy(T *prevData, T *newData, int newSize) {
  prevData=ReAllocate(prevData,newSize);
  memcpy(prevData,newData,newSize*sizeof(T));
  return prevData;
}

template<> inline IplImage* ACDefinitions::ACPosNeg<float>(IplImage *levelSet, bool negThresh) 
    { return ACPosNegThreshold(levelSet,negThresh); }
template<> inline IplImage* ACDefinitions::ACPosNeg<double>(IplImage *levelSet, bool negThresh) {     
  assert(levelSet);
  IplImage *threshold=cvCreateImage(cvGetSize(levelSet),IPL_DEPTH_8U,1);
  cvZero(threshold);
  int X=levelSet->width,Y=levelSet->height;
  //    IMAGEBYTE_T *rawLevelSet=(IMAGEBYTE_T*)(levelSet->imageData);
  double *rawLevelSet=(double*)(levelSet->imageData);
  IMAGEBYTE_T *rawThreshold=(IMAGEBYTE_T*)(threshold->imageData);
  double levelSetVal;
  for (int y=0;y<Y;y++) {
    for (int x=0;x<X;x++) {
      levelSetVal=rawLevelSet[y*X+x];
      if ((negThresh && levelSetVal<=0.) || (!negThresh && levelSetVal>=0.)) {
        rawThreshold[y*X+x]=255;
      }
    }
  }
  return threshold; 
}
#endif //ACDEFINITIONS_HEADER
