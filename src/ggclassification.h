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
#ifndef GGCLASSIFICATION
#define GGCLASSIFICATION

#include <string>

namespace gg {
  //############################################
  //############################################
  /**
   *  Base class for discrete classifications.
   *  gg::discreteclassification::c=0 for no classification.
   */
  class discreteclassification {
  public:
    typedef int classT;
    const static classT NOCLASSIFICATION=0;
    discreteclassification(void) : c(0) {}
    discreteclassification(classT _c) : c(_c) {}
    discreteclassification(const discreteclassification &_c) : c(_c.c) {}
    virtual ~discreteclassification(void) {}
    classT get(void) const {return c;}
    void set(const classT &_c) {c=_c;}
    bool operator!= (const discreteclassification &_dc) const { if (_dc.c!=c) return true; return false; }
    bool operator!= (const classT &_dc) const { if (_dc!=c) return true; return false; }
    bool operator== (const discreteclassification &_dc) const { if (_dc.c==c) return true; return false; }
    bool operator==(const classT &_c) const { if (_c==c) return true; return false; }
    void operator= (const classT &_c) { c=_c; }
    void operator= (const discreteclassification &_c) { c=_c.c; }
    virtual std::string str(void) { return discreteclassification::str(c); }
    virtual bool is(void) { return discreteclassification::is(c); }
    static std::string str(const classT &_c)  {
      return std::string("NOCLASSIFICATION");
    }
    static bool is(const classT &_c) {
      bool _is=false;
      switch (_c) {
      case NOCLASSIFICATION:
        _is=true;
        break;
      }
      return _is;
    }
  protected:
    classT c;
  };
  //############################################
  //############################################
  /**
   *  Helmet vs No Helmet Discrete classification with:\n
   *  gg::discreteclassification::c=gg::helmetclassification::HELMET=1 or \n
   *  gg::discreteclassification::c=gg::helmetclassification::NOHELMET=2 \n
   *  gg::discreteclassification::c=gg::discreteclassification::NOCLASSIFICATION=0 for no classification.
   */
  class helmetclassification : public discreteclassification {
  public:
    typedef discreteclassification::classT classT;
    const static classT HELMET=1;
    const static classT NOHELMET=2;
    helmetclassification(const discreteclassification &dc) {
      c=dc.get();
    }
    const helmetclassification& operator= (const discreteclassification &dc) {
      this->c=dc.get();
      return *this;
    }
    static std::string str(const classT &_c) {
      std::string val;
      switch (_c) {
      case HELMET:
        val=std::string("HELMET");
        break;
      case NOHELMET:
        val=std::string("NOHELMET");
        break;
      default:
        val=discreteclassification::str(_c);
        break;
      }
      return val;
    }
    std::string str(void) {
      return str(c);
    }
    static bool is(const classT &_c) {
      bool _is=false;
      switch (_c) {
      case HELMET:
      case NOHELMET:
        _is=true;
        break;
      default:
        _is=discreteclassification::is(_c);
        break;
      }
      return _is;
    }
  };
  //############################################
  //############################################
  /**
   *  Vehicle Discrete classification with:\n
   *  gg::discreteclassification::c=gg::helmetclassification::MOTORBIKE=3 or \n
   *  gg::discreteclassification::c=gg::helmetclassification::NOTMOTORBIKE=4 \n
   *  gg::discreteclassification::c=gg::discreteclassification::NOCLASSIFICATION=0 for no classification.
   */
  class vehicleclassification : public discreteclassification {
  public:
    typedef discreteclassification::classT classT;
    const static classT MOTORBIKE=3;
    const static classT NOTMOTORBIKE=4;
    vehicleclassification(const discreteclassification &dc) {
          c=dc.get();
    }
    const vehicleclassification& operator= (const discreteclassification &dc) {
          this->c=dc.get();
          return *this;
    }
    static std::string str(const classT &_c) {
      std::string val;
      switch (_c) {
      case MOTORBIKE:
        val=std::string("MOTORBIKE");
        break;
      case NOTMOTORBIKE:
        val=std::string("NOTMOTORBIKE");
        break;
      default:
        val=discreteclassification::str(_c);
        break;
      }
      return val;
    }
    std::string str(void) {
      return str(c);
    }
    static bool is(const classT &_c) {
      bool _is=false;
      switch (_c) {
      case MOTORBIKE:
      case NOTMOTORBIKE:
        _is=true;
        break;
      default:
        _is=discreteclassification::is(_c);
        break;
      }
      return _is;
    }
  };
}

#endif //GGCLASSIFICATION

