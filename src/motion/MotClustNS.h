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
#ifndef MOTCLUSTNS_NAMESPACE
#define MOTCLUSTNS_NAMESPACE

#include "../globalCVHeader.h"
#include "../ACDefinitions.h"
using namespace ACDefinitions;

namespace MotClustNS {
const int MAXLKPNTCNT=5000;
const int LKPNTDENSITY=10;
const int HFLKPNTDENSITY=5;
void SumImgs(IplImage **imgs,int noImgs,IplImage *opImg);
void MultImgs(IplImage **imgs,int noImgs,IplImage *opImg);
void ScaleImg(IplImage *ipImg, IplImage *opImg, double scale);
void RenderPnts(IplImage *img, const CvPoint2D32f *pnts, const int *pntlabels, int nopnts);
void ExportImg(int idx, IplImage *opImg);
const CvScalar FGCOL=CV_RGB(0,255,255);
const CvScalar HALOCOL=CV_RGB(255,255,0);
const AC3TUPLE<IMAGEBYTE_T> FGCOL_B(0,255,255);
const AC3TUPLE<IMAGEBYTE_T> HALOCOL_B(255,255,0);
enum MASK {FG,HALO,COMB};
enum MASKT {B,FL};
}
#endif //MOTCLUSTNS_NAMESPACE

