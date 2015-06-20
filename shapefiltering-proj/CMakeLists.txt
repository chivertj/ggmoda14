cmake_minimum_required(VERSION 2.6)
PROJECT( shapefilter )
FIND_PACKAGE( OpenCV REQUIRED )
INCLUDE_DIRECTORIES( ${OPENCV_INCLUDE_DIR} )
SET(CMAKE_CXX_FLAGS "-DDEBUG -g -std=c++11 -O0")
SET(shapefilter_SRCS main.cpp 
  ../mainhelperfuncs.cpp ../src/bwlabel.cpp ../src/connected.cc ../src/region.cpp ../src/ACDefinitions.cpp ../src/gglowpass.cpp ../src/gglphisto.cpp ../src/gglpanalysis.cpp ../src/gghelpers.cpp ../src/csvfiling.cpp ../src/ggheadtracker.cpp ../src/ggtracker.cpp ../src/ggtimewindow.cpp ../src/ggregcompare.cpp ../src/ggsimcorr.cpp ../src/ggutils.cpp ../src/ggmath.cpp ../src/ggregionprops.cpp ../src/ggregiontrack.cpp ../src/ggregiontracker.cpp ../src/ggpropagating.cpp ../src/ggtracks.cpp ../src/ggframe.cpp ../src/ggclassify.cpp ../src/ggregionhierarchy.cpp ../src/ggclassification.cpp ../src/motion/BootStrapProjector.cpp ../src/motion/ClusterPnts.cpp ../src/motion/ggmatlearn.cpp ../src/motion/FeatPnts.cpp ../src/motion/mdBackProjector.cpp ../src/motion/mdMotionCluster.cpp ../src/motion/mdRasterVoronoi.cpp ../src/motion/mdShapeCluster.cpp ../src/motion/mdSpatialCluster.cpp ../src/motion/mkutilsopencv.cpp ../src/motion/MotClustNS.cpp ../src/motion/ggmotionsegment.cpp ../src/ggbgfgsegmenters.cpp ../src/ggexceptions.cpp ../src/gguuid.cpp ../src/ggexporters.cpp ../src/shape/ggshapeextractor.cpp ../src/shape/ggshapetrackproperties.cpp ../src/shape/ggshapetrackfilter.cpp ../src/pca/frbdp.cpp ../src/pca/ggpca.cpp ../src/pca/is2fsrob.cpp ../src/pca/utility.cpp ../src/pca/xjlinalg.cpp ../src/ggidxatt.cpp ../src/shape/ggregconsist.cpp  ../mainhelperfuncs.h ../src/bwlabel.h ../src/connected.cc ../src/region.h ../src/ACDefinitions.h ../src/gglowpass.h ../src/gglphisto.h ../src/gglpanalysis.h ../src/gghelpers.h ../src/csvfiling.h ../src/ggheadtracker.h ../src/ggtracker.h ../src/ggtimewindow.h ../src/ggregcompare.h ../src/ggsimcorr.h ../src/ggutils.h ../src/ggmath.h ../src/ggregionprops.h ../src/ggregiontrack.h ../src/ggregiontracker.h ../src/ggpropagating.h ../src/ggtracks.h ../src/ggframe.h ../src/ggclassify.h ../src/ggregionhierarchy.h ../src/ggclassification.h ../src/motion/BootStrapProjector.h ../src/motion/ClusterPnts.h ../src/motion/ggmatlearn.h ../src/motion/FeatPnts.h ../src/motion/mdBackProjector.h ../src/motion/mdMotionCluster.h ../src/motion/mdRasterVoronoi.h ../src/motion/mdShapeCluster.h ../src/motion/mdSpatialCluster.h ../src/motion/mkutilsopencv.hpp ../src/motion/MotClustNS.h ../src/motion/ggmotionsegment.h ../src/ggbgfgsegmenters.h ../src/ggexceptions.h ../src/gguuid.h ../src/ggexporters.h ../src/shape/ggshapeextractor.h ../src/shape/ggshapetrackproperties.h ../src/shape/ggshapetrackfilter.h ../src/pca/frbdp.h ../src/pca/ggpca.h ../src/pca/is2fsrob.h ../src/pca/utility.h ../src/pca/xjlinalg.h ../src/ggidxatt.h ../src/shape/ggregconsist.h
)
SET(EXECNAME "moda14_gg-shapefilter050")
ADD_EXECUTABLE( ${EXECNAME} ${shapefilter_SRCS} )
TARGET_LINK_LIBRARIES( ${EXECNAME} ${OpenCV_LIBS})




