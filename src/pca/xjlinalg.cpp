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
#include "xjlinalg.h"
#include "utility.h"
#include <iostream>

namespace xjlinalg {

  //##############################################
  cv::Mat appendcols(const cv::Mat &data1, const cv::Mat &data2) {
    assert(data1.rows==data2.rows);
    assert(data1.type()==data2.type());
    cv::Mat newdata(data1.rows,data1.cols+data2.cols,data1.type());
    appendcols(newdata,data1,data2);
    return newdata;
  }
  void appendcols(cv::Mat &datain, const cv::Mat &data1, const cv::Mat &data2) {
    assert(datain.rows==data1.rows);
    assert(datain.cols==data1.cols+data2.cols);
    assert(datain.type()==data1.type());
    assert(data1.type()==data2.type());
    cv::Mat newdatacols=datain.colRange(cv::Range(0,data1.cols));
    data1.copyTo(newdatacols);
    newdatacols=datain.colRange(cv::Range(data1.cols,data1.cols+data2.cols));
    data2.copyTo(newdatacols);
  }

  cv::Mat appendrows(const cv::Mat &data1, const cv::Mat &data2) {
    assert(data1.cols==data2.cols);
    assert(data1.type()==data2.type());
    cv::Mat newdata(data1.rows+data2.rows,data1.cols,data1.type());
    appendrows(newdata,data1,data2);
    return newdata;
  }
  void appendrows(cv::Mat &datain, const cv::Mat &data1, const cv::Mat &data2) {
    assert(datain.cols==data1.cols);
    assert(datain.rows==data1.rows+data2.rows);
    assert(datain.type()==data1.type());
    assert(data1.type()==data2.type());
    cv::Mat newdatarows=datain.rowRange(cv::Range(0,data1.rows));
    data1.copyTo(newdatarows);
    newdatarows=datain.rowRange(cv::Range(data1.rows,data1.rows+data2.rows));
    data2.copyTo(newdatarows);
  }

  cv::Mat tile(const cv::Mat &data00, const cv::Mat &data01, const cv::Mat &data10, const cv::Mat &data11) {
    assert(data00.type()==data01.type());
    assert(data10.type()==data11.type());
    assert(data00.type()==data11.type());
    assert(data00.rows==data01.rows);
    assert(data10.rows==data11.rows);
    assert(data00.cols==data10.cols);
    assert(data01.cols==data11.cols);
    cv::Mat dataout(data00.rows+data10.rows,data00.cols+data01.cols,data00.type());
    cv::Mat rectang=dataout(cv::Range(0,data00.rows),cv::Range(0,data00.cols));
    data00.copyTo(rectang);
    rectang=dataout(cv::Range(data00.rows,data00.rows+data11.rows),
     cv::Range(data00.cols,data00.cols+data11.cols));
    data11.copyTo(rectang);
    rectang=dataout(cv::Range(0,data01.rows),cv::Range(data00.cols,data00.cols+data01.cols));
    data01.copyTo(rectang);
    rectang=dataout(cv::Range(data00.rows,data00.rows+data10.rows),cv::Range(0,data10.cols));
    data10.copyTo(rectang);
    return dataout;
  }
}
//###########################################
