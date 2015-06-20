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
#ifndef CSVFILING_NS
#define CSVFILING_NS

#include <string>
#include <fstream>
#include <sstream>
#include <iostream>
#include "globalCVHeader.h"

namespace csvfiling {
  std::string& replaceAll(std::string& context, const std::string& from,
			  const std::string& to);
  template <class T> void ReadCSV(const std::string &filename, const std::string &seperator);
  template <class T> void CSVSize(const std::string &filename, const std::string &seperator, int &ncols, int &nrows);
  template <class T> void ReadCSVtoMat(const std::string &filename, const std::string &seperator, cv::Mat_<T> &datamat);
  void ReadCSVtoMatAlt(const std::string &filename, const std::string &seperator, cv::Mat &datamat);
}


namespace csvfiling {

  template <class T> void ReadCSV(const std::string &filename, const std::string &seperator) {
    std::ifstream csvfile(filename.c_str());
    std::string line;
    while (std::getline(csvfile,line)) {
      if (seperator!=" ")
	csvfiling::replaceAll(line,seperator," ");
      std::stringstream ss(line);
      T dataelement;
      while (ss>>dataelement) {
	std::cout <<dataelement<<std::endl;
      }
    }
  }

  template <class T> void CSVSize(const std::string &filename, const std::string &seperator, int &ncols, int &nrows) {
    std::ifstream csvfile(filename.c_str());
    std::string line;
    std::getline(csvfile,line);
    if (seperator!=" ")
	csvfiling::replaceAll(line,seperator," ");
    std::stringstream ss(line);
    T dataelement;
    ncols=0;
    while (ss>>dataelement)
      ncols++;
    nrows=1;      
    while (std::getline(csvfile,line))
      nrows++;
  }

  template <class T> void ReadCSVtoMat(const std::string &filename, const std::string &seperator, cv::Mat_<T> &datamat) {
    int ncols,nrows;
    CSVSize<T>(filename,seperator,ncols,nrows);
    datamat=cv::Mat_<T>(nrows,ncols,0.);

    std::ifstream csvfile(filename.c_str());
    std::string line;

    for (int j=0;j<nrows;j++) {
      T *rawdata=(T*)(datamat.ptr(j));
      std::getline(csvfile,line);
      if (seperator!=" ")
	csvfiling::replaceAll(line,seperator," ");
      std::stringstream ss(line);
      for (int i=0;i<ncols && ss>>*rawdata; i++,rawdata++);
      //      for (int i=0;i<ncols;i++)
      //	ss>>rawdata[i];
    }
  }

}


#endif //CSVFILING_NS
