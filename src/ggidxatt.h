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
#include "globalCVHeader.h"
#include <vector>

namespace gg {
  struct idxcontainer {
    size_t idx;
    inline virtual bool operator () (const idxcontainer &lhs, const idxcontainer &rhs) {
      return lhs.idx<rhs.idx;
    }
  };
 
  template <class T, class I> struct idxattribute : public idxcontainer {
    I item;
    T attribute;
    inline virtual bool operator () (const idxattribute &lhs, const idxattribute &rhs) {
      return lhs.attribute<rhs.attribute;
    }
  };

  template <class T, class I> struct idxattcollection {
    idxattcollection(size_t N) { init(N); }
    idxattcollection(void) {}
    void init(size_t N) {
      v.resize(N);
      for (size_t n=0;n<N;n++) 
  v[n].idx=n;
    }
    inline const T& operator [] (size_t n) const { return v[n]; }
    inline idxattribute<T,I>& operator [] (size_t n) { return v[n]; }
    std::vector<idxattribute<T, I> > v;
    void sort(void) {
      std::sort(v.begin(),v.end(),idxattribute<T,I>());
    }
    void sortidx(void) {
      std::sort(v.begin(),v.end(),idxcontainer());
    }
  };

  typedef idxattcollection<int,cv::Mat> imgattcollection;
}
