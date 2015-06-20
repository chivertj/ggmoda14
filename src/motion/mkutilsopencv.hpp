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
#ifndef MKOPENCV_NAMESPACE
#define MKOPENCV_NAMESPACE

#include "../globalCVHeader.h"

typedef CvMat MKMAT;
typedef IplImage MKIMG;

namespace mk {
  namespace utils {
    namespace opencv {
      //  template<class T> void ReshapeMatND(MKMAT *matIn, CvMatND *matOut);

      template<class T> T*& GetMPtr(MKMAT *mat) { return (T*)(0); }
      template<class T> T* GetImgPtr(MKIMG *img) { return (T*)(0); }
      template<class T> MKMAT* MakeMat(int Y, int X) { return 0; }
      template<class T> void InitMat(MKMAT *mat, int Y, int X, T* data=0) { }
      template<> inline float*& GetMPtr<float>(MKMAT *mat) {  return mat->data.fl; }
      template<> inline double*& GetMPtr<double>(MKMAT *mat) {  return mat->data.db;}
      template<> inline MKMAT* MakeMat<float>(int Y, int X) {  return cvCreateMat(Y,X,CV_32FC1);}
      template<> inline MKMAT* MakeMat<double>(int Y, int X) {  return cvCreateMat(Y,X,CV_64FC1);}
      template<> inline void InitMat<float>(MKMAT *mat, int Y, int X, float* data) {   cvInitMatHeader(mat,Y,X,CV_32FC1,data);}
      template<> inline void InitMat<double>(MKMAT *mat, int Y, int X, double* data) {   cvInitMatHeader(mat,Y,X,CV_64FC1,data);}
      template<> inline float* GetImgPtr<float>(MKIMG *img) {   return (float*)(img->imageData);}
      template<> inline double* GetImgPtr<double>(MKIMG *img) {   return (double*)(img->imageData);}
      template<> inline uchar* GetImgPtr<uchar>(MKIMG *img) {   return (uchar*)(img->imageData);}

      template<class T> MKIMG* MakeImg(CvSize size, int nChs) { return (MKIMG*)(0); }
      template<> inline MKIMG* MakeImg<float>(CvSize size, int nChs) { return cvCreateImage(size,IPL_DEPTH_32F,nChs); }
      template<> inline MKIMG* MakeImg<double>(CvSize size, int nChs) { return cvCreateImage(size,IPL_DEPTH_64F,nChs); }
      template<> inline MKIMG* MakeImg<unsigned char>(CvSize size, int nChs) { return cvCreateImage(size,IPL_DEPTH_8U,nChs); }
      template<> inline MKIMG* MakeImg<unsigned short>(CvSize size, int nChs) { return cvCreateImage(size,IPL_DEPTH_16U,nChs); }
      template<> inline MKIMG* MakeImg<signed short>(CvSize size, int nChs) { return cvCreateImage(size,IPL_DEPTH_16S,nChs); }


      //~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
      template<class T> void ReshapeMatND(MKMAT *matIn, CvMatND *matOut) {
	assert(matIn);
	assert(matOut);
	int ttlSizeIn=matIn->rows*matIn->cols;
#ifdef DEBUG
	int ttlSizeOut=1;
	for (int i=0;i<matOut->dims;i++)
	  ttlSizeOut*=matOut->dim[i].size;
	assert(ttlSizeIn==ttlSizeOut);
	printf("DEBUG CHECK\n");
#endif
	memcpy(matOut->data.ptr,matIn->data.ptr,sizeof(T)*ttlSizeIn);
      }
      //~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
      void CopyImg(MKIMG *imgIn, MKIMG **imgOut);
      template<class T> MKIMG* CopyImg(MKIMG *imgIn) { return 0; }
      template<> inline MKIMG* CopyImg<float>(MKIMG *imgIn) { IplImage *imgOut=cvCreateImage(cvGetSize(imgIn),IPL_DEPTH_32F,imgIn->nChannels); cvConvertScale(imgIn,imgOut); return imgOut; }
      template<> inline MKIMG* CopyImg<double>(MKIMG *imgIn) { IplImage *imgOut=cvCreateImage(cvGetSize(imgIn),IPL_DEPTH_64F,imgIn->nChannels); cvConvertScale(imgIn,imgOut); return imgOut; }
      template<> inline MKIMG* CopyImg<unsigned char>(MKIMG *imgIn) { IplImage *imgOut=cvCreateImage(cvGetSize(imgIn),IPL_DEPTH_8U,imgIn->nChannels); cvConvertScale(imgIn,imgOut); return imgOut; }
      template<> inline MKIMG* CopyImg<unsigned short>(MKIMG *imgIn) { IplImage *imgOut=cvCreateImage(cvGetSize(imgIn),IPL_DEPTH_16U,imgIn->nChannels); cvConvertScale(imgIn,imgOut); return imgOut; }
      //IPL_DEPTH_8U, IPL_DEPTH_8S, IPL_DEPTH_16U, IPL_DEPTH_16S, IPL_DEPTH_32S, IPL_DEPTH_32F and IPL_DEPTH_64F 

      template<class T> inline void ResizeMat(MKMAT **mat, int newrows, int newcols, int rowoff=0, int coloff=0) { //this is a bit bad because mat could be pointing to a different place which isn't really ideal as someone might assume that it was just the data that has changed...
	assert(*mat); 
	int rows=(*mat)->rows, cols=(*mat)->cols;
	if (rows==newrows && cols==newcols) return; 
	MKMAT *newmat=MakeMat<T>(newrows,newcols);
	cvZero(newmat);
	T *rawnew=GetMPtr<T>(newmat);
	T *rawold=GetMPtr<T>(*mat);
	for (int j=rowoff;j<rows && j<newrows;j++) {
	  for (int i=coloff;i<cols && i<newcols;i++)
	    rawnew[j*newrows+i]=rawold[j*rows+i];
	}
	cvReleaseMat(mat);
	*mat=newmat;
      }
      //------------------------------------
      template<class T> inline void CopyMatContent(MKMAT *inmat, MKMAT *outmat) {
	assert(inmat);    assert(outmat);
	T *rawin=GetMPtr<T>(inmat);    T *rawout=GetMPtr<T>(outmat);
	int inrows=inmat->rows, incols=inmat->cols, outrows=outmat->rows, outcols=outmat->cols;
	cvZero(outmat);
	for (int j=0; j<inrows && j<outrows; j++) {
	  for (int i=0; i<incols && i<outcols; i++)
	    rawout[j*outcols+i]=rawin[j*incols+i];
	}
      }

      void CopyMat(MKMAT *inmat, MKMAT **outmat);

      template <class ipT, class opT> void Copy(ipT *ipData, opT *opData, int size);
      template<class T> void PrintMat(MKMAT *mat);

      template<class T> inline void MakeMat(MKMAT **mat, int rows, int cols);
      template<class T> inline IplImage* Mat2Img(MKMAT *mat);
      template<class T> inline int GetImgT(void) { return 0; }
      template<> inline int GetImgT<float>(void) { return IPL_DEPTH_32F; }
      template<> inline int GetImgT<unsigned char>(void) { return IPL_DEPTH_8U; }
      template<> inline int GetImgT<double>(void) { return IPL_DEPTH_64F; }
      template<> inline int GetImgT<int>(void) { return IPL_DEPTH_32S; }
    }
  }
}
//------------------------------------
template <class ipT, class opT> inline void mk::utils::opencv::Copy(ipT *ipData, opT *opData, int size) {
  for (int i=0;i<size;i++)
    opData[i]=opT(ipData[i]);
}
//------------------------------------
template<class T> void mk::utils::opencv::PrintMat(MKMAT *mat) {
  T *data=mk::utils::opencv::GetMPtr<T>(mat);
  int rows=mat->rows, cols=mat->cols;
  for (int j=0;j<rows;j++) {
    for (int i=0;i<cols;i++) 
      printf("%f ",data[j*cols+i]);
    printf("\n");
  }
}
//------------------------------------
template<class T> inline void mk::utils::opencv::MakeMat(MKMAT **mat, int rows, int cols) {
  assert(mat);
  if (!(*mat && (*mat)->rows==rows && (*mat)->cols==cols && ((*mat)->step*(*mat)->rows)==int(cols*sizeof(T)*rows))) {
    if (*mat)
      cvReleaseMat(mat);
    *mat=mk::utils::opencv::MakeMat<T>(rows,cols);
  }
  cvZero(*mat);
}
//------------------------------------
template <class T> inline IplImage* mk::utils::opencv::Mat2Img(MKMAT *mat) {
  assert(mat);
  IplImage *img=cvCreateImage(cvSize(mat->cols,mat->rows),mk::utils::opencv::GetImgT<T>(),1);
  cvCopy(mat,img);
  return img;
}
//------------------------------------
#endif //MKOPENCV_NAMESPACE
