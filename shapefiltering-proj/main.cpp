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
#include "cvaux.h"
#include "cxmisc.h"
#include "../src/globalCVHeader.h"
#include <iostream>
#include <string>
#include "../src/shape/ggshapetrackproperties.h"
#include "../src/ggregiontrack.h"
#include "../src/region.h"
#include "../src/ggframe.h"
#include "../src/shape/jjshapetrackfilter.h"

#include "../src/ggregiontrack.h"

int main(int argc, char** argv) {
  if (argc<3) {
    std::cout <<"Need:\t\t file with list of shapes\n \t\t file with list of corresponding images.\n";
    return -1;
  }
  std::string shapesfilename(argv[1]),imgsfilename(argv[2]);
  region::MREGv shapes;
  region::ReadImages(shapes,shapesfilename);
  region::MREGv imgs;
  region::ReadImages(imgs,imgsfilename);

  if (shapes.size()!=imgs.size()) {
    std::cout <<"Unequal number of shapes and images\n";
    return -1;
  }

  //set image data via void setimgdata(const imgsequencedata &isd) (in ggregiontrack.h)
  //imgsequencedata is in ggframe.h
  gg::imgsequencedata isd;
  for (int i=0;i<imgs.size();i++)
    isd.add(imgs[i]);

  gg::imgtrackproperties itps;
  itps.setimgdata(isd);

  gg::shapetrackproperties stps(itps);
  stps.setshapes(shapes);

  jj::shapetrackfilter stf;
  stf(stps);

  std::string sequencefilename;
    sequencefilename="box-";
    sequencefilename+=stps.uid_str();
    sequencefilename+="-";
    stf.dump(sequencefilename,stf.getboxedshapes());

    sequencefilename="unbox-";
    sequencefilename+=stps.uid_str();
    sequencefilename+="-";
    stf.dump(sequencefilename,stf.getunboxedshapes());

    sequencefilename="origroi-";
    sequencefilename+=stps.uid_str();
    sequencefilename+="-";
    stf.dump(sequencefilename,stf.getorigrois());

    return 0;
}

