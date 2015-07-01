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
#include "ggpca.h"

namespace gg {
  void exportcolm(const cv::Mat &colm, size_t Y, const std::string &name, bool transpose) {
    cv::Mat byteimg(colm.rows,1,CV_8U);
    for (size_t i=0;i<colm.cols;i++) {
      cv::Mat img=colm.col(i);
      img.convertTo(byteimg,byteimg.type());
      if (transpose)
  byteimg=byteimg.reshape(1,Y).t();
      else
  byteimg=byteimg.reshape(1,Y); //.t();
      region::ExportImage(byteimg,name,3,i);
    }
  }

  void exportrowm(const cv::Mat &rowm, size_t X, const std::string &name) {
    std::cout <<rowm.rows<<","<<rowm.cols<<std::endl;
    cv::Mat byteimg(rowm.cols,1,CV_8U);
    for (size_t i=0;i<rowm.rows;i++) {
      cv::Mat img=rowm.row(i);
      img.convertTo(byteimg,byteimg.type());
      byteimg=byteimg.reshape(1,X);
      region::ExportImage(byteimg,name,3,i);
    }
  }

  int getdata(const std::string &filename, uchar **data) {
    std::ifstream file(filename,std::ios::in|std::ios::binary|std::ios::ate);
    if (!file.is_open()) {
      *data=0;
      return 0;
    }
    std::ifstream::pos_type size=file.tellg();
    *data=new uchar [size];
    file.seekg(0,std::ios::beg);
    file.read((char*)*data,size);
    file.close();
    return size;
  }

  void converttocolmat(uchar *rawdata,size_t nimgs,size_t X, size_t Y, cv::Mat &colmat) {
    size_t nrows=nimgs,ncols=X*Y;
    cv::Mat tmpM=cv::Mat(nrows,ncols,CV_8UC1,rawdata);
    tmpM=tmpM.t();
    nrows=tmpM.rows; ncols=tmpM.cols;
    colmat=cv::Mat(nrows,ncols,CV_32F);
    tmpM.convertTo(colmat,colmat.type());
  }


  cv::PCA compressPCA(const cv::Mat& pcaset, int maxComponents,
          const cv::Mat& testset, cv::Mat& compressed, cv::Mat &reconstructset) {
    cv::PCA pca(pcaset,cv::Mat(),CV_PCA_DATA_AS_COL,maxComponents);
    // if there is no test data, just return the computed basis, ready-to-use
    if( !testset.data )
      return pca;
    CV_Assert( testset.rows == pcaset.rows );
    CV_Assert( reconstructset.rows == testset.rows && reconstructset.cols==testset.cols);
    compressed.create(maxComponents,testset.cols, testset.type());
    for( int i = 0; i < testset.cols; i++ ) {
      cv::Mat vec = testset.col(i), coeffs = compressed.col(i);
      cv::Mat rec = reconstructset.col(i);
      pca.project(vec, coeffs);
      pca.backProject(coeffs, rec);
      printf("%d. diff = %g\n", i, norm(vec, rec, cv::NORM_L2));
    }
    return pca;
  }


  cv::PCA robcompressPCA(const cv::Mat& pcaset, int maxComponents,
       const cv::Mat& testset, cv::Mat_<float>& compressed, cv::Mat &reconstructset) {
    if (maxComponents>pcaset.cols) maxComponents=pcaset.cols;

    cv::PCA pca(pcaset,cv::Mat(),CV_PCA_DATA_AS_COL,pcaset.cols);
    // if there is no test data, just return the computed basis, ready-to-use
    if( !testset.data )
      return pca;
    CV_Assert( testset.rows == pcaset.rows );
    CV_Assert( reconstructset.rows == testset.rows && reconstructset.cols==testset.cols);
    cv::Mat U=pca.eigenvectors.t();
    U=U.colRange(0,maxComponents);
    robcoeff calc;
#if(1)
    compressed=calc.is2fsrob<float>(testset,cv::Mat_<float>(pca.mean),cv::Mat_<float>(U),cv::Mat_<float>(pca.eigenvalues));
    cv::Mat_<float> Xm=cv::repeat(pca.mean,1,compressed.cols);
    reconstructset=U*compressed+Xm;

#else
    compressed=cv::Mat_<float>(maxComponents,testset.cols);
    for( int i = 0; i < testset.cols; i++ ) {
      cv::Mat_<float> vec = testset.col(i);
      cv::Mat_<float> coeffs = compressed.col(i);
      cv::Mat rec = reconstructset.col(i);
      //project from image space to feature space
      coeffs=calc.is2fsrob<float>(vec,cv::Mat_<float>(pca.mean),cv::Mat_<float>(U),cv::Mat_<float>(pca.eigenvalues));
      // and then reconstruct it
      rec=U*coeffs+pca.mean;
      // and measure the error
      printf("%d. diff = %g\n", i, norm(vec, rec, cv::NORM_L2));
    }
#endif
    return pca;
  }


}
