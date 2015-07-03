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
#include "csvfiling.h"

//from thinking in C++ vol 2 by Bruce Eckel and Chuck Allison
std::string& csvfiling::replaceAll(std::string& context, const std::string& from,
    const std::string& to) {
  size_t lookHere = 0;
  size_t foundHere;
  while((foundHere = context.find(from, lookHere))
      != std::string::npos) {
    context.replace(foundHere, from.size(), to);
    lookHere = foundHere + to.size();
  }
  return context;
}


void csvfiling::ReadCSVtoMatAlt(const std::string &filename, const std::string &seperator, cv::Mat &datamat) {
  int ncols,nrows;
  CSVSize<float>(filename,seperator,ncols,nrows);
  datamat=cv::Mat(nrows,ncols,CV_32FC1,cv::Scalar(0.));

  std::ifstream csvfile(filename.c_str());
  std::string line;

  for (int j=0;j<nrows;j++) {
    float *rawdata=datamat.ptr<float>(j);
    std::getline(csvfile,line);
    if (seperator!=" ")
      csvfiling::replaceAll(line,seperator," ");
    std::stringstream ss(line);
    //  for (int i=0;i<ncols && ss>>*rawdata; i++,rawdata++);
    for (int i=0;i<ncols && ss;i++) {
      ss>>rawdata[i];
      std::cout <<rawdata[i]<<",";
    }
    std::cout <<std::endl;
  }
}
