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
#ifndef MDRASTERVORONOI_CLASS
#define MDRASTERVORONOI_CLASS

#include "../globalCVHeader.h"
#include <list>

namespace md {

template <typename _Tp, typename _Tl > class LabelledPoint : public cv::Point_<_Tp> {
public:
  LabelledPoint(void) : cv::Point_<_Tp>(0,0),label(0) {}
  LabelledPoint(_Tp _x, _Tp _y): cv::Point_<_Tp>(_x,_y),label(0) {}
  LabelledPoint(_Tp _x, _Tp _y, int _label): cv::Point_<_Tp>(_x,_y),label(_label) {}
  _Tl label;
};

typedef int POS_T;
typedef uchar LAB_T;
typedef LabelledPoint<POS_T,LAB_T> PNT_T;

#if(0)
struct SortedPoint {
  bool operator () (const PNT_T &lhs, const PNT_T &rhs) {
    return lhs.x>rhs.x && lhs.y>rhs.y;
  }
};
#endif
typedef std::list<PNT_T> PNTS;
typedef PNTS::iterator pPNTS;

class mdRasterVoronoi {
public:
  mdRasterVoronoi(const cv::Rect &_rect) : rect(_rect),fgcolor(255),bgcolor(0) {
    storage = cvCreateMemStorage(0);
    subdiv=init_delaunay(storage,rect);
    rasterimg = cv::Mat::zeros(rect.size(),cv::DataType<LAB_T>::type);
  }
  void AddPnt(PNT_T &pnt) {
    cvSubdivDelaunay2DInsert( subdiv, pnt );
    allpoints.push_back(pnt);
  }
  void Tessellate(void) {
    rasterimg=cv::Mat::zeros(rasterimg.size(),rasterimg.type());
    paint_voronoi(subdiv,rasterimg,allpoints);
    //    cv::dilate(rasterimg,rasterimg,cv::Mat(),cv::Point(-1,-1),2);
    //    cv::erode(rasterimg,rasterimg,cv::Mat(),cv::Point(-1,-1),2);
  }
  void ClearReinit(void) {
    cvReleaseMemStorage( &storage );
    //    cvClearSubdivVoronoi2D(subdiv);
    allpoints.clear();

    storage = cvCreateMemStorage(0);
    subdiv=init_delaunay(storage,rect);    
  }
  ~mdRasterVoronoi(void) {
    cvClearSubdivVoronoi2D(subdiv);
    cvReleaseMemStorage( &storage );
    allpoints.clear();
  }
  const cv::Mat& Get(void) { return rasterimg; }
  const PNTS& GetPnts(void) { return allpoints; }
protected:
  //##########################################
  CvSubdiv2D* init_delaunay( CvMemStorage* _storage,
			     CvRect _rect ) {
    CvSubdiv2D* _subdiv;
    
    _subdiv = cvCreateSubdiv2D( CV_SEQ_KIND_SUBDIV2D, sizeof(*_subdiv),
				sizeof(CvSubdiv2DPoint),
				sizeof(CvQuadEdge2D),
				_storage );
    cvInitSubdivDelaunay2D( _subdiv, _rect );

    return _subdiv;
  }
  //##########################################
  void paint_voronoi( CvSubdiv2D* _subdiv, cv::Mat &_img, const PNTS &_pnts ) {
    CvSeqReader  reader;
    int i, total = subdiv->edges->total;
    int elem_size = subdiv->edges->elem_size;
    cvCalcSubdivVoronoi2D( _subdiv );
    cvStartReadSeq( (CvSeq*)(_subdiv->edges), &reader, 0 );
    for( i = 0; i < total; i++ ) {
      CvQuadEdge2D* edge = (CvQuadEdge2D*)(reader.ptr);
      if( CV_IS_SET_ELEM( edge )) {
	CvSubdiv2DEdge e = (CvSubdiv2DEdge)edge;
	// left
	draw_subdiv_facet( _img, cvSubdiv2DRotateEdge( e, 1 ), _pnts);
	// right
	draw_subdiv_facet( _img, cvSubdiv2DRotateEdge( e, 3 ), _pnts);
      }
      CV_NEXT_SEQ_ELEM( elem_size, reader );
    }
  }
  //##########################################
  void draw_subdiv_facet( cv::Mat &_img, CvSubdiv2DEdge edge, const PNTS &_pnts ) {
    CvSubdiv2DEdge t = edge;
    int i, count = 0;
    do {
      count++;
      t = cvSubdiv2DGetEdge( t, CV_NEXT_AROUND_LEFT );
    } while (t != edge );
    
    cv::Point edgepts[count];
    // gather points
    t = edge;
    for( i = 0; i < count; i++ ) {
      CvSubdiv2DPoint* pt = cvSubdiv2DEdgeOrg( t );
      if( !pt ) break;
      edgepts[i]=cv::Point( cvRound(pt->pt.x), cvRound(pt->pt.y));
      t = cvSubdiv2DGetEdge( t, CV_NEXT_AROUND_LEFT );
    }
    if( i == count ) {
      CvSubdiv2DPoint* pt = cvSubdiv2DEdgeDst( cvSubdiv2DRotateEdge( edge, 1 ));
      PNTS::const_iterator labpnt=std::find(_pnts.begin(),_pnts.end(),PNT_T(pt->pt.x,pt->pt.y));
      if ((*labpnt).label!=0)
	//	cv::fillConvexPoly( _img, edgepts, count, bgcolor, CV_AA, 0 );
	//	cv::fillConvexPoly( _img, edgepts, count, bgcolor, 8, 0 );
	//      else
	cv::fillConvexPoly( _img, edgepts, count, fgcolor, 8, 0 );	
	//	cv::fillConvexPoly( _img, edgepts, count, fgcolor, CV_AA, 0 );	
      //      cv::circle( _img, cvPoint(cvRound(pt->pt.x), cvRound(pt->pt.y)), 3, cv::Scalar(0), CV_FILLED, 8, 0 );
    }
  }
  //###################################

  CvMemStorage *storage;
  cv::Rect rect;
  CvSubdiv2D *subdiv;
  PNTS allpoints;
  cv::Mat rasterimg;
  cv::Scalar fgcolor,bgcolor;
};

}
#endif //MDRASTERVORONOI_CLASS

