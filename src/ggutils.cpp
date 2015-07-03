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
#include "ggutils.h"

namespace gg {
size_t inc(const size_t &limit, const size_t &idx) { if (idx>=limit-1) return 0; return idx+1;}
size_t dec(const size_t &limit, const size_t &idx) { if (idx<=1) return limit-1; return idx-1;}
std::ostream& printmatdetails(std::ostream &os, const cv::Mat &m) {
  os <<m.rows<<","<<m.cols<<","<<m.channels()<<","<<m.depth()<<","<<m.type()<<std::endl;
  return os;
}
int refcount(const cv::Mat &m) {
  int r=-1;
  if (!m.empty())
    r=*(m.refcount);
  return r;
}
}

