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
#include "ggclassification.h"
namespace gg {
  const discreteclassification::classT discreteclassification::NOCLASSIFICATION;
  const discreteclassification::classT helmetclassification::HELMET;
  const discreteclassification::classT helmetclassification::NOHELMET;
  const discreteclassification::classT vehicleclassification::MOTORBIKE;
  const discreteclassification::classT vehicleclassification::NOTMOTORBIKE;
  //#######################################################################
  bool discreteclassification::is(const discreteclassification::classT &_c) {
      bool _is=false;
      switch (_c) {
      case NOCLASSIFICATION:
        _is=true;
        break;
      }
      return _is;
    }
  //#######################################################################
  //#######################################################################
  std::string helmetclassification::str(const classT &_c) {
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
  //#######################################################################
  bool helmetclassification::is(const classT &_c) {
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
  //#######################################################################
  //#######################################################################
  std::string vehicleclassification::str(const classT &_c) {
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
  //#######################################################################
  bool vehicleclassification::is(const classT &_c) {
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
  //#######################################################################
  //#######################################################################
}
